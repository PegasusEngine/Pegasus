#include "MessageControllers/AssetIOMessageController.h"
#include "Pegasus/PegasusAssetTypes.h"
#include "Pegasus/Version.h"
#include "Pegasus/Application/Shared/IApplicationProxy.h"
#include "Pegasus/Shader/Shared/IShaderManagerProxy.h"
#include "Pegasus/Shader/Shared/IShaderProxy.h"
#include "Pegasus/Shader/Shared/IProgramProxy.h"
#include "Pegasus/Timeline/Shared/ITimelineManagerProxy.h"
#include "Pegasus/Timeline/Shared/ITimelineProxy.h"
#include "Pegasus/AssetLib/Shared/IAssetLibProxy.h"
#include "Pegasus/AssetLib/Shared/IAssetProxy.h"
#include "Pegasus/AssetLib/Shared/ICategoryProxy.h"
#include "Pegasus/AssetLib/Shared/IRuntimeAssetObjectProxy.h"
#include "Pegasus/AssetLib/Shared/IObjectProxy.h"
#include "Pegasus/AssetLib/Shared/IArrayProxy.h"
#include "Pegasus/Timeline/Shared/ITimelineManagerProxy.h"
#include "Pegasus/Timeline/Shared/ITimelineProxy.h"
#include "Pegasus/Timeline/Shared/ILaneProxy.h"
#include "Pegasus/Timeline/Shared/LaneDefs.h"
#include "Pegasus/Timeline/Shared/IBlockProxy.h"
#include "CodeEditor/SourceCodeManagerEventListener.h"
#include "Widgets/PegasusDockWidget.h"
#include <QByteArray>


Pegasus::AssetLib::IRuntimeAssetObjectProxy* IAssetTranslator::FindInstance(AssetInstanceHandle handle)
{
    ED_ASSERT(mAc != nullptr);
    return mAc->FindInstance(handle);
}

void IAssetTranslator::RegisterAssetIOController(AssetIOMessageController* ac)
{
    ED_ASSERT(mAc == nullptr);
    mAc = ac;
    mAc->RegisterQtTranslator(this);
}

AssetIOMessageController::AssetIOMessageController(Pegasus::App::IApplicationProxy* app)
: mApp(app), mRefreshViewsRequestCount(0)
{
}

AssetIOMessageController::~AssetIOMessageController()
{
    //clear all the instance trees.
    for (TreeObserverSet::iterator it = mActiveTreeViews.begin(); it != mActiveTreeViews.end(); ++it)
    {
        emit((*it)->OnShutdownSignal());
    }
}

void AssetIOMessageController::RegisterQtTranslator(IAssetTranslator* translator)
{
    const Pegasus::PegasusAssetTypeDesc** types = translator->GetTypeList();
    int i = 0;
    while (types != nullptr && types[i] != nullptr)
    {
        mQtTranslators.insert(types[i++]->mTypeGuid, translator);
    }
}

void AssetIOMessageController::OnRenderThreadProcessMessage(PegasusDockWidget* sender, const AssetIOMCMessage& msg)
{
    switch(msg.GetMessageType())
    {
        case AssetIOMCMessage::OPEN_ASSET:
            {
                OnRenderRequestOpenObject(msg.GetString());
            }
            break;
        case AssetIOMCMessage::CLOSE_ASSET:
            {
                OnRenderRequestCloseObject(msg.GetObject());
            }
            break;
        case AssetIOMCMessage::RELOAD_FROM_ASSET:
            {
                OnRenderRequestReloadObject(msg.GetObject());
            }
            break;
        case AssetIOMCMessage::SAVE_ASSET:
            {
                OnSaveObject(sender, msg.GetObject());
            }
            break;
        case AssetIOMCMessage::NEW_ASSET:
            {
                OnRenderRequestNewObject(sender, msg.GetString(), msg.GetTypeDesc());
            }
            break;
        case AssetIOMCMessage::QUERY_START_VIEW_ASSET_TREE:
        case AssetIOMCMessage::QUERY_REFRESH_VIEW_ASSET_TREE:
            {
                OnRenderRequestQueryStartViewAssetTree(msg.GetTreeObserver());
            }
            break;
        case AssetIOMCMessage::QUERY_STOP_VIEW_ASSET_TREE:
            {
                OnRenderRequestQueryStopViewAssetTree(msg.GetTreeObserver());
            }
            break;
        default:
            break;
    }
}


