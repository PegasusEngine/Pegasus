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
    class TypeTable;
    
//! Class representing the stack frame information
class StackFrameInfo
{

public:

    //! Creator constructs of a stack frame
    enum  CreatorCategory
    {
        NONE,     //invalid category
        IF_STMT,  //a stack frame on an if stmt
        FOR_LOOP, //a stack frame on a for loop
        FUN_BODY  //a stack frame on a function body
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

    //! \param type sets the type id to allocate.
    //! \param typeTable type table containing all the type information
    //! \return returns the byte offset for this allocation.
    int Allocate(const char* name, int type, TypeTable& typeTable);

    //! \param name the name for this allocation
    //! \return true if found, false otherwise.
    bool FindDeclaration(const char* name, int& offset, int& type);

    //! Sets the debug information for this stack frame
    //! \param line the line where this stack frame was created
    //! \param category the category of ast node that created this stack frame
    void SetDebugInfo(int line, CreatorCategory category); 

    //! Gets the debug information for this stack frame
    //! \param line the output parameter for the line
    //! \param category the output parameter for the category
    void GetDebugInfo(int& line, CreatorCategory& category);

    //! sets the parent stack frame id
    //! \param parent parent stack frame
    void SetParentStackFrame(int parent) { PG_ASSERT(mParent == -1); mParent = parent; }

    //! \return gets the parent stack frame id
    int GetParentStackFrame() { return mParent; }

private:
    int mSize; 
    CreatorCategory mCreatorCategory;
    struct Entry
    {
    public:
        Entry() : mOffset(-1), mType(-1) { mName[0] = '\0';}
        ~Entry(){}
        char mName[IddStrPool::sCharsPerString];
        int  mOffset;
        int  mType;
    };
    Container<Entry> mEntries;
    int  mCreatedLine;
    int  mParent;
};

}

}

#endif
