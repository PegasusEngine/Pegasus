/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	GraphIOMessageController.h
//! \author	Karolyn Boulanger
//! \date	December 25th 2015
//! \brief	Graph IO controller, communicates events from the graph editor
//!         in a thread-safe way

#ifndef PEGASUS_GRAPHIOMESSAGECONTROLLER_H
#define PEGASUS_GRAPHIOMESSAGECONTROLLER_H

//#include "Pegasus/Application/Shared/IApplicationProxy.h"
//#include "PropertyTypes.h"
////#include "Pegasus/Preprocessor.h"
//#include "Pegasus/PropertyGrid/Shared/PropertyDefs.h"
#include "Pegasus/Texture/Shared/TextureEventDefs.h"
//#include <QObject>
#include <QMap>
#include <QSet>

namespace Pegasus
{
    namespace Texture {
        class ITextureNodeProxy;
    }

    namespace App {
        class IApplicationProxy;
    }
}

class TextureNodeObserver;


//! Qt handle for texture node proxy. Mimics an integer.
//! Cant be an int typedef, because Qt won't be able to pass it around through signals
//! \todo Wait, what? Investigate, as Qt can pass integers in messages
//! \todo Refactor to make it graph generic
struct TextureNodeProxyHandle
{
    explicit TextureNodeProxyHandle(int v) { mValue = v; }

    TextureNodeProxyHandle() : mValue(-1) {}

    bool operator == (const TextureNodeProxyHandle& other) const { return other.mValue == mValue;}

    bool operator != (const TextureNodeProxyHandle& other) const { return !(other == *this); }

    TextureNodeProxyHandle operator++(int) { return TextureNodeProxyHandle(mValue++); }

    const TextureNodeProxyHandle& operator = (const TextureNodeProxyHandle& other) { mValue = other.mValue; return *this; }

    bool operator < (const TextureNodeProxyHandle& other) const { return mValue < other.mValue; }

    int InternalValue() const { return mValue; }

private:
    int mValue;
};

//inline uint qHash(const PropertyGridHandle& h) 
//{
//    return qHash(h.InternalValue());
//}

// Invalid handle declaration
const TextureNodeProxyHandle INVALID_TEXTURE_NODE_PROXY_HANDLE(-1);

//----------------------------------------------------------------------------------------

//! Graph IO controller, communicates events from the graph editor in a thread-safe way
//! \todo Make it work on generic graphs, not just textures
class GraphIOMessageController : public QObject, private Pegasus::Texture::ITextureNodeEventListener
{
    Q_OBJECT

public:

    //! Structure with the update of an element
    struct UpdateElement
    {
        //Pegasus::PropertyGrid::PropertyCategory mCategory;
        //Pegasus::PropertyGrid::PropertyType mType; //! property type
        //int mIndex; // the index position of this property
        //union Data //Union representing the data it holds
        //{   
        //    float         f;        //! float
        //    unsigned int  u;        //! unsigned
        //    int           i;        //! signed, enum
        //    bool          b;        //! bool
        //    char          s64[64];  //! string64
        //    unsigned char rgba8[4]; //! rgb and rgba
        //    float         v[4];     //! float2, float3 and float4
        //} mData;

        char mNodeName[64];

    public:

        UpdateElement() /*:*/
            //mCategory(Pegasus::PropertyGrid::PROPERTYCATEGORY_INVALID),
            //mType(Pegasus::PropertyGrid::PROPERTYTYPE_INVALID),
            //mIndex(-1)
        {
            mNodeName[0] = '\0';
        }
    };


    // Accumulation of update elements
    struct UpdateCache
    {
        TextureNodeProxyHandle mHandle;
        QVector<UpdateElement> mUpdateCache;
        
        UpdateCache()
            :   mHandle(INVALID_TEXTURE_NODE_PROXY_HANDLE)
            { }
    };


    //! User data attached to every texture
    class TextureNodeUserData : public Pegasus::Core::IEventUserData
    {
    public:
        TextureNodeUserData(Pegasus::Texture::ITextureNodeProxy* proxy, 
                            UpdateCache* updateCache,
                            TextureNodeProxyHandle handle)
            :   mTextureNodeProxy(proxy), mUpdateCache(updateCache), mHandle(handle)
            { }

        virtual ~TextureNodeUserData() { }

        Pegasus::Texture::ITextureNodeProxy* GetProxy() const { return mTextureNodeProxy; }
        UpdateCache* GetUpdateCache() const { return mUpdateCache; }
        TextureNodeProxyHandle GetHandle() const { return mHandle; }

    private:
        Pegasus::Texture::ITextureNodeProxy* mTextureNodeProxy;
        UpdateCache* mUpdateCache;
        TextureNodeProxyHandle mHandle;
    };


    //! Message container class so the UI can communicate with the application render thread
    class Message
    {
    public:
        enum MessageType
        {
            OPEN,       //!< When a graph is being opened (shown in editor)
            CLOSE,      //!< When a graph is closed (removed from editor)
            //UPDATE,
            INVALID
        };
        

        //! Constructors
        explicit Message(MessageType type)
            : mMessageType(type), mTextureNodeProxy(nullptr), mTextureNodeProxyHandle(-1), mTextureNodeObserver(nullptr) { }
        
        Message()
            : mMessageType(INVALID), mTextureNodeProxy(nullptr), mTextureNodeProxyHandle(-1), mTextureNodeObserver(nullptr) { }
    

        // Getters
        inline MessageType GetMessageType() const { return mMessageType; }

