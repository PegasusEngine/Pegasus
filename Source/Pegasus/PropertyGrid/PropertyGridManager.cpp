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
    mCurrentClassInfo->SetParentClassInfo(nullptr); //info is connected later, first gather all the plane class info
}

//----------------------------------------------------------------------------------------

void PropertyGridManager::ResolveInternalClassHierarchy()
{
    for (int ci = 0; ci < mClassInfos.GetSize(); ++ci)
    {
        PropertyGridClassInfo * classInfo = &mClassInfos[ci];

        // Find the class info for the parent class if defined and link it
        if (classInfo->GetParentClassName()[0] != '\0')
        {
            //! \todo Make ci an unsigned int (after upgrading Vector)
            PropertyGridClassInfo* parentInfo = nullptr;
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


}   // namespace PropertyGrid
}   // namespace Pegasus