void AssetIOMessageController::OnRenderRequestOpenObject(const QString& path)
{
    QByteArray ba = path.toLocal8Bit();
    const char* asciiPath = ba.constData();

    ED_ASSERT(asciiPath != nullptr);
    if (mApp != nullptr)
    {
        Pegasus::AssetLib::IAssetLibProxy*  assetLib = mApp->GetAssetLibProxy();
        Pegasus::AssetLib::IRuntimeAssetObjectProxy* object = assetLib->LoadObject(asciiPath);

        //can't deserialize this object.
        if (object == nullptr)
        {
            return;
        }
        ED_ASSERT(object->GetOwnerAsset() != nullptr);
        TypeTranslatorMap::iterator it = mQtTranslators.find(object->GetOwnerAsset()->GetTypeDesc()->mTypeGuid);
        int typeGuid = object->GetOwnerAsset()->GetTypeDesc()->mTypeGuid;
        if (it != mQtTranslators.end())
        {
            if (object != nullptr)
            {            
                AssetInstanceHandle h = mObjectMap.Insert(object);
                QVariant data = it.value()->TranslateToQt(h, object);
                emit(SignalOpenObject(h, object->GetDisplayName(), typeGuid, data));
            }
        }
        else if (object != nullptr)
        {
            ED_LOG("No io controller that could process this asset type. %s", asciiPath);
            assetLib->CloseObject(object);
        }
        else
        {
            ED_LOG("%s cannot be opened by Pegasus.", asciiPath);
        }
    }

    emit(RedrawFrame());
}

void AssetIOMessageController::OnRenderRequestCloseObject(AssetInstanceHandle& handle)
{
    ED_ASSERT(mObjectMap.Has(handle));

    Pegasus::AssetLib::IRuntimeAssetObjectProxy* obj = mObjectMap.Get(handle);

    Pegasus::AssetLib::IAssetLibProxy* assetLib = mApp->GetAssetLibProxy();
    assetLib->CloseObject(obj);
    emit(RedrawFrame());
}

void AssetIOMessageController::OnRenderRequestReloadObject(AssetInstanceHandle& handle)
{
    ED_ASSERT(mObjectMap.Has(handle));

    Pegasus::AssetLib::IRuntimeAssetObjectProxy* obj = mObjectMap.Get(handle);
    obj->ReloadFromAsset();
    emit(RedrawFrame());
}

void AssetIOMessageController::OnSaveObject(PegasusDockWidget* sender, AssetInstanceHandle& handle)
{
    Pegasus::AssetLib::IRuntimeAssetObjectProxy* object = mObjectMap.Get(handle);
    Pegasus::Io::IoError err = mApp->GetAssetLibProxy()->SaveObject(object);

    if (err == Pegasus::Io::ERR_NONE)
    {
        emit(SignalPostMessage(sender, AssetIOMCMessage::IO_SAVE_SUCCESS));
    }
    else
    {
        emit(SignalPostMessage(sender, AssetIOMCMessage::IO_SAVE_ERROR));
    }

    FlushAllPendingUpdates();
}

