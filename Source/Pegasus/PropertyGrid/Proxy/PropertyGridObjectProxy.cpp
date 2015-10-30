/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	PropertyGridObjectProxy.cpp
//! \author	Karolyn Boulanger
//! \date	14th September 2015
//! \brief	Proxy object, used by the editor to interact with a property grid object

PEGASUS_AVOID_EMPTY_FILE_WARNING

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/PropertyGrid/Proxy/PropertyGridObjectProxy.h"
#include "Pegasus/PropertyGrid/PropertyGridObject.h"

namespace Pegasus {
namespace PropertyGrid {


PropertyGridObjectProxy::PropertyGridObjectProxy(PropertyGridObject * object)
:   mObject(object)
{
    PG_ASSERT(object != nullptr);
}

//----------------------------------------------------------------------------------------

PropertyGridObjectProxy::~PropertyGridObjectProxy()
{
}

//----------------------------------------------------------------------------------------

const IPropertyGridClassInfoProxy * PropertyGridObjectProxy::GetClassInfoProxy() const
{
    return mObject->GetClassInfo()->GetProxy();
}

//----------------------------------------------------------------------------------------

unsigned int PropertyGridObjectProxy::GetNumClassProperties() const
{
    return mObject->GetNumClassProperties();
}

//----------------------------------------------------------------------------------------

const PropertyRecord & PropertyGridObjectProxy::GetClassPropertyRecord(unsigned int index) const
{
    return mObject->GetClassPropertyRecord(index);
}

//----------------------------------------------------------------------------------------

void PropertyGridObjectProxy::ReadClassProperty(unsigned int index, void * outputBuffer, unsigned int outputBufferSize)
{
    const PropertyAccessor accessor = mObject->GetClassPropertyAccessor(index);
    accessor.Read(outputBuffer, outputBufferSize);
}

//----------------------------------------------------------------------------------------

void PropertyGridObjectProxy::WriteClassProperty(unsigned int index, const void * inputBuffer, unsigned int inputBufferSize)
{
    const PropertyAccessor accessor = mObject->GetClassPropertyAccessor(index);
    accessor.Write(inputBuffer, inputBufferSize);
}

//----------------------------------------------------------------------------------------

unsigned int PropertyGridObjectProxy::GetNumProperties() const
{
    return mObject->GetNumProperties();
}

//----------------------------------------------------------------------------------------

const PropertyRecord & PropertyGridObjectProxy::GetPropertyRecord(unsigned int index) const
{
    return mObject->GetPropertyRecord(index);
}

//----------------------------------------------------------------------------------------

void PropertyGridObjectProxy::ReadProperty(unsigned int index, void * outputBuffer, unsigned int outputBufferSize)
{
    const PropertyAccessor accessor = mObject->GetPropertyAccessor(index);
    accessor.Read(outputBuffer, outputBufferSize);
}

//----------------------------------------------------------------------------------------

void PropertyGridObjectProxy::WriteProperty(unsigned int index, const void * inputBuffer, unsigned int inputBufferSize)
{
    const PropertyAccessor accessor = mObject->GetPropertyAccessor(index);
    accessor.Write(inputBuffer, inputBufferSize);
}


}   // namespace PropertyGrid
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
