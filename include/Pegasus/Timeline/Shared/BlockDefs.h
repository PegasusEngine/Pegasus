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


//! Macro to declare a new block, to use at the top of the class declaration, right after the first brace
//! \param editorString String displayed by the editor (usually class name without the "Block" suffix)
#if PEGASUS_ENABLE_PROXIES

#define DECLARE_TIMELINE_BLOCK(editorString)                                        \
    public:                                                                         \
        virtual const char * GetEditorString() const { return editorString; }

#else

#define DECLARE_TIMELINE_BLOCK(editorString)

#endif  // PEGASUS_ENABLE_PROXIES


#endif  // PEGASUS_TEXTURE_SHARED_BLOCKDEFS_H
