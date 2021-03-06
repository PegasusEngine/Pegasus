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
#include "Pegasus/AssetLib/Shared/IAssetProxy.h"
#include "Pegasus/PegasusAssetTypes.h"
#include "Pegasus/Timeline/Shared/ITimelineProxy.h"
#include "Pegasus/Timeline/Shared/IBlockProxy.h"


PropertyGridIOMessageController::PropertyGridIOMessageController(Pegasus::App::IApplicationProxy* app, AssetIOMessageController* assetMessageController)
 : mApp(app), mNextHandle(0), mAssetMessageController(assetMessageController)
{
}

//----------------------------------------------------------------------------------------

PropertyGridIOMessageController::~PropertyGridIOMessageController()
{
}

//----------------------------------------------------------------------------------------

void PropertyGridIOMessageController::OnRenderThreadProcessMessage(const PropertyGridIOMCMessage& m)
{
    switch(m.GetMessageType())
    {
    case PropertyGridIOMCMessage::UPDATE:
        OnRenderThreadUpdate(m.GetPropertyGridObserver(), m.GetPropertyGridHandle(), m.GetUpdateBatch());
        break;
    case PropertyGridIOMCMessage::CLOSE:
        OnRenderThreadClose(m.GetPropertyGridObserver(), m.GetPropertyGridHandle());
        break;
    case PropertyGridIOMCMessage::OPEN:
        OnRenderThreadOpen(m.GetPropertyGridObserver(), m.GetTitle(), m.GetPropertyGrid());
        break;
    case PropertyGridIOMCMessage::OPEN_FROM_ASSET_HANDLE:
        OnRenderThreadOpenFromAsset(m.GetPropertyGridObserver(), m.GetAssetHandle());
        break;
    case PropertyGridIOMCMessage::OPEN_BLOCK_FROM_TIMELINE:
        OnRenderThreadOpenFromTimelineBlock(m.GetPropertyGridObserver(), m.GetTitle(), m.GetAssetHandle(), m.GetBlockGuid());
        break;
    default:
        ED_FAILSTR("Invalid message");
    }
}

//----------------------------------------------------------------------------------------

