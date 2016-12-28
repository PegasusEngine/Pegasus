/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	PropertyGridIOMessageController.h
//! \author	Kleber Garcia
//! \date	November 2nd 2015
//! \brief	PropertyGrid IO controller - thread safe, communicates with application and sets
//!         correct state for application property grid

#ifndef PEGASUS_PROPERTY_GRID_IO_MSG_CONTROLLER_H
#define PEGASUS_PROPERTY_GRID_IO_MSG_CONTROLLER_H

#include <QObject>
#include <QMap>
#include <QSet>
#include "PropertyTypes.h"
#include "Pegasus/Preprocessor.h"
#include "Pegasus/PropertyGrid/Shared/PropertyDefs.h"
#include "Pegasus/PropertyGrid/Shared/PropertyEventDefs.h"
#include "MessageControllers/AssetIOMessageController.h"
#include "MessageControllers/MsgDefines.h"


// Forward declaration
namespace Pegasus
{
    namespace PropertyGrid {
        class IPropertyGridObjectProxy;
    }

    namespace App {
        class IApplicationProxy;
    }
}

class PropertyGridObserver;

//! The message controller
class PropertyGridIOMessageController : public QObject, private Pegasus::PropertyGrid::IPropertyListener
{

    Q_OBJECT

public:


    //! User data attached to every property grid object
    class PropertyUserData : public Pegasus::Core::IEventUserData
    {
    public:
        PropertyUserData(
                Pegasus::PropertyGrid::IPropertyGridObjectProxy* proxy, 
                PropertyGridIOMCUpdateCache* updateCache,
                QString title,
                PropertyGridHandle handle
        )
             : mProxy(proxy), mTitle(title), mUpdateCache(updateCache), mHandle(handle) {}; 
        virtual ~PropertyUserData() {};

        Pegasus::PropertyGrid::IPropertyGridObjectProxy* GetProxy() const { return mProxy; }
        PropertyGridIOMCUpdateCache* GetUpdateCache() const { return mUpdateCache; }
        PropertyGridHandle GetHandle() const { return mHandle; }
        const QString& GetTitle() const { return mTitle; }
   
        
    private:
        Pegasus::PropertyGrid::IPropertyGridObjectProxy* mProxy;
        PropertyGridIOMCUpdateCache* mUpdateCache;
        PropertyGridHandle mHandle;
        QString mTitle;
        
    };
    
    //! Constructor
    explicit PropertyGridIOMessageController(Pegasus::App::IApplicationProxy* app, AssetIOMessageController* assetMessageController);

    //! Destructor
    virtual ~PropertyGridIOMessageController();

    //! Called by render thread, when a render thread message should be processed.
    //! \param m - the message to get processed
    void OnRenderThreadProcessMessage(const PropertyGridIOMCMessage& m);

    //! Call at the end of the frame, whenever it is ideal to flush all the properties.
    void FlushAllPendingUpdates();

signals:
    //signal triggered when a redraw is requested.
    void RequestRedraw();

private:

    void OnRenderThreadUpdate(PropertyGridObserver* sender, PropertyGridHandle handle, const QVector<PropertyGridIOMCUpdateElement>& elements);
    void OnRenderThreadOpen(PropertyGridObserver* sender, QString title, Pegasus::PropertyGrid::IPropertyGridObjectProxy* proxy);
    void OnRenderThreadOpenFromTimelineBlock(PropertyGridObserver* sender, QString title, AssetInstanceHandle timelineHandle, unsigned blockId);
    void OnRenderThreadOpenFromAsset(PropertyGridObserver* sender, AssetInstanceHandle handle);
    void OnRenderThreadClose(PropertyGridObserver* sender, PropertyGridHandle handle);

    void UpdateObserverInternal(PropertyGridObserver* sender, PropertyGridHandle handle, Pegasus::PropertyGrid::IPropertyGridObjectProxy* proxy);
    void CloseHandleInternal(PropertyGridHandle handle);

    void FlushPendingUpdates(PropertyUserData* userData);

    
    // Property grid event listener callbacks (from app to ui)
    virtual void OnEvent(Pegasus::Core::IEventUserData* userData, Pegasus::PropertyGrid::ValueChangedEventIndexed& e);
    virtual void OnEvent(Pegasus::Core::IEventUserData* userData, Pegasus::PropertyGrid::ObjectPropertiesLayoutChanged& e);
    virtual void OnEvent(Pegasus::Core::IEventUserData* userData, Pegasus::PropertyGrid::PropertyGridDestroyed& e);
    virtual void OnEvent(Pegasus::Core::IEventUserData* userData, Pegasus::PropertyGrid::PropertyGridRenderRequest& e);
    //

    Pegasus::App::IApplicationProxy* mApp;

    typedef QSet<PropertyGridObserver*> ObserverSet;
    typedef QSet< PropertyGridHandle> HandleSet;
    typedef QMap< PropertyGridHandle, ObserverSet > ObserverMap;
    typedef QMap< PropertyGridHandle, Pegasus::PropertyGrid::IPropertyGridObjectProxy* > HandleToProxyMap;    
    typedef QMap< PropertyGridHandle, PropertyGridIOMCUpdateCache> ProxyUpdateCache;
    ObserverMap mObservers;
    HandleToProxyMap mActiveProperties;
    PropertyGridHandle mNextHandle;
    ProxyUpdateCache mUpdateCache;
    HandleSet        mLayoutsToReset;

    //reference to asset io controller
    AssetIOMessageController* mAssetMessageController;
};


//! Class representing a property grid modifier callback. To be used when:
//! We want to update some view after a property has been updated either by the UI or the render app
class PropertyGridObserver  : public QObject
{
    friend class PropertyGridIOMessageController;

    Q_OBJECT

public:

    //! Constructor
    PropertyGridObserver();
    
    //! Destructor
    virtual ~PropertyGridObserver() {}

    //! Callback when this observer gets notified that the IO controller is done initializing
    //! \param handle The new handle. Store this handle locally in this observer for this session.
    //! \param objectProxy Proxy for the observed property grid
    virtual void OnInitialized(PropertyGridHandle handle, QString title, const Pegasus::PropertyGrid::IPropertyGridObjectProxy* objectProxy) = 0;

    //! Callback when an external (the render thread) process has edited a property.
    //! \param handle - the handle that has been updated. This parameter will always be the same one that was returned by OnInitialized.
    //! \note - use the handle for redundancy in the code
    //! \param els - batch of updates to propagate to the UI
    virtual void OnUpdated(PropertyGridHandle handle, const QVector<PropertyGridIOMCUpdateElement>& els) = 0;

    //! Calls when the handle has been closed, or the proxy object has been destroyed and no longer exists
    virtual void OnShutdown(PropertyGridHandle handle) = 0;

signals:
    void OnInitializedSignal(PropertyGridHandle handle, QString title, const Pegasus::PropertyGrid::IPropertyGridObjectProxy* objectProxy);

    void OnUpdatedSignal(PropertyGridHandle handle, const QVector<PropertyGridIOMCUpdateElement> els);

    void OnShutdownSignal(PropertyGridHandle handle);

private slots:
    void OnInitializedSlot(PropertyGridHandle handle, QString title, const Pegasus::PropertyGrid::IPropertyGridObjectProxy* objectProxy) { OnInitialized(handle, title, objectProxy); }

    void OnUpdatedSlot(PropertyGridHandle handle, const QVector<PropertyGridIOMCUpdateElement> els) { OnUpdated(handle, els); }

    void OnShutdownSlot(PropertyGridHandle handle) { OnShutdown(handle); }
    
};

#endif


