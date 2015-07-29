/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	PropertyGridManager.cpp
//! \author	Kevin Boulanger
//! \date	28th January 2015
//! \brief	Manager for all classes using a property grid, with all the member information

#include "Pegasus/PropertyGrid/PropertyGridManager.h"
#include "Pegasus/Utils/String.h"

namespace Pegasus {
namespace PropertyGrid {


PropertyGridManager::PropertyGridManager()
:   mClassInfos(&PropertyGridStaticAllocator::GetInstance()),
    mCurrentClassInfo(nullptr)
#if PEGASUS_ENABLE_PROXIES
,   mProxy()
#endif
{
}

//----------------------------------------------------------------------------------------

PropertyGridManager::~PropertyGridManager()
{
}

//----------------------------------------------------------------------------------------

PropertyGridManager & PropertyGridManager::GetInstance()
{
    // Return the singleton
    static PropertyGridManager sInstance;
    return sInstance;
}

//----------------------------------------------------------------------------------------

void PropertyGridManager::BeginDeclareProperties(const char * className, const char * parentClassName)
{
    //! \todo Test if the current class info is null
    //! \todo Test if className is non-empty
    //! \todo Test that the class name doesn't already exist
    //! \todo After main() is called and the assertion handler is working, do a runtime validation
    //!       of the class list (no duplicates), and check that the connections between classes are correct
    //!       (inheritance)

    mCurrentClassInfo = &mClassInfos.PushEmpty();
    mCurrentClassInfo->SetClassName(className, parentClassName);

    // The parent information is connected later, as it is possible the parent class has not been defined yet.
    // The link will be executed at runtime using \a ResolveInternalClassHierarchy()
    mCurrentClassInfo->SetParentClassInfo(nullptr);
}

//----------------------------------------------------------------------------------------

void PropertyGridManager::ResolveInternalClassHierarchy()
{
    // Resolve the links between the classes (to know who is the parent class of each class)
    //! \todo Make ci an unsigned int (after upgrading Vector)
    for (int ci = 0; ci < mClassInfos.GetSize(); ++ci)
    {
        PropertyGridClassInfo * classInfo = &mClassInfos[ci];

        // Find the class info for the parent class if defined and link it
        if (classInfo->GetParentClassName()[0] != '\0')
        {
            PropertyGridClassInfo* parentInfo = nullptr;
            //! \todo Make pi an unsigned int (after upgrading Vector)
            for (int pi = 0; pi < mClassInfos.GetSize(); ++pi)
            {
                if (Utils::Strcmp(mClassInfos[pi].GetClassName(), classInfo->GetParentClassName()) == 0)
                {
                    parentInfo = &mClassInfos[pi];
                    break;
                }
            }

            PG_ASSERTSTR(parentInfo != nullptr, "Parent class not found");
            classInfo->SetParentClassInfo(parentInfo);
        }
    }

    // Resolve the number of properties in each class (since the classes are not declared in a specific order,
    // the parent's number of properties is not known yet at declaration time)
    //! \todo Make ci an unsigned int (after upgrading Vector)
    for (int ci = 0; ci < mClassInfos.GetSize(); ++ci)
    {
        mClassInfos[ci].UpdateNumPropertiesFromParents();
    }
}

//----------------------------------------------------------------------------------------

void PropertyGridManager::DeclareProperty(PropertyType type, int size, const char * name, void * defaultValuePtr)
{
    //! \todo Test if the current class info is defined
    //! \todo Test if name is non-null?

    mCurrentClassInfo->DeclareProperty(type, size, name, defaultValuePtr);
}

//----------------------------------------------------------------------------------------

void PropertyGridManager::EndDeclareProperties()
{
    //! \todo Test if the current class info is non-null

    mCurrentClassInfo = nullptr;
}

//----------------------------------------------------------------------------------------

const PropertyGridClassInfo & PropertyGridManager::GetClassInfo(unsigned int index) const
{
    //! \todo Test for the validity of index

    return mClassInfos[index];
}

//----------------------------------------------------------------------------------------

const PropertyGridClassInfo * PropertyGridManager::GetClassInfo(const char * className) const
{
    if (className == nullptr)
    {
        PG_FAILSTR("Invalid class name when retrieving a class info");
        return nullptr;
    }

    //! \todo Make ci an unsigned int (after upgrading Vector)
    for (int ci = 0; ci < mClassInfos.GetSize(); ++ci)
    {
        if (Utils::Strcmp(mClassInfos[ci].GetClassName(), className) == 0)
        {
            // Class found
            return &mClassInfos[ci];
        }
    }

    // Class not found
    PG_FAILSTR("Class not found (%s) when retrieving a class info", className);
    return nullptr;
}


}   // namespace PropertyGrid
}   // namespace Pegasus
