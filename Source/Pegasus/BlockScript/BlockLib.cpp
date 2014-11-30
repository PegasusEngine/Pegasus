/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   BlockLib.cpp
//! \author Kleber Garcia
//! \date   November 28th, 2014
//! \brief  Represents a module with libraries, types and functions

#include "Pegasus/BlockScript/BlockLib.h"
#include "Pegasus/Allocator/IAllocator.h"

using namespace Pegasus;
using namespace Pegasus::BlockScript;

 //! Constructor in case you did not notice
BlockLib::BlockLib(Alloc::IAllocator* allocator)
    : BlockScriptCompiler(allocator), mAllocator(allocator)
{
}

//!  Destructor
BlockLib::~BlockLib()
{
}


SymbolTable* BlockLib::GetSymbolTable()
{
    return mBuilder.GetSymbolTable();
}