void PropertyGridIOMessageController::OnRenderThreadUpdate(PropertyGridObserver* sender, PropertyGridHandle handle, const QVector<PropertyGridIOMCUpdateElement>& elements)
{
    ED_ASSERT(handle != INVALID_PGRID_HANDLE);
    PropertyGridIOMessageController::HandleToProxyMap::iterator it = mActiveProperties.find(handle);
    ED_ASSERT(it != mActiveProperties.end());
    if(it != mActiveProperties.end())
    {
        Pegasus::PropertyGrid::IPropertyGridObjectProxy* pgrid = it.value();
        for (int i = 0; i < elements.size(); ++i)
        {
            const PropertyGridIOMCUpdateElement & el = elements[i];
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

//----------------------------------------------------------------------------------------
void PropertyGridIOMessageController::OnRenderThreadOpen(PropertyGridObserver* sender, QString title, Pegasus::PropertyGrid::IPropertyGridObjectProxy* proxy)
{
    ED_ASSERT(proxy != nullptr);
    
    PropertyGridHandle handle  = INVALID_PGRID_HANDLE;
    if (proxy->GetUserData() == nullptr)
    {

        handle = mNextHandle++;        
        mActiveProperties.insert(handle, proxy);
        mObservers.insert(handle, QSet<PropertyGridObserver*>());
        PropertyGridIOMCUpdateCache newUpdateCache;
        newUpdateCache.mHandle = handle;
        PropertyGridIOMessageController::ProxyUpdateCache::iterator it = mUpdateCache.insert(handle, newUpdateCache);
        proxy->SetEventListener(this);
        ED_ASSERT(proxy->GetUserData() == nullptr);
        //TODO: setup title
        proxy->SetUserData(new PropertyUserData(proxy, &(it.value()), title, handle));
    }
    else
    {
        PropertyUserData* u = static_cast<PropertyUserData*>(proxy->GetUserData());
        handle = u->GetHandle();
    }

    ED_ASSERT(mObservers.find(handle) != mObservers.end());
    QSet<PropertyGridObserver*>& observerList = mObservers.find(handle).value();
    observerList.insert(sender);

    emit sender->OnInitializedSignal(handle, title, proxy);

    //now notify the observers to update the uis looking at this object
    UpdateObserverInternal(sender, handle, proxy);
}

//----------------------------------------------------------------------------------------

void PropertyGridIOMessageController::OnRenderThreadOpenFromTimelineBlock(PropertyGridObserver* sender, QString title, AssetInstanceHandle timelineHandle, unsigned blockGuid)
{
    Pegasus::AssetLib::IRuntimeAssetObjectProxy* runtimeObject = mAssetMessageController->FindInstance(timelineHandle);
    if (runtimeObject != nullptr && runtimeObject->GetOwnerAsset() != nullptr && runtimeObject->GetOwnerAsset()->GetTypeDesc()->mTypeGuid == Pegasus::ASSET_TYPE_TIMELINE.mTypeGuid)
    {
        QString newTitle = runtimeObject->GetOwnerAsset()->GetTypeDesc()->mTypeName;

        Pegasus::Timeline::ITimelineProxy* timelineProxy = static_cast<Pegasus::Timeline::ITimelineProxy*>(runtimeObject);
        Pegasus::Timeline::IBlockProxy* blockProxy = timelineProxy->FindBlockByGuid(blockGuid);
        if (blockProxy != nullptr)
        {
            Pegasus::PropertyGrid::IPropertyGridObjectProxy* propertyGrid = blockProxy->GetPropertyGridProxy();
            if (propertyGrid != nullptr)
            {
                OnRenderThreadOpen(sender, newTitle, propertyGrid);
            }
        }
    }
}

//----------------------------------------------------------------------------------------

void PropertyGridIOMessageController::OnRenderThreadOpenFromAsset(PropertyGridObserver* sender, AssetInstanceHandle handle)
{
    Pegasus::AssetLib::IRuntimeAssetObjectProxy* runtimeObject = mAssetMessageController->FindInstance(handle);
    if (runtimeObject != nullptr)
    {
        QString title;
        Pegasus::AssetLib::IAssetProxy* assetProxy = runtimeObject->GetOwnerAsset();
        if (assetProxy != nullptr && assetProxy->GetTypeDesc() != nullptr)
        {
            title = assetProxy->GetTypeDesc()->mTypeName;
        }

        Pegasus::PropertyGrid::IPropertyGridObjectProxy* propertyGrid = runtimeObject->GetPropertyGrid();
        if (propertyGrid != nullptr)
        {
            OnRenderThreadOpen(sender, title, propertyGrid);
        }
    }
}

//----------------------------------------------------------------------------------------

void PropertyGridIOMessageController::UpdateObserverInternal(PropertyGridObserver* sender, PropertyGridHandle handle, Pegasus::PropertyGrid::IPropertyGridObjectProxy* proxy)
{

    //! \todo Refactor to loop over the categories
    QVector<PropertyGridIOMCUpdateElement> updates;

    // Prepare list of updates for class properties
    for (unsigned int i = 0; i < proxy->GetNumClassProperties(); ++i)
    {
        const Pegasus::PropertyGrid::PropertyRecord& r = proxy->GetClassPropertyRecord(i);
        PropertyGridIOMCUpdateElement el;
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
        PropertyGridIOMCUpdateElement el;
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

//----------------------------------------------------------------------------------------

void PropertyGridIOMessageController::CloseHandleInternal(PropertyGridHandle handle)
{
    PropertyGridIOMessageController::HandleToProxyMap::iterator proxyIt = mActiveProperties.find(handle);
    if (proxyIt != mActiveProperties.end())
    {
        PropertyGridIOMessageController::ProxyUpdateCache::iterator cacheIt = mUpdateCache.find(handle);

        proxyIt.value()->SetEventListener(nullptr);
        delete proxyIt.value()->GetUserData();
        proxyIt.value()->SetUserData(nullptr);

            
        mActiveProperties.erase(proxyIt);
        mUpdateCache.erase(cacheIt);
    }
}

//----------------------------------------------------------------------------------------

void PropertyGridIOMessageController::OnRenderThreadClose(PropertyGridObserver* sender, PropertyGridHandle handle)
{
    ED_ASSERT(handle != INVALID_PGRID_HANDLE);

    ObserverMap::iterator it = mObservers.find(handle);
    if (it != mObservers.end())
    {
        QSet<PropertyGridObserver*>obsSet = it.value();
        QSet<PropertyGridObserver*>::iterator obsSetResult = obsSet.find(sender);
        ED_ASSERT(obsSetResult != obsSet.end());
        obsSet.erase(obsSetResult);
        
        if (obsSet.empty())
        {
            CloseHandleInternal(handle);
        }
    }
    
}

//----------------------------------------------------------------------------------------

void PropertyGridIOMessageController::OnEvent(Pegasus::Core::IEventUserData* userData, Pegasus::PropertyGrid::ValueChangedEventIndexed& e)
{   
    PropertyUserData* pUserData = static_cast<PropertyUserData*>(userData);
    const Pegasus::PropertyGrid::PropertyRecord & r = pUserData->GetProxy()->GetClassPropertyRecord(e.GetIndex());

    PropertyGridIOMCUpdateElement el;
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

//----------------------------------------------------------------------------------------

void PropertyGridIOMessageController::OnEvent(Pegasus::Core::IEventUserData* userData, Pegasus::PropertyGrid::PropertyGridRenderRequest& e)
{
    //request a new frame
    emit RequestRedraw();
}

//----------------------------------------------------------------------------------------

void PropertyGridIOMessageController::FlushPendingUpdates(PropertyUserData* userData)
{
    PropertyGridIOMCUpdateCache* cache  = userData->GetUpdateCache();
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

//----------------------------------------------------------------------------------------

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
            QString title = static_cast<PropertyUserData*>(proxy->GetUserData())->GetTitle();
            if (it != mObservers.end())
            {
                foreach(PropertyGridObserver* obs, it.value())
                {
                    emit (obs->OnShutdownSignal(h));
                    emit(obs->OnInitializedSignal(h, title, proxy));
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

//----------------------------------------------------------------------------------------

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

//----------------------------------------------------------------------------------------

void PropertyGridIOMessageController::OnEvent(Pegasus::Core::IEventUserData* userData, Pegasus::PropertyGrid::ObjectPropertiesLayoutChanged& e)
{
    //accumulate all the layouts to reset for this particular property grid handle.
    PropertyUserData* pUserData = static_cast<PropertyUserData*>(userData);
    Pegasus::PropertyGrid::IPropertyGridObjectProxy* proxy = pUserData->GetProxy();
    PropertyGridHandle handle = pUserData->GetHandle();
    ED_ASSERT(handle != INVALID_PGRID_HANDLE);
    mLayoutsToReset.insert(handle);
}

//----------------------------------------------------------------------------------------

PropertyGridObserver::PropertyGridObserver()
{
    connect(this, SIGNAL(OnInitializedSignal(PropertyGridHandle, QString, const Pegasus::PropertyGrid::IPropertyGridObjectProxy*)),
            this, SLOT(OnInitializedSlot(PropertyGridHandle, QString, const Pegasus::PropertyGrid::IPropertyGridObjectProxy*)),
            Qt::QueuedConnection);

    connect(this, SIGNAL(OnUpdatedSignal(PropertyGridHandle, QVector<PropertyGridIOMCUpdateElement>)),
            this, SLOT(OnUpdatedSlot(PropertyGridHandle, QVector<PropertyGridIOMCUpdateElement>)),
            Qt::QueuedConnection);

    connect(this, SIGNAL(OnShutdownSignal(PropertyGridHandle)),
            this, SLOT(OnShutdownSlot(PropertyGridHandle)),
            Qt::QueuedConnection);
}

