/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	PropertyGrid.cpp
//! \author	Kevin Boulanger
//! \date	24th April 2014
//! \brief	Property grid, defining a set of editable properties

#include "Pegasus/PropertyGrid/PropertyGrid.h"

namespace Pegasus {
namespace PropertyGrid {


PropertyGrid::PropertyGrid()
:   mDirty(true)
{
    //! \todo Implement
}

//----------------------------------------------------------------------------------------

#if PEGASUS_ENABLE_PROXIES

unsigned int PropertyGrid::GetNumProperties() const
{
    //! \todo Implement
    /***/return 0;
}

//----------------------------------------------------------------------------------------

PropertyType PropertyGrid::GetPropertyType(unsigned int index) const
{
    //! \todo Implement
    /***/return PROPERTYTYPE_INVALID;
}

//----------------------------------------------------------------------------------------

const char * PropertyGrid::GetPropertyName(unsigned int index) const
{
    //! \todo Implement
    /***/return "";
}

//----------------------------------------------------------------------------------------

const char * PropertyGrid::GetPropertyClassName(unsigned int index) const
{
    //! \todo Implement
    /***/return "";
}

//----------------------------------------------------------------------------------------

void PropertyGrid::SetProperty(const char * name, void * valuePtr)
{
    //! \todo Implement
}

//----------------------------------------------------------------------------------------

const void * PropertyGrid::GetProperty(const char * name) const
{
    //! \todo Implement
    /***/return nullptr;
}

#endif  // PEGASUS_ENABLE_PROXIES
    
//----------------------------------------------------------------------------------------

void PropertyGrid::RegisterProperty(PropertyType type, int size, const char * name, void * varPtr, const char * className)
{
    if (   (type < NUM_PROPERTY_TYPES)
        && (size > 0)
        && (name != nullptr)
        && (name[0] != '\0')
        && (varPtr != nullptr)
        && (className != nullptr)
        && (className[0] != '\0') )
    {
        PropertyRecord record;
        record.type = type;
        record.size = size;
        record.name = name;                 // Copy the pointer, not the string, since the input pointer is considered as constant
        record.varPtr = varPtr;
        record.className = className;       // Copy the pointer, not the string, since the input pointer is considered as constant

        //! \todo Store the record
        /****/
    }
    else
    {
        if (   (name != nullptr)
            && (name[0] != '\0')
            && (className != nullptr)
            && (className[0] != '\0') )
        {
            PG_FAILSTR("Invalid property declaration for \"%s\" in class \"%s\"", name, className)
        }
        else
        {
            PG_FAILSTR("Invalid property declaration");
        }
    }
}


}   // namespace PropertyGrid
}   // namespace Pegasus
