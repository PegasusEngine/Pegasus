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
        TypeDesc* child = nullptr,
        TypeDesc::ModifierProperty modifierProperty = TypeDesc::ModifierProperty(),
        TypeDesc::AluEngine engine = TypeDesc::E_NONE,
        Ast::StmtStructDef* structDef = nullptr,
        EnumNode* enumNode = nullptr,
        PropertyNode* propertyNode = nullptr,
        ObjectPropertyAccessorCallback getPropCallback = nullptr
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
    bool FindEnumByName(const char* name, const EnumNode** outEnumNode, const TypeDesc** outEnumType) const;

    //! \returns a new enum node
    EnumNode* NewEnumNode();

    //! \returns a new property node
    PropertyNode* NewPropertyNode();

    //! \returns the number of types available
    int GetTypeCount() const { return mTypeDescPool.Size(); }

    //! \param the index. index goes from 0 to GetTypeCount()
    //! \returns the type description structure
    const TypeDesc* GetTypeByIndex(int index) const { return &mTypeDescPool[index]; }

private:
    Container<TypeDesc> mTypeDescPool;
    Container<EnumNode> mEnumNodePool;
    Container<PropertyNode> mPropertyNodePool;
};

}
}

#endif
