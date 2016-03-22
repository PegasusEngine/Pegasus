/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	GraphIOMessageController.cpp
//! \author	Karolyn Boulanger
//! \date	December 25th 2015
//! \brief	Graph IO controller, communicates events from the graph editor
//!         in a thread-safe way

#include "MessageControllers/GraphIOMessageController.h"

//#include "Pegasus/Application/Shared/IApplicationProxy.h"
//#include "Pegasus/Preprocessor.h"
#include "Pegasus/Texture/Shared/ITextureNodeProxy.h"


GraphIOMessageController::GraphIOMessageController(Pegasus::App::IApplicationProxy* appProxy)
:   mAppProxy(appProxy)
,   mNextHandle(0)
{
}

//----------------------------------------------------------------------------------------

GraphIOMessageController::~GraphIOMessageController()
{
}

//----------------------------------------------------------------------------------------

void GraphIOMessageController::OnRenderThreadProcessMessage(const GraphIOMessageController::Message& m)
{
    switch(m.GetMessageType())
    {
        case GraphIOMessageController::Message::OPEN:
            OnRenderThreadOpen(m.GetTextureNodeObserver(), m.GetTextureNodeProxy());
            break;

        case GraphIOMessageController::Message::CLOSE:
            //****OnRenderThreadClose(m.GetGraphObserver(), m.GetGraphHandle());
            break;

        //case GraphIOMessageController::Message::UPDATE:
        //    //****OnRenderThreadUpdate(m.GetGraphObserver(), m.GetGraphHandle(), m.GetUpdateBatch());
        //    break;

        default:
            ED_FAILSTR("Invalid message type (%u) for the graph IO message controller", m.GetMessageType());
    }
}

//----------------------------------------------------------------------------------------

void GraphIOMessageController::FlushAllPendingUpdates()
{
    //// first see if any object requested a layout change
    //if (!mLayoutsToReset.empty())
    //{
    //    foreach (PropertyGridHandle h, mLayoutsToReset)
    //    {
    //        ObserverMap::iterator it = mObservers.find(h);
    //        HandleToProxyMap::iterator h2pit = mActiveProperties.find(h);
    //        if (h2pit == mActiveProperties.end()) continue;
    //        Pegasus::PropertyGrid::IPropertyGridObjectProxy* proxy = h2pit.value();
    //        if (it != mObservers.end())
    //        {
    //            foreach(PropertyGridObserver* obs, it.value())
    //            {
    //                emit (obs->OnShutdownSignal(h));
    //                emit(obs->OnInitializedSignal(h, proxy));
    //                UpdateObserverInternal(obs, h, proxy);
    //            }
    //        }
    //    }
    //    mLayoutsToReset.clear();
    //}

    //HandleToProxyMap::iterator it = mActiveProperties.begin();
    //for (; it != mActiveProperties.end(); ++it)
    //{
    //    PropertyUserData* userData = static_cast<PropertyUserData*>(it.value()->GetUserData());
    //    FlushPendingUpdates(userData);
    //}
}

//----------------------------------------------------------------------------------------

void GraphIOMessageController::OnRenderThreadOpen(TextureNodeObserver* sender, Pegasus::Texture::ITextureNodeProxy * proxy)
{
    ED_ASSERT(proxy != nullptr);
    
    TextureNodeProxyHandle handle = INVALID_TEXTURE_NODE_PROXY_HANDLE;
    if (proxy->GetUserData() == nullptr)
    {
        handle = mNextHandle++;
        mActiveTextures.insert(handle, proxy);
        mObservers.insert(handle, QSet<TextureNodeObserver*>());
        UpdateCache newUpdateCache;
        newUpdateCache.mHandle = handle;
        ProxyUpdateCache::iterator it = mUpdateCache.insert(handle, newUpdateCache);
        proxy->SetEventListener(this);
        ED_ASSERT(proxy->GetUserData() == nullptr);
        proxy->SetUserData(new TextureNodeUserData(proxy, &(it.value()), handle));
    }
    else
    {
        TextureNodeUserData* userData = static_cast<TextureNodeUserData*>(proxy->GetUserData());
        handle = userData->GetHandle();
    }

    ED_ASSERT(mObservers.find(handle) != mObservers.end());
    QSet<TextureNodeObserver*>& observerList = mObservers.find(handle).value();
    observerList.insert(sender);

    emit sender->OnInitializedSignal(handle, proxy);

    // Notify the observers to update the user interfaces looking at this object
    UpdateObserverInternal(sender, handle, proxy);
}

