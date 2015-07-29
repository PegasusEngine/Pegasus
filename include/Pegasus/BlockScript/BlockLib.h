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
#include "Pegasus/BlockScript/FunCallback.h"

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
    //! \param allocator
    //! \param library name, pointer is cached. Make sure is a string imm value
    BlockLib(Alloc::IAllocator* allocator, const char* libraryName);

    //!  Destructor
    virtual ~BlockLib();

    //! \return SymbolTable - export element of a block library
    SymbolTable* GetSymbolTable();

    //! \return SymbolTable - export element of a block library
    const SymbolTable* GetSymbolTable() const;

    //! \return builder - returns the builder of this library (collection of parsing states)
    BlockScriptBuilder* GetBuilder() { return &mBuilder; }

    //! Creates a set of intrinsic functions (c++ callback) into blockscript.
    //! \param the description list
    //! \param the count of the description list
    //! \note this function will internally assert on failure
    void CreateIntrinsicFunctions (const FunctionDeclarationDesc* descriptionList, int count);

    //! Creates a set of enumerations available in blocksript code.
    //! \param a list of enum descriptions
    //! \param the count of the descriptions
    //! \note this function will internally assert on failure
    void CreateEnumTypes(const EnumDeclarationDesc* descriptionList, int count);

    //! Creates a set of block script sctructure definitions.
    //! \param StructDeclarationDesc the descriptor for the declaration
    //! \param the count of the descriptor
    //! \note this function will internally assert on failure
    void CreateStructTypes(const StructDeclarationDesc* descriptionList, int count);

    //! Creates a set of classes types in blockscript (use this function to conviniently create classes)
    //! \param the descriptor list, containing the object type description
    //! \param the count of parameters
    void CreateClassTypes(const ClassTypeDesc* descriptionList, int count);

    //! Returns this libraries name
    const char* GetName() const { return mName; }

private:
    //! Creates a set of intrinsic functions (c++ callback) into blockscript. Sets wether these are methods or not
    //! \param the description list
    //! \param the count of the description list
    //! \param if true, the first argument is used as the this pointer of the method, false then it becomes a simple global function
    //! \note this function will internally assert on failure
    void InternalCreateIntrinsicFunctions (const FunctionDeclarationDesc* descriptionList, int count, bool isMethod);
    Alloc::IAllocator* mAllocator;
    const char* mName;
};

}


}


#endif
