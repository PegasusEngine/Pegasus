/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	PropertyDefs.h
//! \author	Karolyn Boulanger
//! \date	25th May 2015
//! \brief	Shared definitions for the property grid

#ifndef PEGASUS_PROPERTYGRID_SHARED_PROPERTYDEFS_H
#define PEGASUS_PROPERTYGRID_SHARED_PROPERTYDEFS_H


namespace Pegasus {
namespace PropertyGrid {


//! Enumerant for the different property categories. Each category represents a scope for the property.
enum PropertyCategory
{
    PROPERTYCATEGORY_CLASS,             //!< For the current class derivation, and all the parent classes
    PROPERTYCATEGORY_OBJECT,            //!< For the instances (created at runtime, per object)

    NUM_PROPERTY_CATEGORIES,

    PROPERTYCATEGORY_INVALID = NUM_PROPERTY_CATEGORIES
};
    
    
//! Enumerant for the different property types. Those are the only ones supported
enum PropertyType
{
    PROPERTYTYPE_BOOL = 0,
    PROPERTYTYPE_INT,
    PROPERTYTYPE_UINT,

    PROPERTYTYPE_FLOAT,
    PROPERTYTYPE_VEC2,
    PROPERTYTYPE_VEC3,
    PROPERTYTYPE_VEC4,

    PROPERTYTYPE_COLOR8RGB,
    PROPERTYTYPE_COLOR8RGBA,

    PROPERTYTYPE_STRING64,
    PROPERTYTYPE_CUSTOM_ENUM,

    NUM_PROPERTY_TYPES,

    PROPERTYTYPE_INVALID = NUM_PROPERTY_TYPES
};

//! Enumerant for the type of editor that a certain property will use.
enum PropertyEditor
{
    EDITOR_DEFAULT,
    EDITOR_SLIDER,
    EDITOR_COUNT
};

//! Variant parameter for an editor, such as min or max.
union VariantParam
{
    unsigned int u;
    int          i;
    float        f;
    float        v[4];
    int          iv[4];

    inline bool operator == (const VariantParam& other) const
    {
        //check if these are binary equivalent
        return    iv[0] == other.iv[0]
               && iv[1] == other.iv[1] 
               && iv[2] == other.iv[2]
               && iv[3] == other.iv[3];
    }
};

//! Descriptor struct for the editor of a property
struct EditorDesc
{
    PropertyEditor editorType;
    union Parameters
    {
        struct SliderBarParams
        {
            VariantParam min;
            VariantParam max;
        } slidebar;

    } params;

    inline bool operator == (const EditorDesc& other) const
    {
        return editorType == other.editorType   
               && params.slidebar.min == other.params.slidebar.min
               && params.slidebar.max == other.params.slidebar.max;
    }
};

class BaseEnumType
{
public:
    //! Enumeration value name.
    BaseEnumType(const char* valueName) :
        mName(valueName), mValue(-1)
    {
    }

    BaseEnumType() : mName("Invalid"), mValue(-1)
    {
    }

            //! Enumeration value name.
    BaseEnumType(const char* valueName, int value) :
        mName(valueName), mValue(value)
    {
        
    }

    ~BaseEnumType()
    {
    }

    //! Gets the name of this enumeration value.
    //! \return value
    inline const char* GetName() const
    {
        return mName;
    }
    
    //! Gets the value of this enumeration
    //! \return the value of this enum.
    inline int GetValue() const
    {
        return mValue;
    }

    //! Gets the value of this enumeration
    //! \return the value of this enum.
    inline operator int()
    {
        return GetValue();
    }

    //! Operator overloading so enums behave like ints.
    //@{             
    inline int operator+(int val) const
    {
        return GetValue() + val;
    }

    inline bool operator==(int value) const
    {
        return GetValue() == value;
    }

    inline bool operator==(const BaseEnumType& value) const
    {
        return GetValue() == value.GetValue();
    }

    inline bool operator != (int value) const
    {
        return !(*this == value);
    }

    inline bool operator != (const BaseEnumType& value) const
    {
        return !(*this == value);
    }

    inline bool operator > (int value) const
    {
        return GetValue() > value;
    }

    inline bool operator >= (int value) const
    {
        return GetValue() >= value;
    }

    inline bool operator < (int value) const
    {
        return GetValue() < value;
    }

    inline bool operator <= (int value) const
    {
        return GetValue() <= value;
    }
    //@}


protected:
    //! Semantical name of this enumeration.
    const char* mName;

    //! Value of this enumeration
    int  mValue;
};

//! Structure holding the description of a property
struct PropertyRecord
{
    PropertyCategory category;          //!< Category of the property, PROPERTYCATEGORY_xxx constant
    PropertyType type;                  //!< Type of the property, PROPERTYTYPE_xxx constant
    int size;                           //!< Size in bytes of the property (> 0)
    const char * name;                  //!< Name of the property, starting with an uppercase letter (non-empty)
                                        //!< (has to be a pointer to a global constant, does not store the string)
	const char * typeName;		        //!< Name of the type of the property
    unsigned char * defaultValuePtr;    //!< Pointer to the default value of the property
    EditorDesc editorDesc;              //!< Description of the editor this property will use.
};


}   // namespace Pegasus
}   // namespace PropertyGrid

#endif  // PEGASUS_PROPERTYGRID_SHARED_PROPERTYDEFS_H
