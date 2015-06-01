/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	PropertyDefs.h
//! \author	Kevin Boulanger
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

    NUM_PROPERTY_TYPES,

    PROPERTYTYPE_INVALID = NUM_PROPERTY_TYPES
};


}   // namespace Pegasus
}   // namespace PropertyGrid

#endif  // PEGASUS_PROPERTYGRID_SHARED_PROPERTYDEFS_H
