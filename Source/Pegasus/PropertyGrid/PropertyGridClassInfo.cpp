/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	PropertyGridClassInfo.cpp
//! \author	Kevin Boulanger
//! \date	28th January 2015
//! \brief	Information about a class containing property grid members

#include "Pegasus/PropertyGrid/PropertyGridClassInfo.h"
#include "Pegasus/PropertyGrid/PropertyGridStaticAllocator.h"

namespace Pegasus {
namespace PropertyGrid {


PropertyGridClassInfo::PropertyGridClassInfo()
:   mClassName("")
,   mPropertyRecords(&PropertyGridStaticAllocator::GetInstance())
{
}

//----------------------------------------------------------------------------------------

PropertyType PropertyGridClassInfo::GetPropertyType(unsigned int index) const
{
    //! \todo Test for the validity of the index

    return mPropertyRecords[index].type;
}

//----------------------------------------------------------------------------------------

unsigned int PropertyGridClassInfo::GetPropertySize(unsigned int index) const
{
    //! \todo Test for the validity of the index

    return mPropertyRecords[index].size;
}

//----------------------------------------------------------------------------------------

const char * PropertyGridClassInfo::GetPropertyName(unsigned int index) const
{
    //! \todo Test for the validity of the index

    return mPropertyRecords[index].name;
}

//----------------------------------------------------------------------------------------

//const char * PropertyGridClassInfo::GetPropertyClassName(unsigned int index) const
//{
//    //! \todo Test for the validity of the index
//    //! \todo Implement the proper behavior, if this function is useful
//
//    //return mPropertyRecords[index].className;
//    return "";
//}

//----------------------------------------------------------------------------------------

void PropertyGridClassInfo::SetClassName(const char * className)
{
    // Copy the pointer, not the string itself. The caller is the owner of the string.
    // Do not test for validity, as this is called at registration time,
    // before main() is even called
    mClassName = className;
}

//----------------------------------------------------------------------------------------
    
void PropertyGridClassInfo::RegisterProperty(PropertyType type, int size, const char * name/*,
                        //! \todo **** Check if we need those variables
                                             void * varPtr, const char * className*/)
{
    if (   (type < NUM_PROPERTY_TYPES)
        && (size > 0)
        && (name != nullptr)
        && (name[0] != '\0')
    //! \todo **** Check if we need those variables
        //&& (varPtr != nullptr)
        //&& (className != nullptr)
        /*&& (className[0] != '\0')*/ )
    {
        PropertyRecord & record = mPropertyRecords.PushEmpty();
        record.type = type;
        record.size = size;
        record.name = name;                 // Copy the pointer, not the string, since the input pointer is considered as constant
    //! \todo **** Check if we need those variables
        //record.varPtr = varPtr;
        //record.className = className;       // Copy the pointer, not the string, since the input pointer is considered as constant
    }
    else
    {
        if (   (name != nullptr)
            && (name[0] != '\0')
    //! \todo **** Check if we need those variables
            /*&& (className != nullptr)
            && (className[0] != '\0')*/ )
        {
            // CANNOT work since it is happening before main()
            //***PG_FAILSTR("Invalid property declaration for \"%s\" in class \"%s\"", name, className)
        }
        else
        {
            // CANNOT work since it is happening before main()
            //***PG_FAILSTR("Invalid property declaration");
        }
    }
}


}   // namespace PropertyGrid
}   // namespace Pegasus
