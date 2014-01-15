/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Lane.h
//! \author	Kevin Boulanger
//! \date	07th November 2013
//! \brief	Timeline lane management, manages a set of blocks on one lane of the timeline

#ifndef PEGASUS_TIMELINE_LANE_H
#define PEGASUS_TIMELINE_LANE_H

#include "Pegasus/Timeline/Shared/LaneDefs.h"

namespace Pegasus {
    namespace Timeline {
        class LaneProxy;
        class Block;
        class IBlockProxy;
    }

    namespace Wnd {
        class Window;
    }
}
    
namespace Pegasus {
namespace Timeline {


//! Timeline lane management, manages a set of blocks on one lane of the timeline
class Lane
{
public:

    //! Constructor
    //! \param allocator Allocator used for all timeline allocations
    Lane(Alloc::IAllocator * allocator);

    //! Destructor
    virtual ~Lane();


    //! Add a block to the lane
    //! \param block Allocated block, with position and size ignored
    //! \param position Position of the block, measured in beats (>= 0.0f)
    //! \param length Length of the block, measured in beats (> 0.0f)
    //! \note The internal linked list stays sorted after this operation
    //! \return True if succeeded, false if the block is invalid, has a collision with an existing block,
    //!         or the number of blocks has already reached LANE_MAX_NUM_BLOCKS
    bool InsertBlock(Block * block, float position, float length);

    //! Remove a block from the lane
    //! \param block Existing block belonging to the lane
    void RemoveBlock(Block * block);

    //! Set the position of a block in the lane
    //! \warning The block has to belong to the lane
    //! \note If the block is not found in the lane, the block is not moved,
    //!       to not break the sorted linked list of another lane
    //! \param block Block to move
    //! \param position New position of the block, measured in beats (>= 0.0f)
    void SetBlockPosition(Block * block, float position);

    //! Set the length of a block in the lane
    //! \warning The block has to belong to the lane
    //! \note If the block is not found in the lane, the block is not resized,
    //!       to not break the sorted linked list of another lane
    //! \param length New length of the block, measured in beats (> 0.0f)
    void SetBlockLength(Block * block, float length);


    // Tell all the blocks of the lane to initialize their content (calling their Initialize() function)
    void InitializeBlocks();

    //! Render the content of the lane for the given window
    //! \param beat Current beat, can have fractional part
    //! \param window Window in which the lane is being rendered
    //! \todo That dependency is ugly. Find a way to remove that dependency
    void Render(float beat, Wnd::Window * window);


#if PEGASUS_ENABLE_PROXIES

    //! Get the list of blocks of the lane
    //! \param blocks Allocated array of LANE_MAX_NUM_BLOCKS pointers to IBlockProxy,
    //!               contains the resulting list of block proxy pointers.
    //! \note Only the valid blocks have their pointers updated
    //! \return Number of block proxy pointers written to the \a blockList array (<= LANE_MAX_NUM_BLOCKS)
    unsigned int GetBlocks(IBlockProxy ** blocks) const;


    //! Get the proxy associated with the lane
    inline LaneProxy * GetProxy() const { return mProxy; }

    //! Maximum length of the name for the lane
    enum { MAX_NAME_LENGTH = 31 };

    //! Set the name of the lane
    //! \param name New name of the lane, can be empty or nullptr, but no longer than MAX_NAME_LENGTH
    void SetName(const char * name);

    //! Get the name of the lane
    //! \return Name of the lane, can be empty or nullptr
    inline const char * GetName() const { return mName; }

    //! Test if the name of the lane is defined
    //! \return True if the name is not an empty string
    inline bool IsNameDefined() const { return (mName[0] != '\0'); }

#endif  // PEGASUS_ENABLE_PROXIES

#if PEGASUS_DEBUG

    //! Dump the content of the lane into the log for debugging
    void DumpToLog();

#endif  // PEGASUS_DEBUG

    //------------------------------------------------------------------------------------

private:

