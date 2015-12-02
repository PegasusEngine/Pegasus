/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	PropertyGridIOMessageController.cpp
//! \author	Kleber Garcia
//! \date	November 2nd 2015
//! \brief	PropertyGrid IO controller - thread safe, communicates with application and sets
//!         correct state for application property grid

#include "Pegasus/Preprocessor.h"
#include "MessageControllers/PropertyGridIOMessageController.h"
#include "Pegasus/PropertyGrid/Shared/IPropertyGridObjectProxy.h"

PropertyGridIOMessageController::PropertyGridIOMessageController(Pegasus::App::IApplicationProxy* app)
 : mApp(app), mNextHandle(0)
{
}

PropertyGridIOMessageController::~PropertyGridIOMessageController()
{
}

void PropertyGridIOMessageController::OnRenderThreadProcessMessage(const PropertyGridIOMessageController::Message& m)
{
    switch(m.GetMessageType())
    {
    case PropertyGridIOMessageController::Message::UPDATE:
        OnRenderThreadUpdate(m.GetPropertyGridObserver(), m.GetPropertyGridHandle(), m.GetUpdateBatch());
        break;
    case PropertyGridIOMessageController::Message::CLOSE:
        OnRenderThreadClose(m.GetPropertyGridObserver(), m.GetPropertyGridHandle());
        break;
    case PropertyGridIOMessageController::Message::OPEN:
        OnRenderThreadOpen(m.GetPropertyGridObserver(), m.GetPropertyGrid());
        break;
    default:
        ED_FAILSTR("Invalid message");
    }
}

void PropertyGridIOMessageController::OnRenderThreadUpdate(PropertyGridObserver* sender, PropertyGridHandle handle, const QVector<PropertyGridIOMessageController::UpdateElement>& elements)
{
    ED_ASSERT(handle != INVALID_PGRID_HANDLE);
    PropertyGridIOMessageController::HandleToProxyMap::iterator it = mActiveProperties.find(handle);
    ED_ASSERT(it != mActiveProperties.end());
    if(it != mActiveProperties.end())
    {
        Pegasus::PropertyGrid::IPropertyGridObjectProxy* pgrid = it.value();
        for (int i = 0; i < elements.size(); ++i)
        {
            const PropertyGridIOMessageController::UpdateElement & el = elements[i];
            switch (el.mCategory)
            {
                case Pegasus::PropertyGrid::PROPERTYCATEGORY_CLASS:
                    {
                        int propSizes = pgrid->GetNumClassProperties();
                        if (elements[i].mIndex < propSizes)
                        {
                            const Pegasus::PropertyGrid::PropertyRecord& r = pgrid->GetClassPropertyRecord(elements[i].mIndex);
                            if (r.type == elements[i].mType)
                            {
                                pgrid->WriteClassProperty(elements[i].mIndex, &elements[i].mData, r.size, /*don't send message*/ false);
                            }
                            else
                            {
                                ED_FAILSTR("Failed reading class property type.");
                            }
                        }
                        else
                        {
                            ED_FAILSTR("Failed setting class property.");
                        }
                    }
                    break;

                case Pegasus::PropertyGrid::PROPERTYCATEGORY_OBJECT:
                    {
                        int propSizes = pgrid->GetNumObjectProperties();
                        if (elements[i].mIndex < propSizes)
                        {
                            const Pegasus::PropertyGrid::PropertyRecord& r = pgrid->GetObjectPropertyRecord(elements[i].mIndex);
                            if (r.type == elements[i].mType)
                            {
                                pgrid->WriteObjectProperty(elements[i].mIndex, &elements[i].mData, r.size, /*don't send message*/ false);
                            }
                            else
                            {
                                ED_FAILSTR("Failed reading object property type.");
                            }
                        }
                        else
                        {
                            ED_FAILSTR("Failed setting object property.");
                        }
                    }
                    break;

                default:
                    ED_FAILSTR("Invalid category for a property");
                    break;
            }
        }
    }
}