////----------------------------------------------------------------------------------------
//
//void PropertyGridIOMessageController::OnRenderThreadUpdate(PropertyGridObserver* sender, PropertyGridHandle handle, const QVector<PropertyGridIOMessageController::UpdateElement>& elements)
//{
//    ED_ASSERT(handle != INVALID_PGRID_HANDLE);
//    PropertyGridIOMessageController::HandleToProxyMap::iterator it = mActiveProperties.find(handle);
//    ED_ASSERT(it != mActiveProperties.end());
//    if(it != mActiveProperties.end())
//    {
//        Pegasus::PropertyGrid::IPropertyGridObjectProxy* pgrid = it.value();
//        for (int i = 0; i < elements.size(); ++i)
//        {
//            const PropertyGridIOMessageController::UpdateElement & el = elements[i];
//            switch (el.mCategory)
//            {
//                case Pegasus::PropertyGrid::PROPERTYCATEGORY_CLASS:
//                    {
//                        int propSizes = pgrid->GetNumClassProperties();
//                        if (elements[i].mIndex < propSizes)
//                        {
//                            const Pegasus::PropertyGrid::PropertyRecord& r = pgrid->GetClassPropertyRecord(elements[i].mIndex);
//                            if (r.type == elements[i].mType)
//                            {
//                                pgrid->WriteClassProperty(elements[i].mIndex, &elements[i].mData, r.size, /*don't send message*/ false);
//                            }
//                            else
//                            {
//                                ED_FAILSTR("Failed reading class property type.");
//                            }
//                        }
//                        else
//                        {
//                            ED_FAILSTR("Failed setting class property.");
//                        }
//                    }
//                    break;
//
//                case Pegasus::PropertyGrid::PROPERTYCATEGORY_OBJECT:
//                    {
//                        int propSizes = pgrid->GetNumObjectProperties();
//                        if (elements[i].mIndex < propSizes)
//                        {
//                            const Pegasus::PropertyGrid::PropertyRecord& r = pgrid->GetObjectPropertyRecord(elements[i].mIndex);
//                            if (r.type == elements[i].mType)
//                            {
//                                pgrid->WriteObjectProperty(elements[i].mIndex, &elements[i].mData, r.size, /*don't send message*/ false);
//                            }
//                            else
//                            {
//                                ED_FAILSTR("Failed reading object property type.");
//                            }
//                        }
//                        else
//                        {
//                            ED_FAILSTR("Failed setting object property.");
//                        }
//                    }
//                    break;
//
//                default:
//                    ED_FAILSTR("Invalid category for a property");
//                    break;
//            }
//        }
//    }
//}
//
//----------------------------------------------------------------------------------------
//
//void PropertyGridIOMessageController::OnRenderThreadClose(PropertyGridObserver* sender, PropertyGridHandle handle)
//{
//    ED_ASSERT(handle != INVALID_PGRID_HANDLE);
//
//    QSet<PropertyGridObserver*>obsSet = mObservers.find(handle).value();
//    obsSet.erase(obsSet.find(sender));
//    
//    if (obsSet.empty())
//    {
//        CloseHandleInternal(handle);
//    }
//    
//}
//
//----------------------------------------------------------------------------------------

void GraphIOMessageController::UpdateObserverInternal(TextureNodeObserver* sender, TextureNodeProxyHandle handle, Pegasus::Texture::ITextureNodeProxy* proxy)
{
//    //! \todo Refactor to loop over the categories
//    QVector<PropertyGridIOMessageController::UpdateElement> updates;
//
//    // Prepare list of updates for class properties
//    for (unsigned int i = 0; i < proxy->GetNumClassProperties(); ++i)
//    {
//        const Pegasus::PropertyGrid::PropertyRecord& r = proxy->GetClassPropertyRecord(i);
//        PropertyGridIOMessageController::UpdateElement el;
//        el.mCategory = Pegasus::PropertyGrid::PROPERTYCATEGORY_CLASS;
//        el.mType = r.type;
//        el.mIndex = (int)i;
//        if (r.size  <= sizeof(el.mData))
//        {
//            proxy->ReadClassProperty(i, &el.mData, r.size);
//            updates.push_back(el);
//        }
//        else
//        {
//            ED_FAILSTR("Data container for ui cannot fit current class property!");
//        }
//    }
//
//    // Prepare list of updates for object properties
//    for (unsigned int i = 0; i < proxy->GetNumObjectProperties(); ++i)
//    {
//        const Pegasus::PropertyGrid::PropertyRecord& r = proxy->GetObjectPropertyRecord(i);
//        PropertyGridIOMessageController::UpdateElement el;
//        el.mCategory = Pegasus::PropertyGrid::PROPERTYCATEGORY_OBJECT;
//        el.mType = r.type;
//        el.mIndex = (int)i;
//        if (r.size  <= sizeof(el.mData))
//        {
//            proxy->ReadObjectProperty(i, &el.mData, r.size);
//            updates.push_back(el);
//        }
//        else
//        {
//            ED_FAILSTR("Data container for ui cannot fit current object property!");
//        }
//    }
//
//    //send signal to UI so it can update the view
//    emit sender->OnUpdatedSignal(handle, updates);
}

////----------------------------------------------------------------------------------------
//
//void PropertyGridIOMessageController::CloseHandleInternal(PropertyGridHandle handle)
//{
//    PropertyGridIOMessageController::HandleToProxyMap::iterator proxyIt = mActiveProperties.find(handle);
//    PropertyGridIOMessageController::ProxyUpdateCache::iterator cacheIt = mUpdateCache.find(handle);
//
//    proxyIt.value()->SetEventListener(nullptr);
//    delete proxyIt.value()->GetUserData();
//    proxyIt.value()->SetUserData(nullptr);
//
//        
//    mActiveProperties.erase(proxyIt);
//    mUpdateCache.erase(cacheIt);
//}

