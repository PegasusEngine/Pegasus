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
    for (unsigned int b = 0; b < MAX_NUM_BLOCKS; ++b)
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
    for (unsigned int b = 0; b < MAX_NUM_BLOCKS; ++b)
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

bool Lane::InsertBlock(Block * block)
{
    PG_LOG('TMLN', "Adding a block to a lane");

    if (block != nullptr)
    {
        if (mNumBlocks < MAX_NUM_BLOCKS)
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
                    if (nextBlockIndex!= INVALID_RECORD_INDEX)
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
                    if (nextBlockIndex!= INVALID_RECORD_INDEX)
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
            PG_FAILSTR("Unable to add a block to a lane, the maximum number of blocks has been reached (%d)", MAX_NUM_BLOCKS);
        }
    }
    else
    {
        PG_FAILSTR("Unable to add a block to the lane, the block is invalid");
    }

    return false;
}

//----------------------------------------------------------------------------------------

#if PEGASUS_ENABLE_PROXIES
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
    PG_LOG('TMLN', "Content of the timeline lane (format: (index,position,length)):");
    if (mNumBlocks == 0)
    {
        PG_LOG('TMLN', "    EMPTY")
    }
    else
    {
        int currentIndex = mFirstBlockIndex;
        do
        {
            PG_LOG('TMLN', "(%d, %f, %f)",
                   currentIndex,
                   mBlockRecords[currentIndex].mBlock->GetPosition(),
                   mBlockRecords[currentIndex].mBlock->GetLength());
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
    if (mNumBlocks >= MAX_NUM_BLOCKS)
    {
        return INVALID_RECORD_INDEX;
    }

    // Look for an available block record
    for (unsigned int b = 0; b < MAX_NUM_BLOCKS; ++b)
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


}   // namespace Timeline
}   // namespace Pegasus
