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

//! Qt handle for property grid proxy. Mimics an integer.
//! Cant be an int typedef, because Qt won't be able to pass it around through signals
struct PropertyGridHandle
{
    explicit PropertyGridHandle(int v) { mValue = v; }

    PropertyGridHandle() : mValue(-1) {}

    bool operator == (const PropertyGridHandle& other) const { return other.mValue == mValue;}

    bool operator != (const PropertyGridHandle& other) const { return !(other == *this); }

    PropertyGridHandle operator++(int) { return PropertyGridHandle(mValue++); }

    const PropertyGridHandle& operator = (const PropertyGridHandle& other) { mValue = other.mValue; return *this; }

    bool operator < (const PropertyGridHandle& other) const { return mValue < other.mValue; }

private:
    int mValue;
};

// invalid handle declaration
const PropertyGridHandle INVALID_PGRID_HANDLE(-1);

//! The message controller
class PropertyGridIOMessageController : public QObject, private Pegasus::PropertyGrid::IPropertyListener
{

    Q_OBJECT

public:

    //! Structure with the update of an element
    struct UpdateElement
    {
        Pegasus::PropertyGrid::PropertyType mType; //! property type
        int mIndex; // the index position of this property
        union Data //Union representing the data it holds
        {   
            float         f;        //! float
            unsigned int  u;        //! unsigned
            int           i;        //! signed
            bool          b;        //! bool
            char          s64[64];  //! string64
            unsigned char rgba8[4]; //! rgb and rgba
            float         v[4];     //! float2, float3 and float4
        } mData;
    };


    // Accumulation of object data
    struct UpdateCache
    {
        PropertyGridHandle mHandle;
        QVector<PropertyGridIOMessageController::UpdateElement> mUpdateCache;
        UpdateCache() : mHandle(INVALID_PGRID_HANDLE) {}
    };


    //! User data attached to every property grid object
    class PropertyUserData : public Pegasus::Core::IEventUserData
    {
    public:
        PropertyUserData(
                Pegasus::PropertyGrid::IPropertyGridObjectProxy* proxy, 
                UpdateCache* updateCache,
                PropertyGridHandle handle
        )
             : mProxy(proxy), mUpdateCache(updateCache), mHandle(handle) {}; 
        virtual ~PropertyUserData() {};

        Pegasus::PropertyGrid::IPropertyGridObjectProxy* GetProxy() const { return mProxy; }
        UpdateCache* GetUpdateCache() const { return mUpdateCache; }
        PropertyGridHandle GetHandle() const { return mHandle; }
        
    private:
        Pegasus::PropertyGrid::IPropertyGridObjectProxy* mProxy;
        UpdateCache* mUpdateCache;
        PropertyGridHandle mHandle;
        
    };
    
    //! Message container class so ui can communicate with render thread
    class Message
    {
    public:
        enum MessageType
        {
            OPEN, //! when a new property grid is desired to be open
            CLOSE, //! when a widget will not view a property grid any more
            UPDATE, //! when the widget request a change in the state of a property grid
            INVALID
        };
        
        //! Constructors
        explicit Message(MessageType t) : mMessageType(t), mPropGrid(nullptr), mPropGridHandle(-1), mObserver(nullptr) {}
        Message() : mMessageType(INVALID), mPropGrid(nullptr), mPropGridHandle(-1), mObserver(nullptr) {}
    
        //! Getters
        MessageType GetMessageType() const { return mMessageType; }
        QVector<UpdateElement>& GetUpdateBatch() { return mUpdates; }
        const QVector<UpdateElement>& GetUpdateBatch() const { return mUpdates; }
        PropertyGridHandle GetPropertyGridHandle() const { return mPropGridHandle; }
        Pegasus::PropertyGrid::IPropertyGridObjectProxy* GetPropertyGrid() const { return mPropGrid; }
        PropertyGridObserver* GetPropertyGridObserver() const { return mObserver; }

