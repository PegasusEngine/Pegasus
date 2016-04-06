/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Editor                                    */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineIOMessageController.cpp
//! \author	Kleber Garcia
//! \date	21st Februarty 2016
//! \brief	Timeline Message controller

#include "Editor.h"
#include "MessageControllers/TimelineIOMessageController.h"
#include "Pegasus/Application/Shared/IApplicationProxy.h"
#include "Pegasus/AssetLib/Shared/IRuntimeAssetObjectProxy.h"
#include "Pegasus/AssetLib/Shared/IAssetProxy.h"
#include "Pegasus/PegasusAssetTypes.h"
#include "Pegasus/Core/Shared/ISourceCodeProxy.h"
#include "Pegasus/Timeline/Shared/ITimelineManagerProxy.h"
#include "Pegasus/Timeline/Shared/ITimelineProxy.h"
#include "Pegasus/Timeline/Shared/IBlockProxy.h"
#include "Pegasus/AssetLib/Shared/IAssetLibProxy.h"
#include "Pegasus/AssetLib/Shared/IRuntimeAssetObjectProxy.h"

using namespace Pegasus::Core;
using namespace Pegasus::Timeline;
using namespace Pegasus::AssetLib;

TimelineIOMessageController::TimelineIOMessageController(Pegasus::App::IApplicationProxy* app)
: mApp(app)
{
}

TimelineIOMessageController::~TimelineIOMessageController()
{
}

QVariant TimelineIOMessageController::TranslateToQt(AssetInstanceHandle handle, Pegasus::AssetLib::IRuntimeAssetObjectProxy* object)
{
    Pegasus::Timeline::ITimelineProxy* timelineProxy = static_cast<Pegasus::Timeline::ITimelineProxy*>(object);

    //HACK: just pass the timeline proxy as a qvariant for now.
    return qVariantFromValue((void*)timelineProxy);
    
}

const Pegasus::PegasusAssetTypeDesc** TimelineIOMessageController::GetTypeList() const
{
    static const Pegasus::PegasusAssetTypeDesc* sAssetTypeList[] = {
        &Pegasus::ASSET_TYPE_TIMELINE,
        nullptr
    };

    return sAssetTypeList;
}

void TimelineIOMessageController::OnRenderThreadProcessMessage(const TimelineIOMessageController::Message& msg)
{
    //TODO: process any messages here.
    switch(msg.GetMessageType())
    {
    case TimelineIOMessageController::Message::SET_BLOCKSCRIPT:
        OnSetBlockscript(msg.GetString(), msg.GetBlockGuid());
        break;
    case TimelineIOMessageController::Message::CLEAR_BLOCKSCRIPT:
        OnClearBlockscript(msg.GetBlockGuid());
        break;
    case TimelineIOMessageController::Message::SET_MASTER_BLOCKSCRIPT:
        OnSetMasterBlockscript(msg.GetString());
        break;
    case TimelineIOMessageController::Message::CLEAR_MASTER_BLOCKSCRIPT:
        OnClearMasterBlockscript();
        break;
    default:
        break;
    }
}


void TimelineIOMessageController::OnSetBlockscript(const QString& str, unsigned blockGuid)
{
    ITimelineProxy* timeline = mApp->GetTimelineManagerProxy()->GetCurrentTimeline();
    IBlockProxy* block = timeline->FindBlockByGuid(blockGuid);
    IAssetLibProxy* assetLib = mApp->GetAssetLibProxy();
    QByteArray ba = str.toLocal8Bit();
    const char* asciiPath = ba.constData();
    if (block != nullptr)
    {
        bool isNew = false;
        IRuntimeAssetObjectProxy* object = assetLib->LoadObject(asciiPath, &isNew);
        if (object != nullptr)
        {
            if(object->GetOwnerAsset()->GetTypeDesc()->mTypeGuid == Pegasus::ASSET_TYPE_BLOCKSCRIPT.mTypeGuid)
            {
                ISourceCodeProxy* sourceProxy = static_cast<ISourceCodeProxy*>(object);
                block->AttachScript(sourceProxy);
            }
            //no need to keep a reference of this in the asset lib.
            if (isNew)
            {
                assetLib->CloseObject(object);
            }
        }
    }
    emit(NotifyRepaintTimeline());    
}

void TimelineIOMessageController::OnClearBlockscript(unsigned blockGuid)
{
    ITimelineProxy* timeline = mApp->GetTimelineManagerProxy()->GetCurrentTimeline();
    IBlockProxy* block = timeline->FindBlockByGuid(blockGuid);
    if (block != nullptr)
    {
        block->ClearScript();
    }
    emit(NotifyRepaintTimeline());    
}

void TimelineIOMessageController::OnSetMasterBlockscript(const QString& str)
{
    ITimelineProxy* timeline = mApp->GetTimelineManagerProxy()->GetCurrentTimeline();
}

void TimelineIOMessageController::OnClearMasterBlockscript()
{
    ITimelineProxy* timeline = mApp->GetTimelineManagerProxy()->GetCurrentTimeline();
}


