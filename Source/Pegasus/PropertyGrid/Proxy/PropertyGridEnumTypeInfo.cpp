/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   PropertyEnumTypeInfo.cpp
//! \author Kleber Garcia
//! \date   October 17, 2015
//! \brief  Proxy implementation for enumeration type information

#include "Pegasus/PropertyGrid/Proxy/PropertyGridEnumTypeInfo.h"
#include "Pegasus/PropertyGrid/Shared/PropertyDefs.h"
#include "Pegasus/PropertyGrid/PropertyGridEnumType.h"

using namespace Pegasus;
using namespace Pegasus::PropertyGrid;

const char* EnumTypeInfoProxy::GetName() const
{

    return mObject->GetName();

}

const BaseEnumType** EnumTypeInfoProxy::GetEnumerations(unsigned int & outSize) const
{

    outSize = mObject->GetEnumerations().GetSize();
    return mObject->GetEnumerations().Data();

}

