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
#include "Pegasus/Math/Scalar.h"

#if PEGASUS_ENABLE_PROXIES
#include "Pegasus/Timeline/Proxy/BlockProxy.h"
#include "Pegasus/Timeline/Shared/IBlockProxy.h"
#include <string.h>
#endif  // PEGASUS_ENABLE_PROXIES

namespace Pegasus {
namespace Timeline {


Lane::Lane(Alloc::IAllocator * allocator, Timeline * timeline)
:   mAllocator(allocator)
,   mTimeline(timeline)
,   mFirstBlockIndex(0)
,   mNumBlocks(0)
#if PEGASUS_ENABLE_PROXIES
,   mProxy(this)
#endif
{
    PG_ASSERTSTR(allocator != nullptr, "Invalid allocator given to a timeline Lane object");
    PG_ASSERTSTR(timeline != nullptr, "Invalid timeline given to a timeline Lane object");

    // Set all block records as invalid
    for (unsigned int b = 0; b < LANE_MAX_NUM_BLOCKS; ++b)
    {
        mBlockRecords[b].mBlock = nullptr;
        mBlockRecords[b].mNext = INVALID_RECORD_INDEX;
    }

    // Create the empty linked list
    mBlockRecords[0].mNext = 0; // == mFirstBlockIndex

#if PEGASUS_ENABLE_PROXIES
    mName[0] = '\0';
#endif
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
}

//----------------------------------------------------------------------------------------

bool Lane::InsertBlock(Block * block, Beat beat, Duration duration)
{
#if PEGASUS_ENABLE_PROXIES
    if (block != nullptr)
    {
        PG_LOG('TMLN', "Adding the block \"%s\" of duration %u to the lane \"%s\", at position %u",
               block->GetEditorString(), duration, GetName(), beat);
    }
    else
#endif
    {
        PG_LOG('TMLN', "Adding a block of length %u to a lane, at position %u", duration, beat);
    }

    if (block != nullptr)
    {
        // Configure the block
        block->SetBeat(beat);
        block->SetDuration(duration);
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

void Lane::SetBlockBeat(Block * block, Beat beat)
{
#if PEGASUS_ENABLE_PROXIES
    if (block != nullptr)
    {
        PG_LOG('TMLN', "Moving the block \"%s\" to position %u", block->GetEditorString(), beat);
    }
    else
#endif
    {
        PG_LOG('TMLN', "Moving a block to position %u", beat);
    }

    if (block != nullptr)
    {
        // Find the block in the linked list
        const int blockIndex = FindBlockIndex(block);

        // Move the block by index
        SetBlockBeat(blockIndex, beat);
    }
    else
    {
        PG_FAILSTR("Unable to move a block, the block is invalid");
    }
}

//----------------------------------------------------------------------------------------

void Lane::SetBlockDuration(Block * block, Duration duration)
{
#if PEGASUS_ENABLE_PROXIES
    if (block != nullptr)
    {
        PG_LOG('TMLN', "Resizing the block \"%s\" to length %u", block->GetEditorString(), duration);
    }
    else
#endif
    {
        PG_LOG('TMLN', "Resizing the block to length %u", duration);
    }

    if (block != nullptr)
    {
        // Find the block in the linked list
        const int blockIndex = FindBlockIndex(block);

        // Resize the block by index
        SetBlockDuration(blockIndex, duration);
    }
    else
    {
        PG_FAILSTR("Unable to resize a block, the block is invalid");
    }
}

//----------------------------------------------------------------------------------------

bool Lane::IsBlockFitting(Block * block, Beat beat, Duration duration) const
{
    // If the lane is full, the block does not fit
    if (mNumBlocks >= LANE_MAX_NUM_BLOCKS)
    {
        return false;
    }

    // If the lane is empty, the block always fit
    if (mNumBlocks == 0)
    {
        return true;
    }

    //! \todo Add support for limited length timeline

    // Find the current block index (block the tick is on or right after)
    // and the next block index (block that is not started yet)
    int currentBlockIndex = INVALID_RECORD_INDEX;
    int nextBlockIndex = INVALID_RECORD_INDEX;
    FindCurrentAndNextBlocks(beat, currentBlockIndex, nextBlockIndex);

    // Test if the input block belongs to the lane
    const int testBlockIndex = FindBlockIndex(block);
    if (testBlockIndex != INVALID_RECORD_INDEX)
    {
        if (currentBlockIndex == testBlockIndex)
        {
            // If the current block is the tested one, find the previous block
            currentBlockIndex = FindPreviousBlockIndex(testBlockIndex);
        }
        else if (nextBlockIndex == testBlockIndex)
        {
            // If the next block is the tested one, find the next block
            nextBlockIndex = FindNextBlockIndex(testBlockIndex);
        }
    }

    if (currentBlockIndex == INVALID_RECORD_INDEX)
    {
        if (nextBlockIndex != INVALID_RECORD_INDEX)
        {
            // If before the first block, make sure the duration does not go too far
            Block * const nextBlock = mBlockRecords[nextBlockIndex].mBlock;
            return (beat + duration) <= nextBlock->GetBeat();
        }
        else
        {
            // If the tested block is the only one in the lane, the fit is possible
            return true;
        }
    }
    else if (nextBlockIndex == INVALID_RECORD_INDEX)
    {
        // If after the last block, make sure we do not collide with the last one
        Block * const currentBlock = mBlockRecords[currentBlockIndex].mBlock;
        return beat >= (currentBlock->GetBeat() + currentBlock->GetDuration());
    }
    else
    {
        // If between two blocks, make sure the beginning of the block does not collide
        // with the current block, and that the duration does not collide with the next one
        Block * const currentBlock = mBlockRecords[currentBlockIndex].mBlock;
        Block * const nextBlock = mBlockRecords[nextBlockIndex].mBlock;
        return   ( beat >= (currentBlock->GetBeat() + currentBlock->GetDuration()) )
              && ( (beat + duration) <= nextBlock->GetBeat() );
    }
}

//----------------------------------------------------------------------------------------

void Lane::MoveBlockToLane(Block * block, Lane * newLane, Beat beat)
{
#if PEGASUS_ENABLE_PROXIES
    if ((block != nullptr) && (newLane != nullptr))
    {
        PG_LOG('TMLN', "Moving the block \"%s\" to the lane \"%s\" at position %u", block->GetEditorString(), newLane->GetName(), beat);
    }
    else
#endif
    {
        PG_LOG('TMLN', "Moving a block to another lane at position %u", beat);
    }

    if (block == nullptr)
    {
        PG_FAILSTR("Unable to move a block to another lane, the block is invalid");
        return;
    }
    else if (newLane == nullptr)
    {
        PG_FAILSTR("Unable to move a block to another lane, the lane is invalid");
        return;
    }
    else if (newLane == this)
    {
        PG_FAILSTR("Unable to move a block to another lane, the other lane is the current lane");
        return;
    }

    // Find the block in the linked list
    const int blockIndex = FindBlockIndex(block);

    // Resize the block by index
    MoveBlockToLane(blockIndex, newLane, beat);
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
            BlockRecord& blockRecord =  mBlockRecords[currentIndex];
            Block* block = blockRecord.mBlock;
            block->Initialize();
            if (block->HasScript())
            {
                block->InitializeScript(); 
            }
            currentIndex = mBlockRecords[currentIndex].mNext;
        }
        while (currentIndex != mFirstBlockIndex);
    }
}

//----------------------------------------------------------------------------------------

void Lane::UninitializeBlocks()
{
    if (mNumBlocks > 0)
    {
        // Call Initialize() for each block
        int currentIndex = mFirstBlockIndex;
        do
        {
            BlockRecord& blockRecord =  mBlockRecords[currentIndex];
            Block* block = blockRecord.mBlock;
            block->Shutdown();
            currentIndex = mBlockRecords[currentIndex].mNext;
        }
        while (currentIndex != mFirstBlockIndex);
    }
}

//----------------------------------------------------------------------------------------
void Lane::Update(float beat, Wnd::Window * window)
{
    //! \todo The current approach is extremely brute force and inefficient.
    //! \todo Cache the current selected block in function of the timeline time.

    Block * block = nullptr;
    float relativeBeat = 0.0f;
    if (FindBlockAndComputeRelativeBeat(beat, block, relativeBeat))
    {
        block->Update(relativeBeat, window);
    }
}

//----------------------------------------------------------------------------------------

void Lane::Render(float beat, Wnd::Window * window)
{
    //! \todo The current approach is extremely brute force and inefficient.
    //! \todo Cache the current selected block in function of the timeline time.

    Block * block = nullptr;
    float relativeBeat = 0.0f;
    if (FindBlockAndComputeRelativeBeat(beat, block, relativeBeat))
    {
        block->Render(relativeBeat, window);
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
            PG_LOG('TMLN', "(%d, %u, %u, %s)",
                   currentIndex,
                   mBlockRecords[currentIndex].mBlock->GetBeat(),
                   mBlockRecords[currentIndex].mBlock->GetDuration(),
                   mBlockRecords[currentIndex].mBlock->GetEditorString());
#else
            PG_LOG('TMLN', "(%d, %u, %u)",
                   currentIndex,
                   mBlockRecords[currentIndex].mBlock->GetBeat(),
                   mBlockRecords[currentIndex].mBlock->GetDuration());
#endif
            currentIndex = mBlockRecords[currentIndex].mNext;
        }
        while (currentIndex != mFirstBlockIndex);
    }
}
#endif  // PEGASUS_DEBUG

//----------------------------------------------------------------------------------------

int Lane::FindCurrentBlock(Beat beat) const
{
    // If the list of blocks is empty, no valid index can be returned
    if (mNumBlocks == 0)
    {
        PG_ASSERTSTR(mBlockRecords[mFirstBlockIndex].mBlock == nullptr, "Invalid first block record in the lane, it is supposed to be nullptr");
        return INVALID_RECORD_INDEX;
    }

    // Test if we are before the first block
    PG_ASSERTSTR(mBlockRecords[mFirstBlockIndex].mBlock != nullptr, "Invalid first block record in the lane, it is supposed to be a valid one");
    if (beat < mBlockRecords[mFirstBlockIndex].mBlock->GetBeat())
    {
        return INVALID_RECORD_INDEX;
    }

    // Look for the space between two blocks where the beat can fit
    int current = mFirstBlockIndex;
    int next = mBlockRecords[mFirstBlockIndex].mNext;
    PG_ASSERTSTR(mBlockRecords[next].mBlock != nullptr, "Invalid block record in the lane, it is supposed to be a valid one");
    while (   (next != mFirstBlockIndex)
           && (beat >= mBlockRecords[next].mBlock->GetBeat()) )
    {
        current = next;
        next = mBlockRecords[next].mNext;
        PG_ASSERTSTR(next != INVALID_RECORD_INDEX, "Invalid block record in the lane, it is supposed to have a next record");
        PG_ASSERTSTR(mBlockRecords[next].mBlock != nullptr, "Invalid block record in the lane, it is supposed to be a valid one");
    }

    // Output the block indexes
    return current;
}

//----------------------------------------------------------------------------------------

void Lane::FindCurrentAndNextBlocks(Beat beat, int & currentBlockIndex, int & nextBlockIndex) const
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
    if (beat < mBlockRecords[mFirstBlockIndex].mBlock->GetBeat())
    {
        nextBlockIndex = mFirstBlockIndex;
        return;
    }

