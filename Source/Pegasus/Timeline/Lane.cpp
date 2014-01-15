/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Lane.cpp
//! \author	Kevin Boulanger
//! \date	07th November 2013
//! \brief	Timeline lane management, manages a set of blocks on one lane of the timeline

#include "Pegasus/Timeline/Lane.h"
#include "Pegasus/Timeline/Block.h"

#if PEGASUS_ENABLE_PROXIES
#include "Pegasus/Timeline/LaneProxy.h"
#include "Pegasus/Timeline/BlockProxy.h"
#include "Pegasus/Timeline/Shared/IBlockProxy.h"
#include <string.h>
#endif  // PEGASUS_ENABLE_PROXIES

namespace Pegasus {
namespace Timeline {


Lane::Lane(Alloc::IAllocator * allocator)
:   mAllocator(allocator),
    mFirstBlockIndex(0),
    mNumBlocks(0)
{
    PG_ASSERTSTR(allocator != nullptr, "Invalid allocator given to a timeline Lane object");

    // Set all block records as invalid
    for (unsigned int b = 0; b < LANE_MAX_NUM_BLOCKS; ++b)
    {
        mBlockRecords[b].mBlock = nullptr;
        mBlockRecords[b].mNext = INVALID_RECORD_INDEX;
    }

    // Create the empty linked list
    mBlockRecords[0].mNext = 0; // == mFirstBlockIndex

#if PEGASUS_ENABLE_PROXIES
    // Create the proxy associated with the lane
    mProxy = PG_NEW(allocator, -1, "Timeline::Lane::mProxy", Pegasus::Alloc::PG_MEM_PERM) LaneProxy(this);

    mName[0] = '\0';
#endif  // PEGASUS_ENABLE_PROXIES
}

//----------------------------------------------------------------------------------------

Lane::~Lane()
{
    // Destroy the blocks allocated in the lane
    for (unsigned int b = 0; b < LANE_MAX_NUM_BLOCKS; ++b)
    {
        if (mBlockRecords[b].mBlock != nullptr)
        {
            PG_DELETE(mAllocator, mBlockRecords[b].mBlock);
        }
    }

#if PEGASUS_ENABLE_PROXIES
    // Destroy the proxy associated with the lane
    PG_DELETE(mAllocator, mProxy);
#endif
}

//----------------------------------------------------------------------------------------

bool Lane::InsertBlock(Block * block, float position, float length)
{
#if PEGASUS_ENABLE_PROXIES
    if (block != nullptr)
    {
        PG_LOG('TMLN', "Adding the block \"%s\" of length %f to the lane \"%s\", at position %f",
               block->GetEditorString(), length, GetName(), position);
    }
    else
#endif
    {
        PG_LOG('TMLN', "Adding a block of length %f to a lane, at position %f", length, position);
    }

    if (block != nullptr)
    {
        // Configure the block
        block->SetPosition(position);
        block->SetLength(length);
        block->SetLane(this);

        // Insert the pre-configured block
        return InsertBlock(block);
    }
    else
    {
#if PEGASUS_ENABLE_PROXIES
        PG_FAILSTR("Unable to insert a block to the lane \"%s\", the block is invalid", GetName());
#else
        PG_FAILSTR("Unable to insert a block to a lane, the block is invalid");
#endif

        return false;
    }
}

//----------------------------------------------------------------------------------------

void Lane::RemoveBlock(Block * block)
{
#if PEGASUS_ENABLE_PROXIES
    if (block != nullptr)
    {
        PG_LOG('TMLN', "Removing the block \"%s\" from the lane \"%s\"", block->GetEditorString(), GetName());
    }
    else
#endif
    {
        PG_LOG('TMLN', "Removing a block from a lane");
    }

    if (block != nullptr)
    {
        // Find the block in the linked list
        const int blockIndex = FindBlockIndex(block);

        // Remove the block by index
        RemoveBlock(blockIndex);
    }
    else
    {
        PG_FAILSTR("Unable to remove a block from the lane, the block is invalid");
    }
}

//----------------------------------------------------------------------------------------

void Lane::SetBlockPosition(Block * block, float position)
{
#if PEGASUS_ENABLE_PROXIES
    if (block != nullptr)
    {
        PG_LOG('TMLN', "Moving the block \"%s\" to position %f", block->GetEditorString(), position);
    }
    else
#endif
    {
        PG_LOG('TMLN', "Moving a block to position %f", position);
    }

    if (block != nullptr)
    {
        // Find the block in the linked list
        const int blockIndex = FindBlockIndex(block);

        // Move the block by index
        SetBlockPosition(blockIndex, position);
    }
    else
    {
        PG_FAILSTR("Unable to move a block, the block is invalid");
    }
}

//----------------------------------------------------------------------------------------

void Lane::SetBlockLength(Block * block, float length)
{
#if PEGASUS_ENABLE_PROXIES
    if (block != nullptr)
    {
        PG_LOG('TMLN', "Resizing the block \"%s\" to length %f", block->GetEditorString(), length);
    }
    else
#endif
    {
        PG_LOG('TMLN', "Resizing the block to length %f", length);
    }

    if (block != nullptr)
    {
        // Find the block in the linked list
        const int blockIndex = FindBlockIndex(block);

        // Resize the block by index
        SetBlockLength(blockIndex, length);
    }
    else
    {
        PG_FAILSTR("Unable to resize a block, the block is invalid");
    }
}

//----------------------------------------------------------------------------------------

void Lane::InitializeBlocks()
{
    if (mNumBlocks > 0)
    {
        // Call Initialize() for each block
        int currentIndex = mFirstBlockIndex;
        do
        {
            mBlockRecords[currentIndex].mBlock->Initialize();
            currentIndex = mBlockRecords[currentIndex].mNext;
        }
        while (currentIndex != mFirstBlockIndex);
    }
}

//----------------------------------------------------------------------------------------

void Lane::Render(float beat, Wnd::Window * window)
{
    //! \todo The current approach is extremely brute force and inefficient.
    //! \todo Cache the current selected block in function of the timeline time.

    if (mNumBlocks > 0)
    {
        int currentBlockIndex = INVALID_RECORD_INDEX;
        int nextBlockIndex = INVALID_RECORD_INDEX;
        FindCurrentAndNextBlocks(beat, currentBlockIndex, nextBlockIndex);
        if (currentBlockIndex != INVALID_RECORD_INDEX)
        {
            Block * const block = mBlockRecords[currentBlockIndex].mBlock;
            float relativeBeat = beat - block->GetPosition();
            if (relativeBeat < 0.0f)
            {
                relativeBeat = 0.0f;
            }
            if (relativeBeat < block->GetLength())
            {
                block->Render(relativeBeat, window);
            }
        }
    }
}

//----------------------------------------------------------------------------------------

#if PEGASUS_ENABLE_PROXIES

unsigned int Lane::GetBlocks(IBlockProxy ** blocks) const
{
    if (blocks != nullptr)
    {
        if (mNumBlocks == 0)
        {
            return 0;
        }
        else
        {
            // Copy the entire linked list into the output array
            unsigned int numBlocks = 0;
            int currentIndex = mFirstBlockIndex;
            do
            {
                blocks[numBlocks++] = mBlockRecords[currentIndex].mBlock->GetProxy();
                currentIndex = mBlockRecords[currentIndex].mNext;
            }
            while (currentIndex != mFirstBlockIndex);
            return numBlocks;
        }
    }
    else
    {
        PG_FAILSTR("Unable to get list of blocks for a lane, the resulting array is nullptr");
        return 0;
    }
}

//----------------------------------------------------------------------------------------

void Lane::SetName(const char * name)
{
    if (name == nullptr)
    {
        PG_LOG('TMLN', "Erasing the name of the lane \"%s\"", mName);
        mName[0] = '\0';
    }
    else
    {
        PG_LOG('TMLN', "Setting the name of the lane \"%s\" to \"%s\"", mName, name);
#if PEGASUS_COMPILER_MSVC
        strncpy_s(mName, MAX_NAME_LENGTH, name, MAX_NAME_LENGTH);
#else
        mName[MAX_NAME_LENGTH - 1] = '\0';
        strncpy(mName, name, MAX_NAME_LENGTH - 1);
#endif  // PEGASUS_COMPILER_MSVC
    }

}
#endif  // PEGASUS_ENABLE_PROXIES


//----------------------------------------------------------------------------------------

#if PEGASUS_DEBUG
void Lane::DumpToLog()
{
#if PEGASUS_ENABLE_PROXIES
    PG_LOG('TMLN', "Content of the timeline lane (format: (index,position,length,name)):");
#else
    PG_LOG('TMLN', "Content of the timeline lane (format: (index,position,length)):");
#endif

    if (mNumBlocks == 0)
    {
        PG_LOG('TMLN', "    EMPTY")
    }
    else
    {
        int currentIndex = mFirstBlockIndex;
        do
        {
#if PEGASUS_ENABLE_PROXIES
            PG_LOG('TMLN', "(%d, %f, %f, %s)",
                   currentIndex,
                   mBlockRecords[currentIndex].mBlock->GetPosition(),
                   mBlockRecords[currentIndex].mBlock->GetLength(),
                   mBlockRecords[currentIndex].mBlock->GetEditorString());
#else
            PG_LOG('TMLN', "(%d, %f, %f)",
                   currentIndex,
                   mBlockRecords[currentIndex].mBlock->GetPosition(),
                   mBlockRecords[currentIndex].mBlock->GetLength());
#endif
            currentIndex = mBlockRecords[currentIndex].mNext;
        }
        while (currentIndex != mFirstBlockIndex);
    }
}
#endif  // PEGASUS_DEBUG

//----------------------------------------------------------------------------------------

void Lane::FindCurrentAndNextBlocks(float beat, int & currentBlockIndex, int & nextBlockIndex) const
{
    currentBlockIndex = INVALID_RECORD_INDEX;
    nextBlockIndex = INVALID_RECORD_INDEX;

    // If the list of blocks is empty, no valid index can be returned
    if (mNumBlocks == 0)
    {
        PG_ASSERTSTR(mBlockRecords[mFirstBlockIndex].mBlock == nullptr, "Invalid first block record in the lane, it is supposed to be nullptr");
        return;
    }

    // Test if we are before the first block
    PG_ASSERTSTR(mBlockRecords[mFirstBlockIndex].mBlock != nullptr, "Invalid first block record in the lane, it is supposed to be a valid one");
    if (beat < mBlockRecords[mFirstBlockIndex].mBlock->GetPosition())
    {
        nextBlockIndex = mFirstBlockIndex;
        return;
    }

    // Look for the space between two blocks where the beat can fit
    int current = mFirstBlockIndex;
    int next = mBlockRecords[mFirstBlockIndex].mNext;
    PG_ASSERTSTR(mBlockRecords[next].mBlock != nullptr, "Invalid block record in the lane, it is supposed to be a valid one");
    while (   (next != mFirstBlockIndex)
           && (beat >= mBlockRecords[next].mBlock->GetPosition()) )
    {
        current = next;
        next = mBlockRecords[next].mNext;
        PG_ASSERTSTR(next != INVALID_RECORD_INDEX, "Invalid block record in the lane, it is supposed to have a next record");
        PG_ASSERTSTR(mBlockRecords[next].mBlock != nullptr, "Invalid block record in the lane, it is supposed to be a valid one");
    }

    // Output the block indexes
    currentBlockIndex = current;
    if (next != mFirstBlockIndex)
    {
        nextBlockIndex = next;
    }
}

//----------------------------------------------------------------------------------------

int Lane::FindFirstAvailableBlockRecord() const
{
    // No free block record found
    if (mNumBlocks >= LANE_MAX_NUM_BLOCKS)
    {
        return INVALID_RECORD_INDEX;
    }

    // Look for an available block record
    for (unsigned int b = 0; b < LANE_MAX_NUM_BLOCKS; ++b)
    {
        if (mBlockRecords[b].mNext == INVALID_RECORD_INDEX)
        {
            // Free block found
            PG_ASSERTSTR(mBlockRecords[b].mBlock == nullptr, "Invalid block record in the lane, it is supposed to an invalid one");
            return static_cast<int>(b);
        }
    }

    PG_FAILSTR("Invalid block record(s) in the lane, at least one is supposed to be invalid");
    return INVALID_RECORD_INDEX;
}

//----------------------------------------------------------------------------------------

int Lane::FindBlockIndex(Block * block) const
{
    if (block != nullptr)
    {
        if (mNumBlocks == 0)
        {
            return INVALID_RECORD_INDEX;
        }
        else
        {
            int currentIndex = mFirstBlockIndex;
            do
            {
                if (mBlockRecords[currentIndex].mBlock == block)
                {
                    // Block found
                    return currentIndex;
                }
                currentIndex = mBlockRecords[currentIndex].mNext;
            }
            while (currentIndex != mFirstBlockIndex);
        }

        // Block not found
        PG_FAILSTR("Unable to find the block in the lane");
        return INVALID_RECORD_INDEX;
    }
    else
    {
        PG_FAILSTR("Unable to find an invalid block in the lane");
        return INVALID_RECORD_INDEX;
    }
}

//----------------------------------------------------------------------------------------

bool Lane::InsertBlock(Block * block)
{
    if (block != nullptr)
    {
        if (mNumBlocks < LANE_MAX_NUM_BLOCKS)
        {
            // Find where to insert the block
            int currentBlockIndex, nextBlockIndex;
            FindCurrentAndNextBlocks(block->GetPosition(), currentBlockIndex, nextBlockIndex);

            if (currentBlockIndex == INVALID_RECORD_INDEX)
            {
                if (nextBlockIndex == INVALID_RECORD_INDEX)
                {
                    PG_ASSERTSTR(mNumBlocks == 0, "Internal error when adding a block to a lane");

                    // Empty list, use the first block
                    mBlockRecords[mFirstBlockIndex].mBlock = block;
                    mBlockRecords[mFirstBlockIndex].mNext = mFirstBlockIndex;

                    ++mNumBlocks;
                    return true;
                }
                else
                {
                    PG_ASSERTSTR(nextBlockIndex == mFirstBlockIndex, "Internal error when adding a block to a lane");

                    // Insert in front of the existing list
                    currentBlockIndex = mFirstBlockIndex;
                    nextBlockIndex = FindFirstAvailableBlockRecord();
                    if (nextBlockIndex != INVALID_RECORD_INDEX)
                    {
                        // Move the content of the first block to the new record
                        mBlockRecords[nextBlockIndex].mBlock = mBlockRecords[currentBlockIndex].mBlock;
                        mBlockRecords[nextBlockIndex].mNext = mBlockRecords[currentBlockIndex].mNext;

                        // Re-use the first block record for the new block
                        mBlockRecords[currentBlockIndex].mBlock = block;
                        mBlockRecords[currentBlockIndex].mNext = nextBlockIndex;

                        ++mNumBlocks;
                        return true;
                    }
                    else
                    {
                        PG_FAILSTR("Unable to find an empty record for a block in a lane");
                    }
                }
            }
            else
            {
                if (nextBlockIndex == INVALID_RECORD_INDEX)
                {
                    // Insert at the end of the existing list
                    nextBlockIndex = FindFirstAvailableBlockRecord();
                    if (nextBlockIndex != INVALID_RECORD_INDEX)
                    {
                        // Fill the new block
                        mBlockRecords[nextBlockIndex].mBlock = block;
                        mBlockRecords[nextBlockIndex].mNext = mFirstBlockIndex;

                        // Redirect the previous last block to the new block
                        mBlockRecords[currentBlockIndex].mNext = nextBlockIndex;

                        ++mNumBlocks;
                        return true;
                    }
                    else
                    {
                        PG_FAILSTR("Unable to find an empty record for a block in a lane");
                    }
                }
                else
                {
                    // Insert somewhere in the middle of the list
                    nextBlockIndex = FindFirstAvailableBlockRecord();
                    if (nextBlockIndex != INVALID_RECORD_INDEX)
                    {
                        PG_ASSERTSTR(nextBlockIndex != currentBlockIndex, "Internal error when adding a block to a lane");

                        // Fill the new block and redirect it to the previously next block
                        mBlockRecords[nextBlockIndex].mBlock = block;
                        mBlockRecords[nextBlockIndex].mNext = mBlockRecords[currentBlockIndex].mNext;

                        // Redirect previously current block to the new block
                        mBlockRecords[currentBlockIndex].mNext = nextBlockIndex;

                        ++mNumBlocks;
                        return true;
                    }
                    else
                    {
                        PG_FAILSTR("Unable to find an empty record for a block in a lane");
                    }
                }
            }
        }
        else
        {
            PG_FAILSTR("Unable to add a block to a lane, the maximum number of blocks has been reached (%d)", LANE_MAX_NUM_BLOCKS);
        }
    }
    else
    {
        PG_FAILSTR("Unable to add a block to the lane, the block is invalid");
    }

    return false;
}

//----------------------------------------------------------------------------------------

void Lane::RemoveBlock(int blockIndex)
{
    if (   (blockIndex >= 0)
        && (blockIndex < LANE_MAX_NUM_BLOCKS) )
    {
        PG_ASSERTSTR(mNumBlocks >= 1, "Internal error when removing a block from a lane");

        if (mNumBlocks == 1)
        {
            PG_ASSERTSTR(blockIndex == mFirstBlockIndex, "Internal error when removing a block from a lane");

            // Remove the only record
            mBlockRecords[blockIndex].mBlock = nullptr;
        }
        else
        {
            // Copy the next block record to the current one,
            // making the current block point the next after the next one
            const int nextBlockIndex = mBlockRecords[blockIndex].mNext;
            mBlockRecords[blockIndex].mBlock = mBlockRecords[nextBlockIndex].mBlock;
            mBlockRecords[blockIndex].mNext = mBlockRecords[nextBlockIndex].mNext;

            // Delete the next record as it is not used anymore
            mBlockRecords[nextBlockIndex].mBlock = nullptr;
            mBlockRecords[nextBlockIndex].mNext = INVALID_RECORD_INDEX;

            // If the next record was the first one in the list,
            // update the index of the first block
            if (nextBlockIndex == mFirstBlockIndex)
            {
                mFirstBlockIndex = blockIndex;
            }
        }

        --mNumBlocks;
    }
    else
    {
        PG_FAILSTR("Invalid block record index (%d), it should be >= 0 and < %d", blockIndex, LANE_MAX_NUM_BLOCKS);
    }
}

//----------------------------------------------------------------------------------------

void Lane::SetBlockPosition(int blockIndex, float position)
{
    if (   (blockIndex >= 0)
        && (blockIndex < LANE_MAX_NUM_BLOCKS) )
    {
        PG_ASSERTSTR(mNumBlocks >= 1, "Internal error when moving a block");

        // Get a pointer to the block
        Block * block = mBlockRecords[blockIndex].mBlock;
        PG_ASSERTSTR(block != nullptr, "Internal error when moving a block");

        // Remove the block from the lane so we do not mess up the sorting of the linked list
        RemoveBlock(blockIndex);

        // Set the new position of the block
        block->SetPosition(position);

        // Insert the block back to the linked list, the proper sorting is guaranteed
        InsertBlock(block);
    }
    else
    {
        PG_FAILSTR("Invalid block record index (%d), it should be >= 0 and < %d", blockIndex, LANE_MAX_NUM_BLOCKS);
    }
}

//----------------------------------------------------------------------------------------

void Lane::SetBlockLength(int blockIndex, float length)
{
    if (   (blockIndex >= 0)
        && (blockIndex < LANE_MAX_NUM_BLOCKS) )
    {
        PG_ASSERTSTR(mNumBlocks >= 1, "Internal error when resizing a block");

        // Get a pointer to the block
        Block * block = mBlockRecords[blockIndex].mBlock;
        PG_ASSERTSTR(block != nullptr, "Internal error when resizing a block");

        // Remove the block from the lane so we do not mess up the sorting of the linked list
        RemoveBlock(blockIndex);

        // Set the new length of the block
        block->SetLength(length);

        // Insert the block back to the linked list, the proper sorting is guaranteed
        InsertBlock(block);
    }
    else
    {
        PG_FAILSTR("Invalid block record index (%d), it should be >= 0 and < %d", blockIndex, LANE_MAX_NUM_BLOCKS);
    }
}


}   // namespace Timeline
}   // namespace Pegasus
