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
    bool FindEnumByName(const char* name, const EnumNode** outEnumNode, const TypeDesc** outEnumType) const;

    //! creates a new node describing an enumeration element
    EnumNode* NewEnumNode();

    //! creates a new node describing a property of an object
    PropertyNode* NewPropertyNode();

    //! Returns the root (only on the scope of this module) function table
    //! \return the function table.
    //! \warning do not use this to find functions from libraries. The root function table does not
    //!          have references to external libraries
    FunTable* GetRootFunTable() { return &mFunTable; }

    //! Returns and creates a scalar type. Scalar types are 4 bytes.
    //! \param name the block script name that will be used
    //! \param aluEngine the ALU engine to use for this scalar type
    //! \return type descriptor structure
    TypeDesc* CreateScalarType(const char* name, TypeDesc::AluEngine aluEngine);

    //! Returns and creates a vectorial type.
    //! \param name the name of the vector type
    //! \param the child type of the vector
    //! \param the size of the vector (how many scalars or vector children it has
    //! \param the aluEngine of the vector type
    TypeDesc* CreateVectorType(const char* name, TypeDesc* childType, int vectorSize, TypeDesc::AluEngine aluEngine);

    //! Returns and creates an object type. Objects are passed around by handles of 4 bytes.
    //! \param the blockscript name of the type
    //! \return type descriptor structure
    TypeDesc* CreateObjectType(const char* name, PropertyNode* propertyList, GetObjectPropertyRuntimePtrCallback getPropertyCallback);

    //! Returns and creates an enumeration type
    //! \param the name of the enumeration type for blockscript
    //! \param the linked list of enumeration definitions with their values
    TypeDesc* CreateEnumType(const char* name, EnumNode* enumNode);

    //! Returns and creates a structure definition from c++ side
    //! \param name the actual name
    //! \param def the actual definition structure statement
    TypeDesc* CreateStructType(const char* name, Ast::StmtStructDef* def);

    //! Creates the * type, which represents "any" value passed by. Used only in c++ callbacks
    //! \return the star type created
    TypeDesc* CreateStarType();

    //! Creates a static array type.
    //! \param name the name of this array. Irrelevant though, since the arrays are found using the array parsing identifiers notation
    //! \param childType the child type of the array (meaning the basic type)
    //! \param count the number of elements this static array has
    TypeDesc* CreateArrayType(const char* name, TypeDesc* childType, int count);

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

    //! \return the function table
    const FunTable* GetFunTable() const { return &mFunTable; }

    //! \return the type table
    const TypeTable* GetTypeTable() const { return &mTypeTable; }

private:
    //! Creates a new type if it does not exist. If the type exists already, it will find it and return it
    //! \param modifier  the modifier to be using
    //! \param name the actual string name of this type
    //! \param child the child id of this type
    //! \param the modifier property. See TypeDesc for more details
    //! \return the description of this type.
    TypeDesc* InternalCreateType(
        TypeDesc::Modifier modifier,
        const char * name,
        TypeDesc* child = nullptr,
        TypeDesc::ModifierProperty modifierProperty = TypeDesc::ModifierProperty(),
        TypeDesc::AluEngine engine = TypeDesc::E_NONE,
        Ast::StmtStructDef* structDef = nullptr,
        EnumNode* enumNode = nullptr,
        PropertyNode* propertyNode = nullptr,
        GetObjectPropertyRuntimePtrCallback getPropertyCallback = nullptr
    );

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
