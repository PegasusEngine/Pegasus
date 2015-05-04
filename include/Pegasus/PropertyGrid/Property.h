/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Property.h
//! \author	Kevin Boulanger
//! \date	05th February 2015
//! \brief	Definition of a property (and its specifications)

#ifndef PEGASUS_PROPERTYGRID_PROPERTY_H
#define PEGASUS_PROPERTYGRID_PROPERTY_H

#include "Pegasus/Math/Vector.h"
#include "Pegasus/Math/Color.h"
#include "Pegasus/Utils/Memcpy.h"

namespace Pegasus {
namespace PropertyGrid {


//! Static string types for property grid
typedef char String64[64];

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
struct PropertyDefinition<bool>
{
    static const PropertyType PROPERTY_TYPE = PROPERTYTYPE_BOOL;
    typedef bool VarType;
    typedef bool ParamType;
    typedef bool ReturnType;
    static void SetProperty(ParamType paramIn, VarType& paramOut) { paramOut = paramIn; }
};

template <>
struct PropertyDefinition<unsigned int>
{
    static const PropertyType PROPERTY_TYPE = PROPERTYTYPE_UINT;
    typedef unsigned int VarType;
    typedef unsigned int ParamType;
    typedef unsigned int ReturnType;
    static void SetProperty(ParamType paramIn, VarType& paramOut) { paramOut = paramIn; }
};

template <>
struct PropertyDefinition<int>
{
    static const PropertyType PROPERTY_TYPE = PROPERTYTYPE_INT;
    typedef int VarType;
    typedef int ParamType;
    typedef int ReturnType;
    static void SetProperty(ParamType paramIn, VarType& paramOut) { paramOut = paramIn; }
};

template <>
struct PropertyDefinition<float>
{
    static const PropertyType PROPERTY_TYPE = PROPERTYTYPE_FLOAT;
    typedef float VarType;
    typedef float ParamType;
    typedef float ReturnType;
    static void SetProperty(ParamType paramIn, VarType& paramOut) { paramOut = paramIn; }
};

template <>
struct PropertyDefinition<Math::Vec2>
{
    static const PropertyType PROPERTY_TYPE = PROPERTYTYPE_VEC2;
    typedef Math::Vec2       VarType;
    typedef Math::Vec2In     ParamType;
    typedef Math::Vec2Return ReturnType;
    static void SetProperty(ParamType paramIn, VarType& paramOut) { paramOut = paramIn; }
};

template <>
struct PropertyDefinition<Math::Vec3>
{
    static const PropertyType PROPERTY_TYPE = PROPERTYTYPE_VEC3;
    typedef Math::Vec3       VarType;
    typedef Math::Vec3In     ParamType;
    typedef Math::Vec3Return ReturnType;
    static void SetProperty(ParamType paramIn, VarType& paramOut) { paramOut = paramIn; }
};

template <>
struct PropertyDefinition<Math::Vec4>
{
    static const PropertyType PROPERTY_TYPE = PROPERTYTYPE_VEC4;
    typedef Math::Vec4       VarType;
    typedef Math::Vec4In     ParamType;
    typedef Math::Vec4Return ReturnType;
    static void SetProperty(ParamType paramIn, VarType& paramOut) { paramOut = paramIn; }
};

template <>
struct PropertyDefinition<Math::Color8RGB>
{
    static const PropertyType PROPERTY_TYPE = PROPERTYTYPE_COLOR8RGB;
    typedef Math::Color8RGB       VarType;
    typedef Math::Color8RGBIn     ParamType;
    typedef Math::Color8RGBReturn ReturnType;
    static void SetProperty(ParamType paramIn, VarType& paramOut) { paramOut = paramIn; }
};

template <>
struct PropertyDefinition<Math::Color8RGBA>
{
    static const PropertyType PROPERTY_TYPE = PROPERTYTYPE_COLOR8RGBA;
    typedef Math::Color8RGBA       VarType;
    typedef Math::Color8RGBAIn     ParamType;
    typedef Math::Color8RGBAReturn ReturnType;
    static void SetProperty(ParamType paramIn, VarType& paramOut) { paramOut = paramIn; }
};

template <>
struct PropertyDefinition<String64>
{
    static const PropertyType PROPERTY_TYPE = PROPERTYTYPE_STRING64;
    typedef String64     VarType;
    typedef const char*  ParamType;
    typedef const char*  ReturnType;
    static void SetProperty(ParamType paramIn, VarType& paramOut) { Pegasus::Utils::Memcpy(paramOut, paramIn, sizeof(VarType)); }
};
//@}


}   // namespace PropertyGrid
}   // namespace Pegasus

#endif  // PEGASUS_PROPERTYGRID_PROPERTY_H