void PropertyGridIOMessageController::OnRenderThreadOpen(PropertyGridObserver* sender, Pegasus::PropertyGrid::IPropertyGridObjectProxy* proxy)
{
    ED_ASSERT(proxy != nullptr);
    
    PropertyGridHandle handle  = INVALID_PGRID_HANDLE;
    if (proxy->GetUserData() == nullptr)
    {

        handle = mNextHandle++;        
        mActiveProperties.insert(handle, proxy);
        mObservers.insert(handle, QSet<PropertyGridObserver*>());
        PropertyGridIOMessageController::UpdateCache newUpdateCache;
        newUpdateCache.mHandle = handle;
        PropertyGridIOMessageController::ProxyUpdateCache::iterator it = mUpdateCache.insert(handle, newUpdateCache);
        proxy->SetEventListener(this);
        ED_ASSERT(proxy->GetUserData() == nullptr);
        proxy->SetUserData(new PropertyUserData(proxy, &(it.value()), handle));
    }
    else
    {
        PropertyUserData* u = static_cast<PropertyUserData*>(proxy->GetUserData());
        handle = u->GetHandle();
    }

    ED_ASSERT(mObservers.find(handle) != mObservers.end());
    QSet<PropertyGridObserver*>& observerList = mObservers.find(handle).value();
    observerList.insert(sender);

    emit sender->OnInitializedSignal(handle, proxy);

    //now notify the observers to update the uis looking at this object
    UpdateObserverInternal(sender, handle, proxy);

}

void PropertyGridIOMessageController::UpdateObserverInternal(PropertyGridObserver* sender, PropertyGridHandle handle, Pegasus::PropertyGrid::IPropertyGridObjectProxy* proxy)
{

    //! \todo Refactor to loop over the categories
    QVector<PropertyGridIOMessageController::UpdateElement> updates;

    // Prepare list of updates for class properties
    for (unsigned int i = 0; i < proxy->GetNumClassProperties(); ++i)
    {
        const Pegasus::PropertyGrid::PropertyRecord& r = proxy->GetClassPropertyRecord(i);
        PropertyGridIOMessageController::UpdateElement el;
        el.mCategory = Pegasus::PropertyGrid::PROPERTYCATEGORY_CLASS;
        el.mType = r.type;
        el.mIndex = (int)i;
        if (r.size  <= sizeof(el.mData))
        {
            proxy->ReadClassProperty(i, &el.mData, r.size);
            updates.push_back(el);
        }
        else
        {
            ED_FAILSTR("Data container for ui cannot fit current class property!");
        }
    }

    // Prepare list of updates for object properties
    for (unsigned int i = 0; i < proxy->GetNumObjectProperties(); ++i)
    {
        const Pegasus::PropertyGrid::PropertyRecord& r = proxy->GetObjectPropertyRecord(i);
        PropertyGridIOMessageController::UpdateElement el;
        el.mCategory = Pegasus::PropertyGrid::PROPERTYCATEGORY_OBJECT;
        el.mType = r.type;
        el.mIndex = (int)i;
        if (r.size  <= sizeof(el.mData))
        {
            proxy->ReadObjectProperty(i, &el.mData, r.size);
            updates.push_back(el);
        }
        else
        {
            ED_FAILSTR("Data container for ui cannot fit current object property!");
        }
    }

    //send signal to UI so it can update the view
    emit sender->OnUpdatedSignal(handle, updates);
}
void PropertyGridIOMessageController::CloseHandleInternal(PropertyGridHandle handle)
{
    PropertyGridIOMessageController::HandleToProxyMap::iterator proxyIt = mActiveProperties.find(handle);
    PropertyGridIOMessageController::ProxyUpdateCache::iterator cacheIt = mUpdateCache.find(handle);

    proxyIt.value()->SetEventListener(nullptr);
    delete proxyIt.value()->GetUserData();
    proxyIt.value()->SetUserData(nullptr);

        
    mActiveProperties.erase(proxyIt);
    mUpdateCache.erase(cacheIt);
}

void PropertyGridIOMessageController::OnRenderThreadClose(PropertyGridObserver* sender, PropertyGridHandle handle)
{
    ED_ASSERT(handle != INVALID_PGRID_HANDLE);

    QSet<PropertyGridObserver*>obsSet = mObservers.find(handle).value();
    obsSet.erase(obsSet.find(sender));
    
    if (obsSet.empty())
    {
        CloseHandleInternal(handle);
    }
    
}

void PropertyGridIOMessageController::OnEvent(Pegasus::Core::IEventUserData* userData, Pegasus::PropertyGrid::ValueChangedEventIndexed& e)
{   
    PropertyUserData* pUserData = static_cast<PropertyUserData*>(userData);
    const Pegasus::PropertyGrid::PropertyRecord & r = pUserData->GetProxy()->GetClassPropertyRecord(e.GetIndex());

    PropertyGridIOMessageController::UpdateElement el;
    el.mCategory = e.GetCategory();
    el.mIndex = e.GetIndex();
    el.mType = r.type;
    if (el.mIndex > 0 && r.size <= sizeof(el.mData))
    {
        pUserData->GetProxy()->ReadClassProperty(el.mIndex, &el.mData, r.size);
    }
    else
    {
        ED_FAIL();
    }
    pUserData->GetUpdateCache()->mUpdateCache.push_back(el);

    const int  MAX_CACHE_SIZE = 20;
    if (pUserData->GetUpdateCache()->mUpdateCache.size() >= MAX_CACHE_SIZE)
    {
        FlushPendingUpdates(pUserData);
    }
}

