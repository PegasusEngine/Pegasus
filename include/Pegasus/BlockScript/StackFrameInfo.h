/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   StackFrameInfo.h
//! \author Kleber Garcia
//! \date   September 7th 2014
//! \brief  Blockscript stack frame information, to be used in order to obtain stack elements (variables)
//!         positions.

#ifndef STACK_FRAME_INFO_H
#define STACK_FRAME_INFO_H

#include "Pegasus/BlockScript/Container.h"
#include "Pegasus/BlockScript/IddStrPool.h"

namespace Pegasus
{

namespace BlockScript
{
    class TypeDesc;
    
//! Class representing the stack frame information
class StackFrameInfo
{

public:

    struct Entry
    {
    public:
        Entry() : mOffset(-1), mType(nullptr), mIsArg(false) { mName[0] = '\0';}
        ~Entry(){}
        char mName[IddStrPool::sCharsPerString];
        int  mOffset;
        const TypeDesc* mType;
        int  mIsArg;
    };

    //! Creator constructs of a stack frame
    enum  CreatorCategory
    {
        NONE,     //invalid category
        IF_STMT,  //a stack frame on an if stmt
        LOOP, //a stack frame on a for loop
        FUN_BODY,  //a stack frame on a function body
        GLOBAL,  // the global body, accessible for all functions
        STRUCT_DEF //to be used to determine offsets within this stack frame. This frame should have a null parent
    };

    //! Constructor
    StackFrameInfo();

    //! Destructor
    ~StackFrameInfo();

    //! Initializer
    //! \param allocator internal allocator to be used
    void Initialize(Alloc::IAllocator* allocator);

    //! Constructor
    //! \param resets the internal allocation
    void Reset();

    //! \return gets the size in bytes of the current stack frame
    int GetSize() const { return mSize; }

    //! \return gets the size in bytes of the total temporal space in memory
    int GetTempSize() const { return mTempSize; }

    //! \return the total size of this frame plus the temporal space size
    int GetTotalFrameSize() const { return mSize + mTempSize; }

    //! \param type sets the type id to allocate.
    //! \param typeTable type table containing all the type information
    //! \return returns the byte offset for this allocation.
    int Allocate(const char* name, const TypeDesc* type, bool isFunctionArgument = false);

    //! Allocates a temp variable
    //! \param the byte size to allocate
    int AllocateTemporal(int byteSize);

    //! \param name the name for this allocation
    //! \return null if not found, otherwise true.
    Entry* FindDeclaration(const char* name);

    //! Sets the creator category of this stack frame
    //! \param the creator category
    void SetCreatorCategory(CreatorCategory category) { mCreatorCategory = category; }

    //! Gets the creator category
    //! \return category the category
    CreatorCategory GetCreatorCategory() const { return mCreatorCategory; }

    //! sets the parent stack frame id
    //! \param parent parent stack frame
    void SetParentStackFrame(StackFrameInfo* parent) { PG_ASSERT(mParent == nullptr); mParent = parent; }

    //! Unlinks this frame to its parent. Usually only done on struct frames to prevent getting parent frame definitions
    void UnlinkParentStackFrame() { mParent = nullptr; }

    //! \return gets the parent stack frame id
    StackFrameInfo* GetParentStackFrame() const { return mParent; }

private:
    int mSize; 
    int mTempSize;
    CreatorCategory mCreatorCategory;
    Container<Entry> mEntries;
    StackFrameInfo*  mParent;
};

}

}

#endif