    // Look for the space between two blocks where the beat can fit
    int current = mFirstBlockIndex;
    int next = mBlockRecords[mFirstBlockIndex].mNext;
    PG_ASSERTSTR(mBlockRecords[next].mBlock != nullptr, "Invalid block record in the lane, it is supposed to be a valid one");
    while (   (next != mFirstBlockIndex)
           && (beat >= mBlockRecords[next].mBlock->GetBeat()) )
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
        return INVALID_RECORD_INDEX;
    }
    else
    {
        return INVALID_RECORD_INDEX;
    }
}

//----------------------------------------------------------------------------------------

int Lane::FindPreviousBlockIndex(int blockIndex) const
{
    if ((blockIndex  >= 0) && (blockIndex < LANE_MAX_NUM_BLOCKS))
    {
        Block * const block = mBlockRecords[blockIndex].mBlock;
        if (block != nullptr)
        {
            if (blockIndex == mFirstBlockIndex)
            {
                // If the tested block is the first one, there is no previous block
                return INVALID_RECORD_INDEX;
            }
            else
            {
                // Search for the previous block
                int currentIndex = mFirstBlockIndex;
                do
                {
                    if (mBlockRecords[currentIndex].mNext == blockIndex)
                    {
                        // Previous block found
                        return currentIndex;
                    }
                    currentIndex = mBlockRecords[currentIndex].mNext;
                }
                while (currentIndex != mFirstBlockIndex);

                PG_FAILSTR("Internal error when looking for the previous block of the block of index %d", blockIndex);
                return INVALID_RECORD_INDEX;
            }
        }
        else
        {
            PG_FAILSTR("Invalid block record index (%d), it should be the one of an allocated block", blockIndex);
            return INVALID_RECORD_INDEX;
        }
    }
    else
    {
        PG_FAILSTR("Invalid block record index (%d), it should be >= 0 and < %u", blockIndex, LANE_MAX_NUM_BLOCKS);
        return INVALID_RECORD_INDEX;
    }
}

