/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   PropertyEnumType.cpp
//! \author Kleber Garcia
//! \date   October 5, 2015
//! \brief  Definition of a property grid enumeration type. Contains metadata necessary.

#include "Pegasus/PropertyGrid/PropertyGridEnumType.h"

namespace Pegasus {
    namespace PropertyGrid {

        int EnumBuilder::sCurrVal = 0;

        EnumTypeInfo::EnumTypeInfo()
        : mName(nullptr), mEnumValues(&PropertyGridStaticAllocator::GetInstance()) 
#if PEGASUS_ENABLE_PROXIES
        , mProxy(this)
#endif
        {
        }


        EnumTypeInfo::~EnumTypeInfo()
        {
        }
        
    }
}
