/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	BlockDefs.h
//! \author	Kevin Boulanger
//! \date	12th January 2014
//! \brief	Shared definitions for the timeline blocks

#ifndef PEGASUS_TEXTURE_SHARED_BLOCKDEFS_H
#define PEGASUS_TEXTURE_SHARED_BLOCKDEFS_H

#include "Pegasus/Preprocessor.h"
#include "Pegasus/Allocator/IAllocator.h"


//! Macro to declare a new block, to use at the top of the class declaration, right after the first brace
//! \param editorString String displayed by the editor (usually class name without the "Block" suffix)
//!                     (maximum length: MAX_BLOCK_CLASS_NAME_LENGTH characters)
#if PEGASUS_ENABLE_PROXIES

#define DECLARE_TIMELINE_BLOCK(className, editorString)                                             \
    public:                                                                                         \
        static Pegasus::Timeline::Block * CreateBlock(Pegasus::Alloc::IAllocator * allocator,       \
                                                 Pegasus::Core::IApplicationContext* appContext)    \
        {   return PG_NEW(allocator, -1, #className, Pegasus::Alloc::PG_MEM_PERM)                   \
                            className(allocator, appContext);    }                                  \
        virtual const char * GetEditorString() const { return editorString; }                       \
        virtual const char * GetClassName()    const { return #className; }                          \
        static const char * GetStaticEditorString() { return editorString; }                        \

#else

#define DECLARE_TIMELINE_BLOCK(className, editorString)                                             \
    public:                                                                                         \
        virtual const char * GetClassName()    const { return #className; }                          \
        static Pegasus::Timeline::Block * CreateBlock(Pegasus::Alloc::IAllocator * allocator,       \
                                                 Pegasus::Core::IApplicationContext* appContext)    \
        {   return PG_NEW(allocator, -1, #className, Pegasus::Alloc::PG_MEM_PERM)                   \
                            className(allocator, appContext);    }                                  \

#endif  // PEGASUS_ENABLE_PROXIES


#endif  // PEGASUS_TEXTURE_SHARED_BLOCKDEFS_H
