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

unsigned int PropertyGridObjectProxy::GetNumDerivedClassProperties() const
{
    return mObject->GetNumDerivedClassProperties();
}

//----------------------------------------------------------------------------------------

const PropertyRecord & PropertyGridObjectProxy::GetDerivedClassPropertyRecord(unsigned int index) const
{
    return mObject->GetDerivedClassPropertyRecord(index);
}

//----------------------------------------------------------------------------------------

void PropertyGridObjectProxy::ReadDerivedClassProperty(unsigned int index, void * outputBuffer, unsigned int outputBufferSize)
{
    const PropertyAccessor accessor = mObject->GetDerivedClassPropertyAccessor(index);
    accessor.Read(outputBuffer, outputBufferSize);
}

//----------------------------------------------------------------------------------------

void PropertyGridObjectProxy::WriteDerivedClassProperty(unsigned int index, const void * inputBuffer, unsigned int inputBufferSize)
{
    const PropertyAccessor accessor = mObject->GetDerivedClassPropertyAccessor(index);
    accessor.Write(inputBuffer, inputBufferSize);
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

void PropertyGridObjectProxy::WriteClassProperty(unsigned int index, const void * inputBuffer, unsigned int inputBufferSize, bool sendMessage)
{
    const PropertyAccessor accessor = mObject->GetClassPropertyAccessor(index);
    IPropertyListener* tempListener = nullptr;
    if (!sendMessage)
    {
        tempListener = mObject->GetEventListener();
        mObject->SetEventListener(nullptr);
    }
    accessor.Write(inputBuffer, inputBufferSize);

    if (!sendMessage)
    {
        mObject->SetEventListener(tempListener);
    }
}

//----------------------------------------------------------------------------------------

unsigned int PropertyGridObjectProxy::GetNumObjectProperties() const
{
    return mObject->GetNumObjectProperties();
}

//----------------------------------------------------------------------------------------

const PropertyRecord & PropertyGridObjectProxy::GetObjectPropertyRecord(unsigned int index) const
{
    return mObject->GetObjectPropertyRecord(index);
}

//----------------------------------------------------------------------------------------

void PropertyGridObjectProxy::ReadObjectProperty(unsigned int index, void * outputBuffer, unsigned int outputBufferSize)
{
    const PropertyAccessor accessor = mObject->GetObjectPropertyAccessor(index);
    accessor.Read(outputBuffer, outputBufferSize);
}

//----------------------------------------------------------------------------------------

void PropertyGridObjectProxy::WriteObjectProperty(unsigned int index, const void * inputBuffer, unsigned int inputBufferSize, bool sendMessage)
{
    const PropertyAccessor accessor = mObject->GetObjectPropertyAccessor(index);
    IPropertyListener* tempListener = nullptr;
    if (!sendMessage)
    {
        tempListener = mObject->GetEventListener();
        mObject->SetEventListener(nullptr);
    }
    accessor.Write(inputBuffer, inputBufferSize);
    if (!sendMessage)
    {
        mObject->SetEventListener(tempListener);
    }
}

//----------------------------------------------------------------------------------------

void PropertyGridObjectProxy::SetEventListener(IPropertyListener* listener)
{
    mObject->SetEventListener(listener);
}

//----------------------------------------------------------------------------------------

void PropertyGridObjectProxy::SetUserData(Pegasus::Core::IEventUserData* userData)
{
    mObject->SetEventUserData(userData);
}

//----------------------------------------------------------------------------------------

Pegasus::Core::IEventUserData* PropertyGridObjectProxy::GetUserData() const
{
    return mObject->GetEventUserData();
}


}   // namespace PropertyGrid
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
