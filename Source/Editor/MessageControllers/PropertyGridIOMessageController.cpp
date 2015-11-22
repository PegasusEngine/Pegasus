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
        int propSizes = pgrid->GetNumClassProperties();
        for (int i = 0; i < elements.size(); ++i)
        {
            if (elements[i].mIndex < propSizes)
            {
                const Pegasus::PropertyGrid::PropertyRecord& r = pgrid->GetClassPropertyRecord(elements[i].mIndex);
                if (r.type == elements[i].mType)
                {
                    pgrid->WriteClassProperty(elements[i].mIndex, &elements[i].mData, r.size);
                }
                else
                {
                    ED_FAILSTR("Failed reading property type.");
                }
            }
            else
            {
                ED_FAILSTR("Failed setting property.");
            }
        }
    }
}

void PropertyGridIOMessageController::OnRenderThreadOpen(PropertyGridObserver* sender, Pegasus::PropertyGrid::IPropertyGridObjectProxy* proxy)
{
    ED_ASSERT(proxy != nullptr);
    PropertyGridIOMessageController::ProxyToHandleMap::iterator handleToProxy = mProxyToHandle.find(proxy);
    PropertyGridHandle handle  = INVALID_PGRID_HANDLE;
    if (handleToProxy == mProxyToHandle.end())
    {

        handle = mNextHandle++;
        mProxyToHandle.insert(proxy, handle);
        mActiveProperties.insert(handle, proxy);
        mObservers.insert(handle, QSet<PropertyGridObserver*>());
    }
    else
    {
        handle = handleToProxy.value();
    }

    ED_ASSERT(mObservers.find(handle) != mObservers.end());
    QSet<PropertyGridObserver*>& observerList = mObservers.find(handle).value();
    observerList.insert(sender);

    emit sender->OnInitializedSignal(handle, proxy);

    //prepare list of updates
    QVector<PropertyGridIOMessageController::UpdateElement> updates;
    for (unsigned i = 0; i < proxy->GetNumClassProperties(); ++i)
    {
        const Pegasus::PropertyGrid::PropertyRecord& r = proxy->GetClassPropertyRecord(i);
        PropertyGridIOMessageController::UpdateElement el;
        el.mType = r.type;
        el.mIndex = (int)i;
        if (r.size  <= sizeof(el.mData))
        {
            proxy->ReadClassProperty(i, &el.mData, r.size);
            updates.push_back(el);
        }
        else
        {
            ED_FAILSTR("Data container for ui cannot fit current property!");
        }
    }

    //send signal to UI so it can update the view
    emit sender->OnUpdatedSignal(handle, updates);
}

void PropertyGridIOMessageController::OnRenderThreadClose(PropertyGridObserver* sender, PropertyGridHandle handle)
{
    ED_ASSERT(handle != INVALID_PGRID_HANDLE);
    PropertyGridIOMessageController::HandleToProxyMap::iterator proxyIt = mActiveProperties.find(handle);
    PropertyGridIOMessageController::ProxyToHandleMap::iterator handleIt = mProxyToHandle.find(proxyIt.value());
    
    QSet<PropertyGridObserver*>obsSet = mObservers.find(handle).value();
    obsSet.erase(obsSet.find(sender));
    
    if (obsSet.empty())
    {
        mActiveProperties.erase(proxyIt);
        mProxyToHandle.erase(handleIt);
    }
    
    emit sender->OnShutdownSlot(handle);
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