void AssetIOMessageController::OnRenderRequestNewObject(PegasusDockWidget* sender, const QString& path, const Pegasus::PegasusAssetTypeDesc* desc)
{
    QByteArray ba = path.toLocal8Bit();
    const char* asciiPath = ba.constData();

    ED_ASSERT(asciiPath != nullptr);
    Pegasus::AssetLib::IAssetLibProxy* assetLib = mApp->GetAssetLibProxy();
    Pegasus::AssetLib::IRuntimeAssetObjectProxy* newObj = assetLib->CreateObject(asciiPath, desc);
    ED_ASSERT(newObj->GetOwnerAsset() != nullptr);
    TypeTranslatorMap::iterator it = mQtTranslators.find(newObj->GetOwnerAsset()->GetTypeDesc()->mTypeGuid);
    int typeGuid = newObj->GetOwnerAsset()->GetTypeDesc()->mTypeGuid;
    if (it != mQtTranslators.end())
    {
        if (newObj != nullptr)
        {
            AssetInstanceHandle handle = mObjectMap.Insert(newObj);
            QVariant data = it.value()->TranslateToQt(handle, newObj);

            if (Pegasus::Io::ERR_NONE == assetLib->SaveObject(newObj))
            {
                emit(SignalOpenObject(handle, newObj->GetDisplayName(), typeGuid, data));  
            } 
            else
            {
                emit(SignalPostMessage(sender, AssetIOMCMessage::IO_NEW_ERROR));
            }
        }
        else
        {
            emit(SignalPostMessage(sender, AssetIOMCMessage::IO_NEW_ERROR));
        }
    }
    else
    {
        ED_LOG("Cannot create new asset because of unknown handled type: %s", asciiPath);
    }
    emit(RedrawFrame());
}

void AssetIOMessageController::InternalBuildAssetTreeFromObj(
    QMap<Pegasus::AssetLib::IAssetProxy*, int>& cachedAssetMap, 
    Pegasus::AssetLib::IObjectProxy* obj, 
    AssetInformation& outAssetInfo,
    AssetViewTree& outTree)
{

    for (unsigned i = 0; i < obj->GetChildAssetCount(); ++i)
    {
        Pegasus::AssetLib::IAssetProxy* childProxy = obj->GetChildAsset(i);
        int childAssetTreeId = InternalBuildSingleAssetTree(cachedAssetMap, childProxy, outTree);
        outAssetInfo.children.push_back(childAssetTreeId);
    }

    for (unsigned i = 0; i < obj->GetChildObjectCount(); ++i)
    {
        InternalBuildAssetTreeFromObj(cachedAssetMap, obj->GetChildObject(i), outAssetInfo,  outTree);
    }

    for (unsigned i = 0; i < obj->GetArrayCount(); ++i)
    {
        Pegasus::AssetLib::IArrayProxy* arrProxy = obj->GetArray(i);
        if (arrProxy->IsObjectType())
        {
            for (unsigned j = 0; j < arrProxy->GetSize(); ++j)
            {
                InternalBuildAssetTreeFromObj(cachedAssetMap, arrProxy->GetObjectEl(j), outAssetInfo, outTree);
            }
        }
        else if (arrProxy->IsAssetType())
        {
            for (unsigned j = 0; j < arrProxy->GetSize(); ++j)
            {
                Pegasus::AssetLib::IAssetProxy* childProxy = arrProxy->GetAssetEl(j);
                int childAssetTreeId = InternalBuildSingleAssetTree(cachedAssetMap, childProxy, outTree);
                outAssetInfo.children.push_back(childAssetTreeId);
            }
        }
    }
}

