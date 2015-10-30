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

    inline bool operator != (int value) const
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
    PropertyType type;          //!< Type of the property, PROPERTYTYPE_xxx constant
    int size;                   //!< Size in bytes of the property (> 0)
    const char * name;          //!< Name of the property, starting with an uppercase letter (non-empty)
                                //!< (has to be a pointer to a global constant, does not store the string)
	const char * typeName;		//!< Name of the type of the property
    void * defaultValuePtr;     //!< Pointer to the default value of the property
};


}   // namespace Pegasus
}   // namespace PropertyGrid

#endif  // PEGASUS_PROPERTYGRID_SHARED_PROPERTYDEFS_H
