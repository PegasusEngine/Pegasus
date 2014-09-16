/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   TypeDesc.h
//! \author Kleber Garcia
//! \date   2nd September 2014
//! \brief  BlockScript type descriptor class. A type descriptor is a linked list of type
//!         modifiers

#ifndef BLOCKSCRIPT_TYPEDESC_H
#define BLOCKSCRIPT_TYPEDESC_H

namespace Pegasus
{
namespace BlockScript
{

//! the descriptor class
class TypeDesc
{
public:
    static const int sMaxTypeName = 64;

    //! the constructor for the type descriptor.
    TypeDesc();

    //! the destructor for the type descriptor.
    ~TypeDesc();

    //! Sets the name of this typedesc
    //! \param typeName the actual name of the parameter
    void SetName(const char * typeName);

    //! Gets the name of this typedesc
    //! \return the name of this type
    const char * GetName() const { return mName; }

    //! Sets the guid of this typedesc
    //! \param guid the guid of such type
    void SetGuid(int guid) { mGuid = guid; }

    //!  Gets the guid of this typedesc
    //! \return the guid of such type
    int  GetGuid() const { return mGuid; }

    //! public enumeration of type modifiers
    enum Modifier
    {
        M_INVALID,
        M_SCALAR,
        M_VECTOR,
        M_REFERECE,
        M_ARRAY,
        M_TREE
    };

    //! sets the modifier type of this type
    //! \param m the modifier
    void SetModifier(Modifier m) { mModifier = m; }

    //! Gets the modifier of this type
    //! \return the modifier of this type
    Modifier GetModifier() const { return mModifier; }

    //! Sets the child of this type
    //! \param child the child to set
    void  SetChild(int child) { mChild = child; }

    //! Gets the child of this type
    //! \return the typedesc of the child
    const int GetChild() const { return mChild; }

    //! Sets a polymorphic property of modifiers:
    //! M_SCALAR - unused
    //! M_VECTOR - vector size
    //! M_REFERENCE - unused
    //! M_ARRAY - array size
    //! M_TREE  - the underlying byte size
    //! \param prop the property that has been used. 0 when unused
    void SetModifierProperty(int prop) { mModifierProperty = prop; }

    //! Gets the modifier property of this type
    //! \return the polymorphic property of this type
    int  GetModifierProperty() const { return mModifierProperty; }

private:
    //no copy constructor / destructor of object
    TypeDesc(TypeDesc&);
    TypeDesc& operator=(TypeDesc&);

    int       mGuid;
    char      mName[sMaxTypeName];
    Modifier  mModifier;
    int       mChild;
    int       mModifierProperty;
};

}
}

#endif