//----------------------------------------------------------------------------------------

void GraphIOMessageController::OnEvent(Pegasus::Core::IEventUserData* userData, Pegasus::Texture::TextureNodeNotificationEvent& e)
{   
    TextureNodeUserData* textureUserData = static_cast<TextureNodeUserData*>(userData);
//    const Pegasus::PropertyGrid::PropertyRecord & r = textureUserData->GetProxy()->GetClassPropertyRecord(e.GetIndex());
//
//    PropertyGridIOMessageController::UpdateElement el;
//    el.mCategory = e.GetCategory();
//    el.mIndex = e.GetIndex();
//    el.mType = r.type;
//    if (el.mIndex > 0 && r.size <= sizeof(el.mData))
//    {
//        textureUserData->GetProxy()->ReadClassProperty(el.mIndex, &el.mData, r.size);
//    }
//    else
//    {
//        ED_FAIL();
//    }
//    textureUserData->GetUpdateCache()->mUpdateCache.push_back(el);
//
//    const int MAX_CACHE_SIZE = 20;
//    if (textureUserData->GetUpdateCache()->mUpdateCache.size() >= MAX_CACHE_SIZE)
//    {
//        FlushPendingUpdates(pUserData);
//    }
}

//----------------------------------------------------------------------------------------

void GraphIOMessageController::OnEvent(Pegasus::Core::IEventUserData* userData, Pegasus::Texture::TextureNodeGenerationEvent& e)
{
//    //request a new frame
//    emit RequestRedraw();
}

//----------------------------------------------------------------------------------------

void GraphIOMessageController::OnEvent(Pegasus::Core::IEventUserData* userData, Pegasus::Texture::TextureNodeOperationEvent& e)
{
    //UpdateCache* cache = userData->GetUpdateCache();
    //ObserverMap::iterator obsIt = mObservers.find(userData->GetHandle());
    //if (obsIt != mObservers.end())
    //{
    //    ObserverSet& observerSet = obsIt.value();
//        foreach (PropertyGridObserver* obs, observerSet)
//        {
//            emit obs->OnUpdatedSignal(userData->GetHandle(), userData->GetUpdateCache()->mUpdateCache);
//        }
//    }
//    cache->mUpdateCache.clear();
}

////----------------------------------------------------------------------------------------
//
//void PropertyGridIOMessageController::OnEvent(Pegasus::Core::IEventUserData* userData, Pegasus::PropertyGrid::PropertyGridDestroyed& e)
//{
//    PropertyUserData* pUserData = static_cast<PropertyUserData*>(userData);
//    Pegasus::PropertyGrid::IPropertyGridObjectProxy* proxy = pUserData->GetProxy();
//    PropertyGridHandle handle = pUserData->GetHandle();
//    
//    PropertyGridIOMessageController::ObserverMap::iterator obsIt = mObservers.find(handle);
//    if (obsIt != mObservers.end())
//    {
//        PropertyGridIOMessageController::ObserverSet& set = obsIt.value();
//        foreach(PropertyGridObserver* obs, set)
//        {
//            emit obs->OnShutdownSignal(handle);
//        }
//
//        mObservers.erase(obsIt);
//
//        CloseHandleInternal(handle);
//        
//    }
//    else
//    {
//        ED_FAIL();
//    }
//}
//
////----------------------------------------------------------------------------------------
//
//void PropertyGridIOMessageController::OnEvent(Pegasus::Core::IEventUserData* userData, Pegasus::PropertyGrid::ObjectPropertiesLayoutChanged& e)
//{
//    //accumulate all the layouts to reset for this particular property grid handle.
//    PropertyUserData* pUserData = static_cast<PropertyUserData*>(userData);
//    Pegasus::PropertyGrid::IPropertyGridObjectProxy* proxy = pUserData->GetProxy();
//    PropertyGridHandle handle = pUserData->GetHandle();
//    ED_ASSERT(handle != INVALID_PGRID_HANDLE);
//    mLayoutsToReset.insert(handle);
//}

//----------------------------------------------------------------------------------------

TextureNodeObserver::TextureNodeObserver()
{
    connect(this, SIGNAL(OnInitializedSignal(TextureNodeProxyHandle, const Pegasus::Texture::ITextureNodeProxy*)),
            this, SLOT(OnInitializedSlot(TextureNodeProxyHandle, const Pegasus::Texture::ITextureNodeProxy*)),
            Qt::QueuedConnection);

    connect(this, SIGNAL(OnUpdatedSignal(TextureNodeProxyHandle, QVector<GraphIOMessageController::UpdateElement>)),
            this, SLOT(OnUpdatedSlot(TextureNodeProxyHandle, QVector<GraphIOMessageController::UpdateElement>)),
            Qt::QueuedConnection);

    connect(this, SIGNAL(OnShutdownSignal(TextureNodeProxyHandle)),
            this, SLOT(OnShutdownSlot(TextureNodeProxyHandle)),
            Qt::QueuedConnection);
}