        //! Setters
        void SetPropertyGridHandle(PropertyGridHandle handle) { mPropGridHandle = handle; }
        void SetPropertyGrid(Pegasus::PropertyGrid::IPropertyGridObjectProxy* proxy) { mPropGrid  = proxy; }
        void SetPropertyGridObserver(PropertyGridObserver* observer) { mObserver = observer; }
        void SetMessageType(MessageType t) { mMessageType = t; }
        

    private:
        MessageType mMessageType;
        QVector<UpdateElement> mUpdates;
        PropertyGridHandle mPropGridHandle;
        Pegasus::PropertyGrid::IPropertyGridObjectProxy* mPropGrid;
        PropertyGridObserver* mObserver;
    };
    
    //! Constructor
    explicit PropertyGridIOMessageController(Pegasus::App::IApplicationProxy* app);

    //! Destructor
    virtual ~PropertyGridIOMessageController();

    //! Called by render thread, when a render thread message should be processed.
    //! \param m - the message to get processed
    void OnRenderThreadProcessMessage(const Message& m);

    //! Call at the end of the frame, whenever it is ideal to flush all the properties.
    void FlushAllPendingUpdates();

private:

    void OnRenderThreadUpdate(PropertyGridObserver* sender, PropertyGridHandle handle, const QVector<UpdateElement>& elements);
    void OnRenderThreadOpen(PropertyGridObserver* sender, Pegasus::PropertyGrid::IPropertyGridObjectProxy* proxy);
    void OnRenderThreadClose(PropertyGridObserver* sender, PropertyGridHandle handle);

    void CloseHandleInternal(PropertyGridHandle handle);

    void FlushPendingUpdates(PropertyUserData* userData);

    
    // Property grid event listener callbacks (from app to ui)
    virtual void OnEvent(Pegasus::Core::IEventUserData* userData, Pegasus::PropertyGrid::ValueChangedEventIndexed& e);
    virtual void OnEvent(Pegasus::Core::IEventUserData* userData, Pegasus::PropertyGrid::PropertyGridDestroyed& e);
    //

    Pegasus::App::IApplicationProxy* mApp;

    typedef QSet<PropertyGridObserver*> ObserverSet;
    typedef QMap< PropertyGridHandle, ObserverSet > ObserverMap;
    typedef QMap< PropertyGridHandle, Pegasus::PropertyGrid::IPropertyGridObjectProxy* > HandleToProxyMap;    
    typedef QMap< PropertyGridHandle, UpdateCache> ProxyUpdateCache;
    ObserverMap mObservers;
    HandleToProxyMap mActiveProperties;
    PropertyGridHandle mNextHandle;
    ProxyUpdateCache mUpdateCache;
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
    virtual void OnInitialized(PropertyGridHandle handle, const Pegasus::PropertyGrid::IPropertyGridObjectProxy* objectProxy) = 0;

    //! Callback when an external (the render thread) process has edited a property.
    //! \param handle - the handle that has been updated. This parameter will always be the same one that was returned by OnInitialized.
    //! \note - use the handle for redundancy in the code
    //! \param els - batch of updates to propagate to the UI
    virtual void OnUpdated(PropertyGridHandle handle, const QVector<PropertyGridIOMessageController::UpdateElement>& els) = 0;

    //! Calls when the handle has been closed, or the proxy object has been destroyed and no longer exists
    virtual void OnShutdown(PropertyGridHandle handle) = 0;

signals:
    void OnInitializedSignal(PropertyGridHandle handle, const Pegasus::PropertyGrid::IPropertyGridObjectProxy* objectProxy);

    void OnUpdatedSignal(PropertyGridHandle handle, const QVector<PropertyGridIOMessageController::UpdateElement> els);

    void OnShutdownSignal(PropertyGridHandle handle);

private slots:
    void OnInitializedSlot(PropertyGridHandle handle, const Pegasus::PropertyGrid::IPropertyGridObjectProxy* objectProxy) { OnInitialized(handle, objectProxy); }

    void OnUpdatedSlot(PropertyGridHandle handle, const QVector<PropertyGridIOMessageController::UpdateElement> els) { OnUpdated(handle, els); }

    void OnShutdownSlot(PropertyGridHandle handle) { OnShutdown(handle); }
    
};

#endif