    //! Given a beat on the timeline, return the index of the current (or previous) block and the next one (not reached yet)
    //! \param beat Input beat, can have fractional part
    //! \param currentBlockIndex Resulting block index intersected by the given beat,
    //!                          or just before the beat if the next block is not reached yet.
    //!                          INVALID_RECORD_INDEX if the beat is before the first beat, or no block is defined on the timeline
    //! \param nextBlockIndex Resulting block index after the given beat, not reached yet.
    //!                       INVALID_RECORD_INDEX if the beat is on or after the beginning of the last block,
    //!                       or no block is defined on the timeline
    void FindCurrentAndNextBlocks(float beat, int & currentBlockIndex, int & nextBlockIndex) const;

    //! Find the index of a block record in the array that is not used yet
    //! \return Index of a block record free to use (0 <= index < LANE_MAX_NUM_BLOCKS, when mNumBlocks < LANE_MAX_NUM_BLOCKS),
    //!         INVALID_RECORD_INDEX if the array is full (mNumBlocks == LANE_MAX_NUM_BLOCKS), or in case of error
    int FindFirstAvailableBlockRecord() const;

    //! Find the index of a block record in the linked list
    //! \param block Block to find
    //! \return Index of the block record for the input block, INVALID_RECORD_INDEX if not found or in case of error
    int FindBlockIndex(Block * block) const;

    //! Add a block to the lane with predefined position and length
    //! \param block Allocated block, with position and size already defined
    //! \note The internal linked list stays sorted after this operation
    //! \return True if succeeded, false if the block is invalid, has a collision with an existing block,
    //!         or the number of blocks has already reached LANE_MAX_NUM_BLOCKS
    bool InsertBlock(Block * block);

    //! Remove a block from the lane given a block record index in the linked list
    //! \param blockIndex Index of the block in the \a mBlockRecords array, < LANE_MAX_NUM_BLOCKS
    void RemoveBlock(int blockIndex);

    //! Set the position of a block in the lane given a block record index in the linked list
    //! \param blockIndex Index of the block in the \a mBlockRecords array, < LANE_MAX_NUM_BLOCKS
    //! \param position New position of the block, measured in beats (>= 0.0f)
    void SetBlockPosition(int blockIndex, float position);

    //! Set the length of a block in the lane given a block record index in the linked list
    //! \param blockIndex Index of the block in the \a mBlockRecords array, < LANE_MAX_NUM_BLOCKS
    //! \param length New length of the block, measured in beats (> 0.0f)
    void SetBlockLength(int blockIndex, float length);

    //------------------------------------------------------------------------------------

private:

    // Lanes cannot be copied
    PG_DISABLE_COPY(Lane)

    //! Allocator used for all timeline allocations
    Alloc::IAllocator * mAllocator;


    //! Record to store one block inside a linked list
    typedef struct BlockRecord
    {
        Block * mBlock;     //!< Pointer to the stored block, nullptr if the record is invalid
        int mNext;          //!< Index of the next record, INVALID_RECORD_INDEX if the record is invalid,
                            //!< mFirstBlockIndex if the last step is reached
    };

    //! Invalid index for the block records
    enum { INVALID_RECORD_INDEX = -1 };

    //! Set of block records, stored as linked list in a fixed size array.
    //! The blocks are ordered by position from beginning to end.
    //! Only mNumBlocks block records are valid, but are in a random order
    BlockRecord mBlockRecords[LANE_MAX_NUM_BLOCKS];

    //! Index of the first block record in the table (0 by default, but can change if a block is removed)
    unsigned int mFirstBlockIndex;

    //! Number of used block records in mBlockRecords (<= LANE_MAX_NUM_BLOCKS)
    unsigned int mNumBlocks;


#if PEGASUS_ENABLE_PROXIES

    //! Proxy associated with the lane
    LaneProxy * mProxy;

    //! Name of the lane, can be empty or nullptr, but no longer than MAX_NAME_LENGTH
    char mName[MAX_NAME_LENGTH + 1];

#endif  // PEGASUS_ENABLE_PROXIES
};


}   // namespace Timeline
}   // namespace Pegasus

#endif  // PEGASUS_TIMELINE_LANE_H