//----------------------------------------------------------------------------------------

int Lane::FindNextBlockIndex(int blockIndex) const
{
    if ((blockIndex  >= 0) && (blockIndex < LANE_MAX_NUM_BLOCKS))
    {
        Block * const block = mBlockRecords[blockIndex].mBlock;
        if (block != nullptr)
        {
            if (mBlockRecords[blockIndex].mNext != mFirstBlockIndex)
            {
                // Next block found
                return mBlockRecords[blockIndex].mNext;
            }
            else
            {
                // Next block not found
                return INVALID_RECORD_INDEX;
            }
        }
        else
        {
            PG_FAILSTR("Invalid block record index (%d), it should be the one of an allocated block", blockIndex);
            return INVALID_RECORD_INDEX;
        }
    }
    else
    {
        PG_FAILSTR("Invalid block record index (%d), it should be >= 0 and < %u", blockIndex, LANE_MAX_NUM_BLOCKS);
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
            FindCurrentAndNextBlocks(block->GetBeat(), currentBlockIndex, nextBlockIndex);

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
            PG_FAILSTR("Unable to add a block to a lane, the maximum number of blocks has been reached (%u)", LANE_MAX_NUM_BLOCKS);
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
        PG_FAILSTR("Invalid block record index (%d), it should be >= 0 and < %u", blockIndex, LANE_MAX_NUM_BLOCKS);
    }
}

