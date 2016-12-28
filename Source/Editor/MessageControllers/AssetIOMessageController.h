/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	AssetIOMessageController.h
//! \author	Kleber Garcia
//! \date	31st March 2015
//! \brief	Asset IO controller - keeps state of assets / dirty / on loading etc

#ifndef EDITOR_ASSET_IO_CONTROLLER_H
#define EDITOR_ASSET_IO_CONTROLLER_H

#include <QObject>
#include "Pegasus/Pegasus.h"
#include "Pegasus/Core/Shared/IoErrors.h"
#include "Widgets/HandleMap.h"
#include "MessageControllers/MsgDefines.h"
#include "Pegasus/AssetLib/Shared/AssetEvent.h"

//forward declarations
class PegasusDockWidget;
class AssetIOMessageController;

namespace Pegasus
{
    struct PegasusAssetTypeDesc;

    namespace App
    {
        class IApplicationProxy;
    }

    namespace AssetLib
    {
        class IAssetProxy;
        class IAssetLibProxy;
        class IRuntimeAssetObjectProxy;
    }

    namespace Core
    {
        class ISourceCodeProxy;
    }
}

//-------------------------------------------------------------

//Translates an asset type into a qt generic data type.
class IAssetTranslator
{
public:
    IAssetTranslator() : mAc(nullptr) {}
    virtual ~IAssetTranslator() {}

    //! \param handle - handle to used for this object. Use this to set the user data,
    //!                 so the event listeners can send messages with the handle to the ui.
    //! \param object input asset to open
    //! \return a qvariant that contains the representation of this asset for the ui to read.
    virtual QVariant TranslateToQt(AssetInstanceHandle handle, Pegasus::AssetLib::IRuntimeAssetObjectProxy* object) = 0;

    //! \param object asked for validity.
    //! \return a boolean expressing if the state of this object is valid, or not.
    //!         for example, a shader code that had a compilation error must return false. This will help the asset instance
    //!         viewer display feedback on such object.
    virtual bool IsRuntimeObjectValid(Pegasus::AssetLib::IRuntimeAssetObjectProxy* object) { return true; }

    //! \return null terminated list that contains all the necessary asset data so assets can be translated.
    virtual const Pegasus::PegasusAssetTypeDesc** GetTypeList() const = 0;

    //! Internally resolve a handle to an object.
    //! \param handle - the handle to resolve
    //! \return the object instance
    Pegasus::AssetLib::IRuntimeAssetObjectProxy* FindInstance(AssetInstanceHandle handle);

    //! Register an asset io controller. This is done by the asset controller itself when a translator gets registered.
    void RegisterAssetIOController(AssetIOMessageController* ac);

private:
    AssetIOMessageController* mAc;
};

//! Class representing an asset tree view. To be used when:
//! We want to visualize an asset tree of the project.
class AssetTreeObserver : public QObject
{
    Q_OBJECT

public:

    //TODO: if it becomes too much of a performance problem, make sure that we also push updates.
    //! Constructor
    AssetTreeObserver();

    //! Destructor
    virtual ~AssetTreeObserver(){}

    //! Called when there is a new tree.
    virtual void OnInitialized(const AssetViewTree& tree) = 0; 

    //! Called when an asset is notified regarding its validity
    virtual void OnSetValidity(const QString& assetPath, bool validity) = 0; 

    //! Called when the tree of assets in the project has been closed.
    virtual void OnShutdown() = 0;

signals:
    void OnInitializedSignal(AssetViewTree tree); 

    void OnSetValiditySignal(QString assetPath, bool validity);

    void OnShutdownSignal();

private slots:
    void OnInitializedSlot(AssetViewTree tree) { OnInitialized(tree); }

    void OnSetValiditySlot(QString assetPath, bool validity) { OnSetValidity(assetPath, validity); }

    void OnShutdownSlot() { OnShutdown(); }
};

//! Asset IO controller class
class AssetIOMessageController : public QObject, public Pegasus::AssetLib::IAssetEventListener
{
    Q_OBJECT;
public:
    //! Constructor
    explicit AssetIOMessageController(Pegasus::App::IApplicationProxy* app);

    //! Destructor
    virtual ~AssetIOMessageController();

    //! Registers a translator that translates from proxy to qt type.
    void RegisterQtTranslator(IAssetTranslator* translator);

    //! Called by the render thread when we open a message
    void OnRenderThreadProcessMessage(PegasusDockWidget* sender, const AssetIOMCMessage& msg);

