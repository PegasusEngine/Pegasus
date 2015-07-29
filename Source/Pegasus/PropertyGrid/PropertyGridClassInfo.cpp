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
,   mParentClassName("")
,   mParentClassInfo(nullptr)
,   mClassPropertyRecords(&PropertyGridStaticAllocator::GetInstance())
,   mNumProperties(0)
#if PEGASUS_ENABLE_PROXIES
,   mProxy(this)
#endif
{
}

//----------------------------------------------------------------------------------------

const PropertyGridClassInfo::PropertyRecord & PropertyGridClassInfo::GetClassProperty(unsigned int index) const
{
    //! \todo Test for the validity of the index

    return mClassPropertyRecords[index];
}

//----------------------------------------------------------------------------------------

const PropertyGridClassInfo::PropertyRecord & PropertyGridClassInfo::GetProperty(unsigned int index) const
{
    //! \todo Test for the validity of the index

    if (mParentClassInfo != nullptr)
    {
        if (index < mParentClassInfo->GetNumProperties())
        {
            // If a parent class is defined and the index is among the properties of the parent class,
            // call the parent class info to get the record
            return mParentClassInfo->GetProperty(index);
        }
        else
        {
            // If a parent class is defined and the index is among the properties of the current class,
            // offset the index and access the local info
            return mClassPropertyRecords[index - mParentClassInfo->GetNumProperties()];
        }
    }
    else
    {
        // If no parent class is defined, simply return the property record of the current class
        return mClassPropertyRecords[index];
    }
}

//----------------------------------------------------------------------------------------

void PropertyGridClassInfo::SetClassName(const char * className, const char * parentClassName)
{
    // Copy the pointers, not the string themselves. The caller is the owner of the strings.
    // Do not test for validity, as this is called at registration time,
    // before main() is even called
    mClassName = className;
    mParentClassName = parentClassName;
}

//----------------------------------------------------------------------------------------

void PropertyGridClassInfo::SetParentClassInfo(PropertyGridClassInfo * parentClassInfo)
{
    //! \todo Check that mParentClassInfo was nullptr before that call

    mParentClassInfo = parentClassInfo;
}

//----------------------------------------------------------------------------------------
    
void PropertyGridClassInfo::DeclareProperty(PropertyType type, int size, const char * name, void * defaultValuePtr)
{
    //! \todo Make size an unsigned int

    if (   (type < NUM_PROPERTY_TYPES)
        && (size > 0)
        && (name != nullptr)
        && (name[0] != '\0') )
    {
        PropertyRecord & record = mClassPropertyRecords.PushEmpty();
        record.type = type;
        record.size = size;
        record.name = name;     // Copy the pointer, not the string, since the input pointer is considered as constant
        record.defaultValuePtr = defaultValuePtr;
    }
    else
    {
        if (   (name != nullptr)
            && (name[0] != '\0') )
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

//----------------------------------------------------------------------------------------

void PropertyGridClassInfo::UpdateNumPropertiesFromParents()
{
    PG_ASSERT(mNumProperties == 0);

    const PropertyGridClassInfo * classInfo = this;
    do
    {
        mNumProperties += classInfo->GetNumClassProperties();
        classInfo = classInfo->mParentClassInfo;
    }
    while (classInfo != nullptr);
}


}   // namespace PropertyGrid
}   // namespace Pegasus