        //QVector<UpdateElement>& GetUpdateBatch() { return mUpdates; }
        //const QVector<UpdateElement>& GetUpdateBatch() const { return mUpdates; }
        TextureNodeProxyHandle GetTextureNodeProxyHandle() const { return mTextureNodeProxyHandle; }
        Pegasus::Texture::ITextureNodeProxy* GetTextureNodeProxy() const { return mTextureNodeProxy; }
        TextureNodeObserver* GetTextureNodeObserver() const { return mTextureNodeObserver; }

        // Setters
        void SetTextureNodeProxyHandle(TextureNodeProxyHandle handle) { mTextureNodeProxyHandle = handle; }
        void SetTextureNodeProxy(Pegasus::Texture::ITextureNodeProxy* proxy) { mTextureNodeProxy = proxy; }
        void SetTextureNodeObserver(TextureNodeObserver* textureNodeObserver) { mTextureNodeObserver = textureNodeObserver; }
        //void SetMessageType(MessageType t) { mMessageType = t; }
        
    private:

        //! Type of the message being sent
        MessageType mMessageType;

        //QVector<UpdateElement> mUpdates;
        TextureNodeProxyHandle mTextureNodeProxyHandle;
        Pegasus::Texture::ITextureNodeProxy* mTextureNodeProxy;
        TextureNodeObserver* mTextureNodeObserver;
    };


    //! Constructor
    explicit GraphIOMessageController(Pegasus::App::IApplicationProxy* appProxy);

    //! Destructor
    virtual ~GraphIOMessageController();

    //! Called by the render thread when a render thread message should be processed
    //! \param m The message to get processed
    void OnRenderThreadProcessMessage(const Message& m);

    //! Call at the end of the frame, whenever it is ideal to flush all the changes in the graph
    void FlushAllPendingUpdates();

//signals:
//    //signal triggered when a redraw is requested.
//    void RequestRedraw();
//
private:

    void OnRenderThreadOpen(TextureNodeObserver* sender, Pegasus::Texture::ITextureNodeProxy* proxy);
//    void OnRenderThreadUpdate(PropertyGridObserver* sender, PropertyGridHandle handle, const QVector<UpdateElement>& elements);
//    void OnRenderThreadClose(PropertyGridObserver* sender, PropertyGridHandle handle);

    void UpdateObserverInternal(TextureNodeObserver* sender, TextureNodeProxyHandle handle, Pegasus::Texture::ITextureNodeProxy* proxy);
//    void CloseHandleInternal(PropertyGridHandle handle);
//
//    void FlushPendingUpdates(PropertyUserData* userData);
//
//    
    // Texture event listener callbacks (from app to UI)
    virtual void OnEvent(Pegasus::Core::IEventUserData* userData, Pegasus::Texture::TextureNodeNotificationEvent& e);
    virtual void OnEvent(Pegasus::Core::IEventUserData* userData, Pegasus::Texture::TextureNodeGenerationEvent& e);
    virtual void OnEvent(Pegasus::Core::IEventUserData* userData, Pegasus::Texture::TextureNodeOperationEvent& e);

    Pegasus::App::IApplicationProxy* mAppProxy;

    typedef QSet<TextureNodeObserver*> ObserverSet;
    typedef QMap<TextureNodeProxyHandle, ObserverSet> ObserverMap;
    ObserverMap mObservers;

//    typedef QSet<TextureProxyHandle> HandleSet;
//    HandleSet        mLayoutsToReset;

    typedef QMap<TextureNodeProxyHandle, Pegasus::Texture::ITextureNodeProxy*> HandleToProxyMap;    
    HandleToProxyMap mActiveTextures;

    typedef QMap<TextureNodeProxyHandle, UpdateCache> ProxyUpdateCache;
    ProxyUpdateCache mUpdateCache;
 
    TextureNodeProxyHandle mNextHandle;
};


//! Class representing a texture node modifier callback. To be used when we want to update the view
//! after an element of the texture graph has been updated either by the UI or the render app
class TextureNodeObserver : public QObject
{
    friend class GraphIOMessageController;

    Q_OBJECT

public:

    //! Constructor
    TextureNodeObserver();
    
    //! Destructor
    virtual ~TextureNodeObserver() {}

    //! Called when this observer gets notified that the IO controller is done initializing
    //! \param handle The new handle. Store this handle locally in this observer for this session
    //! \param textureProxy Proxy for the observed texture
    virtual void OnInitialized(TextureNodeProxyHandle handle, const Pegasus::Texture::ITextureNodeProxy * textureProxy) = 0;

    //! Called when an external process (the render thread) has edited an element of the graph
    //! \param handle The handle that has been updated. This parameter will always be the same one as returned by OnInitialized()
    //! \param updateElements Batch of updates to propagate to the UI
    virtual void OnUpdated(TextureNodeProxyHandle handle, const QVector<GraphIOMessageController::UpdateElement>& updateElements) = 0;

    //! Called when the handle has been closed, or the proxy object has been destroyed and no longer exists
    virtual void OnShutdown(TextureNodeProxyHandle handle) = 0;

signals:

    void OnInitializedSignal(TextureNodeProxyHandle handle, const Pegasus::Texture::ITextureNodeProxy* textureProxy);
    void OnUpdatedSignal(TextureNodeProxyHandle handle, const QVector<GraphIOMessageController::UpdateElement> updateElements);
    void OnShutdownSignal(TextureNodeProxyHandle handle);

private slots:

    void OnInitializedSlot(TextureNodeProxyHandle handle, const Pegasus::Texture::ITextureNodeProxy* textureProxy) { OnInitialized(handle, textureProxy); }
    void OnUpdatedSlot(TextureNodeProxyHandle handle, const QVector<GraphIOMessageController::UpdateElement> updateElements) { OnUpdated(handle, updateElements); }
    void OnShutdownSlot(TextureNodeProxyHandle handle) { OnShutdown(handle); }
};


#endif  // PEGASUS_GRAPHIOMESSAGECONTROLLER_H