int AssetIOMessageController::InternalBuildSingleAssetTree(
    QMap<Pegasus::AssetLib::IAssetProxy*, int>& cachedAssetMap,
    Pegasus::AssetLib::IAssetProxy* assetProxy,
    AssetViewTree& outTree)
{
    AssetInformation outAssetInfo;
    int treeId = -1;
    QMap<Pegasus::AssetLib::IAssetProxy*, int>::iterator it = cachedAssetMap.find(assetProxy);
    if (it == cachedAssetMap.end())
    {
        treeId = (int)outTree.allAssets.size(); 
        outTree.allAssets.push_back(AssetInformation());
        cachedAssetMap.insert(assetProxy, treeId);
    
        if (assetProxy->Root() != nullptr)
        {
            InternalBuildAssetTreeFromObj(cachedAssetMap, assetProxy->Root(), outAssetInfo, outTree);
        }

        if (assetProxy->GetTypeDesc() != nullptr)
        {
            TypeTranslatorMap::iterator it = mQtTranslators.find(assetProxy->GetTypeDesc()->mTypeGuid);
            if (it != mQtTranslators.end())
            {
                outAssetInfo.isValid = (*it)->IsRuntimeObjectValid(assetProxy->GetRuntimeObject());
            }
            else
            {
                outAssetInfo.isValid = false;
            }
        }
    
        outAssetInfo.displayName = assetProxy->GetPath();
        outAssetInfo.typeInfo = assetProxy->GetTypeDesc();
        outTree.allAssets[treeId] = outAssetInfo;
    }
    else
    {
        treeId = it.value();
    }

    return treeId;
}

QString GetCategoryName(Pegasus::App::IApplicationProxy* app, Pegasus::AssetLib::ICategoryProxy* category)
{
    Pegasus::Timeline::ITimelineProxy* timeline = app->GetTimelineManagerProxy()->GetCurrentTimeline();
    Pegasus::Timeline::IBlockProxy* blockProxy = timeline->FindBlockByGuid(category->GetUserData());
    if (blockProxy != nullptr)
    {
        QString instanceName = blockProxy->GetInstanceName();
        QString className = blockProxy->GetClassName();
        return instanceName.isEmpty() ? className : instanceName + ":" + className;
    }

    return QString("<unknown>");
}

void AssetIOMessageController::InternalBuildAssetTree(Pegasus::App::IApplicationProxy* appProxy, Pegasus::AssetLib::IAssetLibProxy* assetLib, AssetViewTree& outTree)
{
    QMap<Pegasus::AssetLib::IAssetProxy*, int> cachedAssetMap;

    for (unsigned categoryId = 0; categoryId < assetLib->GetCategoryCount(); ++categoryId)
    {
        Pegasus::AssetLib::ICategoryProxy* categoryProxy = assetLib->GetCategory(categoryId);
        AssetCategory categoryInfo;
        categoryInfo.displayName = GetCategoryName(appProxy, categoryProxy);; //todo: bring here the name
        categoryInfo.blockGuid = categoryProxy->GetUserData();

        for (unsigned assetId = 0; assetId < categoryProxy->GetAssetCount(); ++assetId)
        {
            Pegasus::AssetLib::IAssetProxy* assetProxy = categoryProxy->GetAsset(assetId);
            int treeAssetId = InternalBuildSingleAssetTree(cachedAssetMap, assetProxy, outTree);
            categoryInfo.children.push_back(treeAssetId);
        }

        outTree.categories.push_back(categoryInfo);
    }

    Pegasus::PegasusDesc engineDesc;
    appProxy->GetEngineDesc(engineDesc);

    const Pegasus::PegasusAssetTypeDesc* const * typeList = engineDesc.mAssetTypes;
    while (*typeList != nullptr)
    {
        const Pegasus::PegasusAssetTypeDesc* typeDesc = *typeList; 
        Pegasus::AssetLib::ICategoryProxy* categoryProxy = assetLib->FindTypeCategory(typeDesc);

        AssetCategory categoryInfo;
        categoryInfo.displayName = QString(typeDesc->mTypeName);
        categoryInfo.blockGuid = 0; //invalid block guid

        for (unsigned assetId = 0; assetId < categoryProxy->GetAssetCount(); ++assetId)
        {
            Pegasus::AssetLib::IAssetProxy* assetProxy = categoryProxy->GetAsset(assetId);
            int treeAssetId = InternalBuildSingleAssetTree(cachedAssetMap, assetProxy, outTree);
            categoryInfo.children.push_back(treeAssetId);
        }

        outTree.typeCategories.push_back(categoryInfo);

        typeList++;
    }
    
}


