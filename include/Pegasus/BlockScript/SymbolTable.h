/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   SymbolTable.h
//! \author Kleber Garcia
//! \date   November 28th, 2014
//! \brief  Master recursive table of functions and types. Use this to have reusable components
//!         across blockscript libraries

#ifndef PEGASUS_BLOCKSCRIPT_SYMBOLTABLE_H
#define PEGASUS_BLOCKSCRIPT_SYMBOLTABLE_H

#include "Pegasus/BlockScript/Container.h"
#include "Pegasus/BlockScript/TypeTable.h"
#include "Pegasus/BlockScript/FunTable.h"
#include "Pegasus/BlockScript/TypeDesc.h"
#include "Pegasus/BlockScript/StackFrameInfo.h"


//! fwd declarations
namespace Pegasus
{
    namespace Alloc
    {
        class IAllocator;
    }

    namespace BlockScript 
    {
        namespace Ast
        {
            class StmtStructDef;
            class StmtFunDec;
            class FunCall;
        }
    }

}

namespace Pegasus
{

namespace BlockScript
{

class SymbolTable
{
public:
    //! Constructor for symbol table
    SymbolTable();

    //! Destructor for symbol table
    ~SymbolTable();

    //! Initialization of symbol table. Run only once to store allocator
    //! \param allocator - the allocator to use internally
    void Initialize(Alloc::IAllocator* allocator);

    //! Registers a child symbol table (external library)
    //! \param symbolTable - the symbol table to have as a child
    void RegisterChild(SymbolTable* symbolTable);

    //! Call to go back to initial empty state and restart compilation (children need to be re-added)
    void Reset();

    //! \return gets the type description from the type name specified (non arrayd)
    const TypeDesc* GetTypeByName(const char* typeName) const;

    //! \returns a writable type description for patching purposes
    //! \note use only this function for hacks
    TypeDesc* GetTypeForPatching(const char* typeName);

    //! \param name the name of the enumeration value
    //! \param outEnumNode a pointer to fill in with the enumeration node 
    //! \param outEnumType a pointer to fill in with the enumeration type
    //! \return true if found it, false otherwise
    bool FindEnumByName(const char* name, const TypeDesc::EnumNode** outEnumNode, const TypeDesc** outEnumType) const;

    TypeDesc::EnumNode* NewEnumNode();

    //! Creates a new type if it does not exist. If the type exists already, it will find it and return it
    //! \param modifier  the modifier to be using
    //! \param name the actual string name of this type
    //! \param child the child id of this type
    //! \param the modifier property. See TypeDesc for more details
    //! \return the description of this type.
    TypeDesc* CreateType(
        TypeDesc::Modifier modifier,
        const char * name,
        const TypeDesc* child = nullptr,
        int modifierProperty = 0,
        TypeDesc::AluEngine engine = TypeDesc::E_NONE,
        Ast::StmtStructDef* structDef = nullptr,
        TypeDesc::EnumNode* enumNode = nullptr
    );

    //! Returns the root (only on the scope of this module) function table
    //! \return the function table.
    //! \warning do not use this to find functions from libraries. The root function table does not
    //!          have references to external libraries
    FunTable* GetRootFunTable() { return &mFunTable; }


    //! Returns a function description based on an AST function call. The FunDesc
    //! \param functionCall - AST node with function call.
    //! \return nullptr if not found, otherwise the description of such function
    FunDesc* FindFunctionDescription(Ast::FunCall* functionCall);

    //! Creates a new function description. Returns null if such function already exists
    //! \param funDec - AST with entire function definition
    //! \return funDesc - nullptr if hte function definition exists already.
    FunDesc* CreateFunctionDescription(Ast::StmtFunDec* funDec);

    //! Creates new stack frame
    //! \return the new frame information
    StackFrameInfo* CreateFrame();

    //! Returns the root global stack frame
    //! \return the root global stack frame
    StackFrameInfo* GetRootGlobalFrame();

private:
    //! function table
    FunTable     mFunTable;

    //! type table
    TypeTable    mTypeTable;
    
    //! the stack frame tree information
    Container<StackFrameInfo> mFrames;

    //! children symbols
    Container<SymbolTable*>   mChildren;

    //! allocator
    Alloc::IAllocator* mAllocator;
};

}

}


#endif