//----------------------------------------------------------------------------------------

void Lane::SetBlockBeat(int blockIndex, Beat beat)
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
        block->SetBeat(beat);

        // Insert the block back to the linked list, the proper sorting is guaranteed
        InsertBlock(block);
    }
    else
    {
        PG_FAILSTR("Invalid block record index (%d), it should be >= 0 and < %u", blockIndex, LANE_MAX_NUM_BLOCKS);
    }
}

//----------------------------------------------------------------------------------------

void Lane::SetBlockDuration(int blockIndex, Duration duration)
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

        // Set the new duration of the block
        block->SetDuration(duration);

        // Insert the block back to the linked list, the proper sorting is guaranteed
        InsertBlock(block);
    }
    else
    {
        PG_FAILSTR("Invalid block record index (%d), it should be >= 0 and < %u", blockIndex, LANE_MAX_NUM_BLOCKS);
    }
}

//----------------------------------------------------------------------------------------

void Lane::MoveBlockToLane(int blockIndex, Lane * newLane, Beat beat)
{
    if (   (blockIndex >= 0)
        && (blockIndex < LANE_MAX_NUM_BLOCKS) )
    {
        PG_ASSERTSTR(mNumBlocks >= 1, "Internal error when moving a block to another lane");

        // Check for a space in the other lane
        if (newLane->GetNumBlocks() < LANE_MAX_NUM_BLOCKS)
        {
            // Get a pointer to the block
            Block * block = mBlockRecords[blockIndex].mBlock;
            PG_ASSERTSTR(block != nullptr, "Internal error when moving a block to another lane");

            // Remove the block from the current lane
            RemoveBlock(blockIndex);

            // Set the new position of the block
            block->SetBeat(beat);

            // Insert the block to the new lane, the proper sorting is guaranteed
            newLane->InsertBlock(block);
            block->SetLane(newLane);
        }
        else
        {
            PG_FAILSTR("Unable to move a block to another lane, that other lane is full");
        }
    }
    else
    {
        PG_FAILSTR("Invalid block record index (%d), it should be >= 0 and < %u", blockIndex, LANE_MAX_NUM_BLOCKS);
    }
}

//----------------------------------------------------------------------------------------

bool Lane::FindBlockAndComputeRelativeBeat(float beat, Block * & block, float & relativeBeat)
{
    //! \todo The current approach is extremely brute force and inefficient.
    //! \todo Cache the current selected block in function of the timeline time.

    if (mNumBlocks > 0)
    {
        // Convert the time in floating point format to a tick
        const unsigned int tick = static_cast<unsigned int>(Pegasus::Math::Floor(beat * mTimeline->GetNumTicksPerBeatFloat()));

        // Find the current block index (block the tick is on or right after)
        const int currentBlockIndex = FindCurrentBlock(tick);
        if (currentBlockIndex != INVALID_RECORD_INDEX)
        {
            block = mBlockRecords[currentBlockIndex].mBlock;

            // Compute the number of ticks since the beginning of the block.
            // If the number is strictly lower than the duration in ticks,
            // the block is rendered
            const unsigned int blockTick = tick - block->GetBeat();
            if (blockTick < block->GetDuration())
            {
                // Compute the beat relative to the beginning of the block.
                // This time is measured in beats, not in ticks.
                relativeBeat = static_cast<float>(blockTick) * mTimeline->GetRcpNumTicksPerBeat();

                // Valid block and relative beat
                return true;
            }
        }
    }

    return false;
}


}   // namespace Timeline
}   // namespace Pegasus
