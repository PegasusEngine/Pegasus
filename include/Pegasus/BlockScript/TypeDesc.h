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

//forward declarations
namespace Pegasus
{
    namespace BlockScript {
        class BsVmState;
        class SymbolTable;
        class FunCallbackContext;
        class TypeDesc;
        struct PropertyNode;
        struct EnumNode;
    }
}


namespace Pegasus
{
namespace BlockScript
{

namespace Ast
{
    class StmtStructDef;
}

//! callback definition to get node properties
//! \param BsVmState the virtual machine state
//! \param int the handle of the object
//! \param propertyDesc the description of the property to acquire
typedef void*     (*GetObjectPropertyRuntimePtrCallback)(BsVmState* state, int, const PropertyNode* propertyDesc);

//! an enumeration description for enumeration types
struct EnumNode
{
    const char* mIdd;
    int         mGuid;
    EnumNode*   mNext;
public:
    EnumNode() : mIdd(nullptr), mGuid(0), mNext(nullptr) {}
};

//! a property description for object types
struct PropertyNode
{
    const char* mName;
    int mGuid;
    const TypeDesc* mType;
    PropertyNode*  mNext;
public:
    PropertyNode() : mName(nullptr), mGuid(-1), mType(nullptr), mNext(nullptr) {} 
};


//! the type descriptor class.
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

    //! public enumeration of type modifiers
    enum Modifier
    {
        M_INVALID,
        M_SCALAR, //int, float
        M_VECTOR, //float2, float3 and float4
        M_ARRAY,  //array or structure
        M_STRUCT, //user defined struct
        M_ENUM,   //user defined enumeration 
        M_REFERECE, // custom c++ object reference
        M_STAR      // only used in c++ callbacks, not in blockscript, grabs the pointer of whichever input is passed.
                    // the actual type is actually recorded in the funcall list passed in the FunCallback
    };

    //! only Types that use arithmetic and logical operations. 
    //! if a type has one of these, then an ExpressionEngine must be defined
    enum AluEngine
    {
        E_NONE,
        E_INT,
        E_FLOAT,
        E_FLOAT2,
        E_FLOAT3,
        E_FLOAT4
    };

    //! sets the modifier type of this type
    //! \param m the modifier
    void SetModifier(Modifier m) { mModifier = m; }

    //! Gets the modifier of this type
    //! \return the modifier of this type
    Modifier GetModifier() const { return mModifier; }

    //! Sets the child of this type
    //! \param child the child to set
    void  SetChild(const TypeDesc* child) { mChild = child; }

    //! Comparison function
    //! \param other the other pointer
    //! \return true if both are semantically equal, false otherwise
    bool Equals(const TypeDesc* other) const;

    //! Gets the child of this type
    //! \return the typedesc of the child
    const TypeDesc* GetChild() const { return mChild; }

    //! \return Returns a structure definition if this type is a structure.
    const Ast::StmtStructDef* GetStructDef() const { return mStructDef; }
    
    //! sets this type's struct def. This functio
    //! \param structDef the definition of this struct
    void SetStructDef(Ast::StmtStructDef* structDef) { mStructDef = structDef; }

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

    //! Sets the byte size of a type
    void SetByteSize(int byteSize) { mByteSize = byteSize; }

    //! Gets the byte size of a type
    int GetByteSize() const { return mByteSize; }

    //! sets the logical and arithmetic engine to be used in the runtime
    void SetAluEngine(AluEngine engine) { mAluEngine = engine; }

    //! gets the logical and arithmetic engine to be used in the runtime
    AluEngine GetAluEngine() const { return mAluEngine; }

    //! sets the enumeration node for this type. This function assumes the modifier is an enum.
    void  SetEnumNode(EnumNode* enumNode) { mEnumNode = enumNode; }

    //! sets the property node list for this type. This function assumes the modifier is a Reference
    void SetPropertyNode(PropertyNode* propNode) { mPropertyNode = propNode; }

    //! gets the enumeration node for this type
    const EnumNode* GetEnumNode() const { return mEnumNode; }

    //! gets a property node for this object type.
    const PropertyNode* GetPropertyNode() const { return mPropertyNode; }
    
    //! sets the callback used by the runtime to access a property from an object reference
    void SetPropertyCallback(GetObjectPropertyRuntimePtrCallback callback) { mPropertyCallback = callback; }

    //! returns the callback used by the runtime to access a property from an object reference
    GetObjectPropertyRuntimePtrCallback GetPropertyCallback() const { return mPropertyCallback; }

private:
    //no copy constructor / destructor of object
    TypeDesc(TypeDesc&);
    TypeDesc& operator=(TypeDesc&);

    bool CmpStructProperty(const TypeDesc* other) const;
    bool CmpEnumProperty(const TypeDesc* other) const;

    char       mName[sMaxTypeName];
    Modifier   mModifier;
    AluEngine  mAluEngine;
    const TypeDesc*  mChild;
    Ast::StmtStructDef* mStructDef;
    EnumNode*           mEnumNode;
    PropertyNode*       mPropertyNode;
    GetObjectPropertyRuntimePtrCallback mPropertyCallback;
    int        mModifierProperty;
    int        mByteSize;
};

}
}

#endif