void PropertyGridIOMessageController::OnEvent(Pegasus::Core::IEventUserData* userData, Pegasus::PropertyGrid::PropertyGridRenderRequest& e)
{
    //request a new frame
    emit RequestRedraw();
}

void PropertyGridIOMessageController::FlushPendingUpdates(PropertyUserData* userData)
{
    UpdateCache* cache  = userData->GetUpdateCache();
    ObserverMap::iterator obsIt = mObservers.find(userData->GetHandle());
    if (obsIt != mObservers.end())
    {
        ObserverSet& observerSet = obsIt.value();
        foreach (PropertyGridObserver* obs, observerSet)
        {
            emit obs->OnUpdatedSignal(userData->GetHandle(), userData->GetUpdateCache()->mUpdateCache);
        }
    }
    cache->mUpdateCache.clear();
}

void PropertyGridIOMessageController::FlushAllPendingUpdates()
{
    // first see if any object requested a layout change
    if (!mLayoutsToReset.empty())
    {
        foreach (PropertyGridHandle h, mLayoutsToReset)
        {
            ObserverMap::iterator it = mObservers.find(h);
            HandleToProxyMap::iterator h2pit = mActiveProperties.find(h);
            if (h2pit == mActiveProperties.end()) continue;
            Pegasus::PropertyGrid::IPropertyGridObjectProxy* proxy = h2pit.value();
            if (it != mObservers.end())
            {
                foreach(PropertyGridObserver* obs, it.value())
                {
                    emit (obs->OnShutdownSignal(h));
                    emit(obs->OnInitializedSignal(h, proxy));
                    UpdateObserverInternal(obs, h, proxy);
                }
            }
        }
        mLayoutsToReset.clear();
    }

    HandleToProxyMap::iterator it = mActiveProperties.begin();
    for (; it != mActiveProperties.end(); ++it)
    {
        PropertyUserData* userData = static_cast<PropertyUserData*>(it.value()->GetUserData());
        FlushPendingUpdates(userData);
    }

}

void PropertyGridIOMessageController::OnEvent(Pegasus::Core::IEventUserData* userData, Pegasus::PropertyGrid::PropertyGridDestroyed& e)
{
    PropertyUserData* pUserData = static_cast<PropertyUserData*>(userData);
    Pegasus::PropertyGrid::IPropertyGridObjectProxy* proxy = pUserData->GetProxy();
    PropertyGridHandle handle = pUserData->GetHandle();
    
    PropertyGridIOMessageController::ObserverMap::iterator obsIt = mObservers.find(handle);
    if (obsIt != mObservers.end())
    {
        PropertyGridIOMessageController::ObserverSet& set = obsIt.value();
        foreach(PropertyGridObserver* obs, set)
        {
            emit obs->OnShutdownSignal(handle);
        }

        mObservers.erase(obsIt);

        CloseHandleInternal(handle);
        
    }
    else
    {
        ED_FAIL();
    }
}

void PropertyGridIOMessageController::OnEvent(Pegasus::Core::IEventUserData* userData, Pegasus::PropertyGrid::ObjectPropertiesLayoutChanged& e)
{
    //accumulate all the layouts to reset for this particular property grid handle.
    PropertyUserData* pUserData = static_cast<PropertyUserData*>(userData);
    Pegasus::PropertyGrid::IPropertyGridObjectProxy* proxy = pUserData->GetProxy();
    PropertyGridHandle handle = pUserData->GetHandle();
    ED_ASSERT(handle != INVALID_PGRID_HANDLE);
    mLayoutsToReset.insert(handle);
}

PropertyGridObserver::PropertyGridObserver()
{
    connect(this, SIGNAL(OnInitializedSignal(PropertyGridHandle, const Pegasus::PropertyGrid::IPropertyGridObjectProxy*)),
            this, SLOT(OnInitializedSlot(PropertyGridHandle, const Pegasus::PropertyGrid::IPropertyGridObjectProxy*)), Qt::QueuedConnection);

    connect(this, SIGNAL(OnUpdatedSignal(PropertyGridHandle, QVector<PropertyGridIOMessageController::UpdateElement>)),
            this, SLOT(OnUpdatedSlot(PropertyGridHandle, QVector<PropertyGridIOMessageController::UpdateElement>)), Qt::QueuedConnection);

    connect(this, SIGNAL(OnShutdownSignal(PropertyGridHandle)),
            this, SLOT(OnShutdownSlot(PropertyGridHandle)), Qt::QueuedConnection);
}

