/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	PropertyGrid.h
//! \author	Kevin Boulanger
//! \date	24th April 2014
//! \brief	Property grid, defining a set of editable properties

#ifndef PEGASUS_PROPERTYGRID_PROPERTYGRID_H
#define PEGASUS_PROPERTYGRID_PROPERTYGRID_H

#include "Pegasus/Math/Vector.h"
#include "Pegasus/Math/Color.h"

namespace Pegasus {
namespace PropertyGrid {


//! Enumerant for the different property types. Those are the only ones supported
enum PropertyType
{
    PROPERTYTYPE_INT = 0,

    PROPERTYTYPE_FLOAT,
    PROPERTYTYPE_VEC2,
    PROPERTYTYPE_VEC3,
    PROPERTYTYPE_VEC4,

    PROPERTYTYPE_COLOR8RGB,
    PROPERTYTYPE_COLOR8RGBA,

    NUM_PROPERTY_TYPES,

    PROPERTYTYPE_INVALID = NUM_PROPERTY_TYPES
};


//! Template class to contain constants and types for each property type
//! \param T Property type
template <typename T>
class PropertyDefinition
{
    // Empty on purpose, to throw a compilation error if the type is undefined
};


//@{
//! Template specialization for the property definitions, one per type.
//! PROPERTY_TYPE is the enumerant corresponding to the type (to avoid switch case).
//! VarType is the type required to store the property.
//! ParamType is the type required to pass the property as a parameter of a function
//! (typically a const reference for non-primitive types).
//! ReturnType is the type required to return a property from a function
//! (typically a value, even for non-primitive types).
template <>
struct PropertyDefinition<int>
{
    static const PropertyType PROPERTY_TYPE = PROPERTYTYPE_INT;
    typedef int VarType;
    typedef int ParamType;
    typedef int ReturnType;
};

template <>
struct PropertyDefinition<float>
{
    static const PropertyType PROPERTY_TYPE = PROPERTYTYPE_FLOAT;
    typedef float VarType;
    typedef float ParamType;
    typedef float ReturnType;
};

template <>
struct PropertyDefinition<Math::Vec2>
{
    static const PropertyType PROPERTY_TYPE = PROPERTYTYPE_VEC2;
    typedef Math::Vec2       VarType;
    typedef Math::Vec2In     ParamType;
    typedef Math::Vec2Return ReturnType;
};

template <>
struct PropertyDefinition<Math::Vec3>
{
    static const PropertyType PROPERTY_TYPE = PROPERTYTYPE_VEC3;
    typedef Math::Vec3       VarType;
    typedef Math::Vec3In     ParamType;
    typedef Math::Vec3Return ReturnType;
};

template <>
struct PropertyDefinition<Math::Vec4>
{
    static const PropertyType PROPERTY_TYPE = PROPERTYTYPE_VEC4;
    typedef Math::Vec4       VarType;
    typedef Math::Vec4In     ParamType;
    typedef Math::Vec4Return ReturnType;
};

template <>
struct PropertyDefinition<Math::Color8RGB>
{
    static const PropertyType PROPERTY_TYPE = PROPERTYTYPE_COLOR8RGB;
    typedef Math::Color8RGB       VarType;
    typedef Math::Color8RGBIn     ParamType;
    typedef Math::Color8RGBReturn ReturnType;
};

template <>
struct PropertyDefinition<Math::Color8RGBA>
{
    static const PropertyType PROPERTY_TYPE = PROPERTYTYPE_COLOR8RGBA;
    typedef Math::Color8RGBA       VarType;
    typedef Math::Color8RGBAIn     ParamType;
    typedef Math::Color8RGBAReturn ReturnType;
};
//@}

//----------------------------------------------------------------------------------------

// Macros to declare properties in a class.

// In the header file, inside the class definition, declare properties as following:
//     BEGIN_DECLARE_PROPERTIES()
//         DECLARE_PROPERTY(int, NumLoops)
//         DECLARE_PROPERTY(Math::Vec3, Position)
//             etc.
//     END_DECLARE_PROPERTIES()

// In the implementation file, after the namespace declaration:
//     BEGIN_IMPLEMENT_PROPERTIES()
//         IMPLEMENT_PROPERTY(int, NumLoops, 1)
//         IMPLEMENT_PROPERTY(Math::Vec3, Position, Math::Vec3(0.0f, 1.0f, 2.0f))
//             etc.
//     END_IMPLEMENT_PROPERTIES()


//! Macro to start declaring a set of properties
//! \note To be used inside a class declaration, before calling \a DECLARE_PROPERTY()
#define BEGIN_DECLARE_PROPERTIES()                                                  \
    public:                                                                         \
        virtual void InitProperties();                                              \
    private:                                                                        \
        static const char * sClassName;                                             \


//! Macro to declare a property
//! \param type Type of the property, use Math:: in front of types from the math library
//! \param name Name of the property, starting with an uppercase letter
//! \warning To be used after \a BEGIN_DECLARE_PROPERTIES() and before END_DECLARE_PROPERTIES()
#define DECLARE_PROPERTY(type, name)                                                \
    public:                                                                         \
        inline PropertyGrid::PropertyDefinition<type>::ReturnType Get##name() const { return mProperty##name; }         \
        inline void Set##name(PropertyGrid::PropertyDefinition<type>::ParamType value) { mProperty##name = value; }     \
    private:                                                                        \
        PropertyGrid::PropertyDefinition<type>::VarType mProperty##name;            \


//! Macro to stop declaring a set of properties
//! \note To be used inside a class declaration, after calling \a DECLARE_PROPERTY()
#define END_DECLARE_PROPERTIES()


//! Macro to start declaring a set of properties in the implementation file
//! \param className Class name of the property grid owner
//! \note To be used inside a .cpp file, before calling \a IMPLEMENT_PROPERTY()
#define BEGIN_IMPLEMENT_PROPERTIES(className)                                       \
    const char * className::sClassName = #className;                                \
    void className::InitProperties()                                                \
    {                                                                               \


//! Macro to declare a property in the implementation file
//! \param type Type of the property, use Math:: in front of types from the math library
//! \param name Name of the property, starting with an uppercase letter
//! \param defaultValue Default value of the property, use Math::Type(values) with math types
//! \warning To be used after \a BEGIN_IMPLEMENT_PROPERTIES() and before END_IMPLEMENT_PROPERTIES()
#define IMPLEMENT_PROPERTY(type, name, defaultValue)                                \
    mProperty##name = defaultValue;                                                 \
    GetPropertyGrid().RegisterProperty<type>(#name, &mProperty##name, sClassName);  \


//! Macro to stop declaring a set of properties in the implementation file
//! \note To be used inside a .cpp file, after calling \a IMPLEMENT_PROPERTY()
#define END_IMPLEMENT_PROPERTIES()                                                  \
    }                                                                               \

//----------------------------------------------------------------------------------------

//! Property grid, defining a set of editable properties.
//! When properties are declared, their association accessors are created (SetName() and GetName()).
//! The property grid object stores the property definitions and gives access to string-based accessors.
//! \warning For a class to support properties, it needs to store an instance of this class,
//!          accessible through a GetPropertyGrid() accessor, returning a reference.
//!          It also needs to define an empty virtual void InitProperties() function.
//! \note Typically used inside \a Graph::Node, but can be used within any other class
class PropertyGrid
{
public:

    //! Constructor
    PropertyGrid();

    //! Register a property
    //! \param T Type of the property, use Math:: in front of types from the math library
    //! \param name Name of the property, starting with an uppercase letter
    //! \param varPtr Pointer to the property variable
    //! \param className Class name of the property grid owner
    //! \note Called by \a IMPLEMENT_PROPERTY()
    template <typename T>
    void RegisterProperty(const char * name, T * varPtr, const char * className);

    //------------------------------------------------------------------------------------

    // Accessors for the proxy interface

#if PEGASUS_ENABLE_PROXIES

    //! Get the number of registered properties using \a RegisterProperty()
    //! \return Number of successfully registered properties
    unsigned int GetNumProperties() const;

    //! Get the type of a property
    //! \param index Index of the property (0 <= index < GetNumProperties())
    //! \return Type of the property, PROPERTYTYPE_xxx constant, PROPERTYTYPE_INVALID in case of error
    PropertyType GetPropertyType(unsigned int index) const;

    //! Get the name of a property
    //! \param index Index of the property (0 <= index < GetNumProperties())
    //! \return Name of the property, empty string in case of error
    const char * GetPropertyName(unsigned int index) const;

    //! Get the name of the class owning the property
    //! \param index Index of the property (0 <= index < GetNumProperties())
    //! \return Name of the class owning the property, empty string in case of error
    const char * GetPropertyClassName(unsigned int index) const;

    //! Set the value of a property by name
    //! \param name Name of the property, non-empty string
    //! \param valuePtr Non-nullptr pointer to the value
    //! \note The value is copied, not its pointer
    //! \note No property is set if the parameters are invalid
    void SetProperty(const char * name, void * valuePtr);

    //! Get the value of a property by name
    //! \param name Name of the property, non-empty string
    //! \return Pointer to the value, nullptr in case of error
    const void * GetProperty(const char * name) const;

#endif  // PEGASUS_ENABLE_PROXIES

    //------------------------------------------------------------------------------------
    
private:

    //! Record for one property, containing information such as the name
    struct PropertyRecord
    {
        PropertyType type;          //!< Type of the property, PROPERTYTYPE_xxx constant
        int size;                   //!< Size in bytes of the property (> 0)
        const char * name;          //!< Name of the property, starting with an uppercase letter (non-empty)
        void * varPtr;              //!< Pointer to the variable (!= nullptr)
        const char * className;     //!< Name of the class owning the property (non-empty)
    };

    //! Register a property
    //! \param type Type of the property, PROPERTYTYPE_xxx constant
    //! \param name Name of the property, starting with an uppercase letter
    //! \param varPtr Pointer to the property variable
    //! \param className Class name of the property grid owner
    //! \note Called by \a IMPLEMENT_PROPERTY() through the templated \a RegisterProperty() function
    void RegisterProperty(PropertyType type, int size, const char * name,
                          void * varPtr, const char * className);


    //! \todo Store list of PropertyRecord, filled by RegisterProperty()
};


//----------------------------------------------------------------------------------------

// Implementation

template <typename T>
void PropertyGrid::RegisterProperty(const char * name, T * varPtr, const char * className)
{
    RegisterProperty(PropertyDefinition<T>::PROPERTY_TYPE, sizeof(T), name, varPtr, className);
}


}   // namespace PropertyGrid
}   // namespace Pegasus

#endif  // PEGASUS_PROPERTYGRID_PROPERTYGRID_H