void AssetIOMessageController::OnRenderRequestQueryStartViewAssetTree(AssetTreeObserver* observer)
{
    if (mActiveTreeViews.find(observer) == mActiveTreeViews.end())
    {
        mActiveTreeViews.insert(observer);
    }
     
    if (mApp != nullptr)
    {
        Pegasus::AssetLib::IAssetLibProxy*  assetLib = mApp->GetAssetLibProxy();
        // generate the entire asset tree. This is an expensive step (if the asset tree is too big). In the future,
        // we can refactor this into only updating small deltas instead of the entire tree.
        AssetViewTree tree;
        InternalBuildAssetTree(mApp, mApp->GetAssetLibProxy(), tree);
        emit(observer->OnInitializedSignal(tree));
    }
}

void AssetIOMessageController::OnTagValidity(QString assetPath, bool validity)
{
    for (AssetTreeObserver* observer : mActiveTreeViews)
    {
        emit(observer->OnSetValiditySignal(assetPath, validity));
    }
}

void AssetIOMessageController::OnRenderRequestQueryStopViewAssetTree(AssetTreeObserver* observer)
{
    TreeObserverSet::iterator it = mActiveTreeViews.find(observer);
    if (it != mActiveTreeViews.end())
    {
        emit(observer->OnShutdownSignal());
        mActiveTreeViews.remove(observer);
    }
}


Pegasus::AssetLib::IRuntimeAssetObjectProxy* AssetIOMessageController::FindInstance(AssetInstanceHandle handle)
{
    if (mObjectMap.Has(handle))
    {
        return mObjectMap.Get(handle);
    }
    return nullptr;
}

void AssetIOMessageController::OnEvent(Pegasus::Core::IEventUserData* userData, Pegasus::AssetLib::AssetCreated& e)
{
    RequestRefreshViewOnAllObservers();
}

void AssetIOMessageController::OnEvent(Pegasus::Core::IEventUserData* userData, Pegasus::AssetLib::AssetLinkAdded& e)
{
    RequestRefreshViewOnAllObservers();
}

void AssetIOMessageController::OnEvent(Pegasus::Core::IEventUserData* userData, Pegasus::AssetLib::AssetDestroyed& e)
{
    RequestRefreshViewOnAllObservers();
}

void AssetIOMessageController::OnEvent(Pegasus::Core::IEventUserData* userData, Pegasus::AssetLib::RuntimeAssetObjectCreated& e)
{
    RequestRefreshViewOnAllObservers();
}

void AssetIOMessageController::OnEvent(Pegasus::Core::IEventUserData* userData, Pegasus::AssetLib::RuntimeAssetObjectDestroyed& e)
{
    RequestRefreshViewOnAllObservers();
    //Redraw is not triggered usually when an asset is closed. Go ahead and flush.
    emit(RedrawFrame());
}

void AssetIOMessageController::FlushAllPendingUpdates()
{
    RefreshViewOnAllObservers();
}

void AssetIOMessageController::RefreshViewOnAllObservers()
{
    if (mRefreshViewsRequestCount > 0)
    {
        AssetViewTree tree;
        InternalBuildAssetTree(mApp, mApp->GetAssetLibProxy(), tree);

        TreeObserverSet::iterator it = mActiveTreeViews.begin();
        for (;it != mActiveTreeViews.end(); ++it)
        {
            emit((*it)->OnInitializedSignal(tree));
        }
        mRefreshViewsRequestCount = 0;
    }
}

AssetTreeObserver::AssetTreeObserver()
{
    connect(this, SIGNAL(OnInitializedSignal(AssetViewTree)),
        this, SLOT(OnInitializedSlot(AssetViewTree)), Qt::QueuedConnection);

    connect(this, SIGNAL(OnSetValiditySignal(QString, bool)),
            this, SLOT(OnSetValiditySlot(QString, bool)), Qt::QueuedConnection);

    connect(this, SIGNAL(OnShutdownSignal()),
            this, SLOT(OnShutdownSlot()), Qt::QueuedConnection);
}