    //! Returns the asset instance using a handle.
    Pegasus::AssetLib::IRuntimeAssetObjectProxy* FindInstance(AssetInstanceHandle handle);

    //! Flushes all pending updates done throughout events.
    void FlushAllPendingUpdates();

public slots:

    //! Tags if an asset is valid or invalid
    //! \param assetPath - the asset to discover if its valid or invalid
    //! \param isValid - validity state
    void OnTagValidity(QString assetPath, bool isValid);

signals:

    //! Signal triggered when an object is requested for opening. Calls the UI to open on a proper editor.
    void SignalOpenObject(AssetInstanceHandle handle, QString displayName, int typeGuid, QVariant initData);
    
    //! Signal triggered when the UI needs to update the active node views
    void RedrawFrame();

    //! Signal triggered when an error has occured.
    void SignalOnErrorMessagePopup(const QString& message);

    //! Signal triggered when a message is sent to the code editor.
    void SignalPostMessage(PegasusDockWidget* sender, AssetIOMCMessage::IoResponseMessage id);

private:
    
    //! Opens a serialized object stored in the asset path passed.
    //! \param path the file path containing the asset
    void OnRenderRequestOpenObject(const QString& path);

    //! Closes (decreases ref count) on serialized object
    //! \param object to close
    void OnRenderRequestCloseObject(AssetInstanceHandle& handle);

    //! Reloads an object from disk (discards its current state)
    //! \param object handle to reload
    void OnRenderRequestReloadObject(AssetInstanceHandle& handle);

    //! Saves object to disk.
    //! \param sender dock widget - for message responses
    //! \param object - the object to save
    void OnSaveObject(PegasusDockWidget* sender, AssetInstanceHandle& handle);

    //! Requests a new object internally
    //! \param path - the path to use to create the object
    //! \param desc - the description of the object used
    void OnRenderRequestNewObject(PegasusDockWidget* sender, const QString& path, const Pegasus::PegasusAssetTypeDesc* desc);

    //! Requests a new observation of an asset tree.
    //! \param observer - the observer querying this tree to observe.
    void OnRenderRequestQueryStartViewAssetTree(AssetTreeObserver* observer);

    //! Close an observation of an asset tree.
    //! \param observer - the observer querying this tree to observe.
    void OnRenderRequestQueryStopViewAssetTree(AssetTreeObserver* observer);

    //!{@
    //! Event listeners
    virtual void OnEvent(Pegasus::Core::IEventUserData* userData, Pegasus::AssetLib::AssetCreated& e);
    virtual void OnEvent(Pegasus::Core::IEventUserData* userData, Pegasus::AssetLib::AssetLinkAdded& e);
    virtual void OnEvent(Pegasus::Core::IEventUserData* userData, Pegasus::AssetLib::AssetDestroyed& e);
    virtual void OnEvent(Pegasus::Core::IEventUserData* userData, Pegasus::AssetLib::RuntimeAssetObjectCreated& e);
    virtual void OnEvent(Pegasus::Core::IEventUserData* userData, Pegasus::AssetLib::RuntimeAssetObjectDestroyed& e);
    //!@}

    //! Sends a request brand new tree for all observers.
    void RequestRefreshViewOnAllObservers() { ++mRefreshViewsRequestCount; }

    //! Sends a brand new tree for all observers.
    void RefreshViewOnAllObservers();

    void InternalBuildAssetTree(Pegasus::App::IApplicationProxy* appProxy, Pegasus::AssetLib::IAssetLibProxy* assetLib, AssetViewTree& outTree);

    int InternalBuildSingleAssetTree(
        QMap<Pegasus::AssetLib::IAssetProxy*, int>& cachedAssetMap,
        Pegasus::AssetLib::IAssetProxy* assetProxy,
        AssetViewTree& outTree);

    void AssetIOMessageController::InternalBuildAssetTreeFromObj(
        QMap<Pegasus::AssetLib::IAssetProxy*, int>& cachedAssetMap, 
        Pegasus::AssetLib::IObjectProxy* obj, 
        AssetInformation& outAssetInfo,
        AssetViewTree& outTree);

    Pegasus::App::IApplicationProxy* mApp;

    HandleMap<AssetInstanceHandle, Pegasus::AssetLib::IRuntimeAssetObjectProxy*> mObjectMap;

    typedef QMap<int, IAssetTranslator*> TypeTranslatorMap;
    typedef QSet<AssetTreeObserver*> TreeObserverSet;
    TypeTranslatorMap mQtTranslators;
    TreeObserverSet mActiveTreeViews;

    int mRefreshViewsRequestCount;
    
};


#endif
