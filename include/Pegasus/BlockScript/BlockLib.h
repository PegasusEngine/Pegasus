/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   BlockLib.h
//! \author Kleber Garcia
//! \date   November 28th, 2014
//! \brief  Represents a module with libraries, types and functions
//!         WARNING: for now, libraries generated from c++ can only exist

#ifndef PEGASUS_BLOCKLIB_H
#define PEGASUS_BLOCKLIB_H

#include "Pegasus/BlockScript/BlockScriptCompiler.h"

// forward declarations
namespace Pegasus
{
}
namespace Pegasus
{

namespace BlockScript
{

class SymbolTable;

//! BlockLib Declaration
class BlockLib : public BlockScriptCompiler
{
public:
    //! Constructor in case you did not notice
    BlockLib(Alloc::IAllocator* allocator);

    //!  Destructor
    virtual ~BlockLib();

    //! \return SymbolTable - export element of a block library
    SymbolTable* GetSymbolTable();

    //! \return builder - returns the builder of this library (collection of parsing states)
    BlockScriptBuilder* GetBuilder() { return &mBuilder; }

private:
    Alloc::IAllocator* mAllocator;
};

}


}


#endif
