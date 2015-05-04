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

namespace Pegasus {
namespace PropertyGrid {


PropertyGridManager::PropertyGridManager()
:   mClassInfos(&PropertyGridStaticAllocator::GetInstance()),
    mCurrentClassInfo(nullptr)
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

void PropertyGridManager::BeginDeclareProperties(const char * className)
{
    //! \todo Test if the current class info is null
    //! \todo Test if className is non-empty
    //! \todo Test that the class name doesn't already exist

    mCurrentClassInfo = &mClassInfos.PushEmpty();
    mCurrentClassInfo->SetClassName(className);
}

//----------------------------------------------------------------------------------------

void PropertyGridManager::DeclareProperty(PropertyType type, int size, const char * name)
{
    //! \todo Test if the current class info is defined
    //! \todo Test if name is non-null?

    mCurrentClassInfo->RegisterProperty(type, size, name);
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
