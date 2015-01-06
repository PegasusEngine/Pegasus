/****************************************************************************************/
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   TypeTable.h
//! \author Kleber Garcia
//! \date   2nd September 2014
//! \brief  Global Container of registered types.

#ifndef PEGASUS_TYPETABLE_H
#define PEGASUS_TYPETABLE_H
#include "Pegasus/BlockScript/TypeDesc.h"
#include "Pegasus/BlockScript/Container.h"

namespace Pegasus
{


//forward declarations
namespace Alloc
{
    class IAllocator;
}

namespace Ast
{
    class StmtStructDef;
}

namespace BlockScript
{

//type table, master container of all unique types
class TypeTable
{
public:
    // constructor
    TypeTable();

    //! destructor
    ~TypeTable();

    //! initializes the type table internally with default scalar / tree types
    //! \param alloc the allocator to be used internally
    void Initialize(Alloc::IAllocator* alloc);

    //! shuts down the type and frees memory
    void Shutdown();

    //! Creates a new type if it does not exist. If the type exists already, it will find it and return it
    //! \param modifier  the modifier to be using
    //! \param name the actual string name of this type
    //! \param child the child id of this type
    //! \param the modifier property. See TypeDesc for more details
    //! \return the guid of this type.
    TypeDesc* CreateType(
        TypeDesc::Modifier modifier,
        const char * name,
        const TypeDesc* child = nullptr,
        int modifierProperty = 0,
        TypeDesc::AluEngine engine = TypeDesc::E_NONE,
        Ast::StmtStructDef* structDef = nullptr,
        TypeDesc::EnumNode* enumNode = nullptr
    );

    //! Gets a type description structure
    //! \param name unique name
    //! \return the description struct 
	const TypeDesc* GetTypeByName(const char* name) const;

    //! Gets a type description structure for writable purposes
    //! \param name unique name
    //! \return the description struct for modification
    TypeDesc* GetTypeForPatching(const char* name);

    //! \param name the name of the enumeration value
    //! \param outEnumNode a pointer to fill in with the enumeration node 
    //! \param outEnumType a pointer to fill in with the enumeration type
    //! \return true if found it, false otherwise
    bool FindEnumByName(const char* name, const TypeDesc::EnumNode** outEnumNode, const TypeDesc** outEnumType) const;

    //! returns a new enum node
    TypeDesc::EnumNode* NewEnumNode();

private:
    //! Computes the size of a type
    bool ComputeSize(const TypeDesc* type, int& outSize) const;
    Container<TypeDesc> mTypeDescPool;
    Container<TypeDesc::EnumNode> mEnumNodePool;
};

}
}

#endif
