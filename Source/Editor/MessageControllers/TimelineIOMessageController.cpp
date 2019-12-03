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
#include "Pegasus/Timeline/Shared/ILaneProxy.h"
#include "Pegasus/Timeline/Shared/IBlockProxy.h"
#include "Pegasus/AssetLib/Shared/IAssetLibProxy.h"
#include "Pegasus/AssetLib/Shared/IRuntimeAssetObjectProxy.h"

using namespace Pegasus;
using namespace Pegasus::Timeline;
using namespace Pegasus::AssetLib;

#define E2S_BEGIN(V) switch(V) {
#define E2S(X) case X: return QString( #X );
#define E2S_END() default: return QString("Unknown-enum"); }
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//Implementation of all shadow states of timeline.
ShadowTimelineState::ShadowTimelineState()
{
}

QString ShadowTimelineState::Str(ShadowTimelineState::PropName nm)
{
    E2S_BEGIN(nm)
    E2S(PROP_CURR_BEAT)
    E2S(PROP_BEATS)
    E2S(PROP_BEATS_PER_MIN)
    E2S(PROP_TICKS_PER_BEAT)
    E2S(PROP_SCRIPT_PATH)
    E2S(PROP_LANE_COUNT)
    E2S(PROP_LANES)
    E2S(PROP_VOLUME)
    E2S(PROP_COUNT)
    E2S_END()
}

unsigned ShadowTimelineState::GetLaneCount() const
{
    return mRootState[Str(PROP_LANE_COUNT)].toUInt();
}

unsigned ShadowTimelineState::GetNumBeats() const
{
    return mRootState[Str(PROP_BEATS)].toUInt();
}

float ShadowTimelineState::GetBeatsPerMinute() const
{
    return mRootState[Str(PROP_BEATS_PER_MIN)].toFloat();
}

QString ShadowTimelineState::GetMasterScriptPath() const
{
    return mRootState[Str(PROP_SCRIPT_PATH)].toString();
}

float ShadowTimelineState::GetCurrBeat() const
{
    return mRootState[Str(PROP_CURR_BEAT)].toFloat();
}

float ShadowTimelineState::GetVolume() const
{
    return mRootState[Str(PROP_VOLUME)].toFloat();
}

bool ShadowTimelineState::HasMasterScript() const
{
    return mRootState.find(Str(PROP_SCRIPT_PATH)) != mRootState.end();
}

unsigned ShadowTimelineState::GetNumTicksPerBeat() const
{
    return mRootState[Str(PROP_TICKS_PER_BEAT)].toUInt();
}

void ShadowTimelineState::Build(const ITimelineProxy* proxy)
{
    mRootState = QVariantMap();
    mRootState.insert(Str(PROP_LANE_COUNT), proxy->GetNumLanes());
    mRootState.insert(Str(PROP_BEATS), proxy->GetNumBeats());
    mRootState.insert(Str(PROP_BEATS_PER_MIN), proxy->GetBeatsPerMinute() );
    mRootState.insert(Str(PROP_CURR_BEAT), proxy->GetCurrentBeat() );
    mRootState.insert(Str(PROP_TICKS_PER_BEAT), proxy->GetNumTicksPerBeat() );
    mRootState.insert(Str(PROP_VOLUME), proxy->GetVolume() );

    bool scriptIsLoaded = proxy->GetScript() != nullptr;
    if (scriptIsLoaded)
    {
        mRootState.insert(Str(PROP_SCRIPT_PATH), QString(proxy->GetScript()->GetOwnerAsset()->GetPath()));
    }

    QVariantList lanes;
    for (unsigned i = 0; i < proxy->GetNumLanes(); ++i)
    {
        ShadowLaneState laneState;
        laneState.Build(proxy->GetLane(i));
        lanes.append(laneState.GetRootState());
    }

    mRootState.insert(Str(PROP_LANES), lanes);
}

void ShadowTimelineState::FlushProp(ShadowTimelineState::PropName name, const QVariant& val, ITimelineProxy* dest)
{
    switch (name)
    {
    case PROP_BEATS:
        dest->SetNumBeats(val.toUInt());
        break;
    case PROP_BEATS_PER_MIN:
        dest->SetBeatsPerMinute(val.toFloat());
        break;
    case PROP_TICKS_PER_BEAT:
        //nothing to do here. Assert?
        break;
    case PROP_CURR_BEAT:
        dest->SetCurrentBeat(val.toFloat());
        break;
    case PROP_VOLUME:
        dest->SetVolume(val.toFloat());
        break;
    default:
        ED_FAILSTR("Unable to flush property from shadow state.");
        break;
    }
}

ShadowLaneState::ShadowLaneState()
{
}

QString ShadowLaneState::Str(ShadowLaneState::PropName name)
{
    E2S_BEGIN(name)
        E2S(PROP_BLOCK_COUNT)
        E2S(PROP_BLOCK_LIST)
        E2S(PROP_LANE_NAME)
    E2S_END()
}

QString ShadowLaneState::GetName() const
{
    return mRootState[Str(PROP_LANE_NAME)].toString();
}

unsigned ShadowLaneState::GetBlockCount() const
{
    return mRootState[Str(PROP_BLOCK_COUNT)].toUInt();
}

void ShadowLaneState::Build(const ILaneProxy* proxy)
{
    mRootState.insert(Str(PROP_LANE_NAME), QString(proxy->IsNameDefined() ? proxy->GetName() : ""));
    IBlockProxy* blockList[LANE_MAX_NUM_BLOCKS];
    unsigned int numBlocks = proxy->GetBlocks(blockList);
    mRootState.insert(Str(PROP_BLOCK_COUNT), numBlocks);

    QVariantList blockQtList;    
    for (unsigned int i = 0; i < numBlocks; ++i)
    {
        ShadowBlockState blockState;
        blockState.Build(blockList[i]);
        blockQtList.append(blockState.GetRootState());
    }

    mRootState.insert(Str(PROP_BLOCK_LIST), blockQtList);
}

void ShadowLaneState::FlushProp(ShadowLaneState::PropName name, const QVariant& val, ILaneProxy* proxy)
{
    switch (name)
    {
    case PROP_LANE_NAME:
        {
            QString qs = val.toString();
            QByteArray ba = qs.toLocal8Bit();
            const char* name = ba.constData();
            proxy->SetName(name);
        }
        break;
    default:
        ED_FAILSTR("Unable to flush property from shadow state.");
        break;
    }
}

ShadowBlockState::ShadowBlockState()
{
}

QString ShadowBlockState::Str(ShadowBlockState::PropName name)
{
    E2S_BEGIN(name)
        E2S(PROP_BLOCK_NAME)
        E2S(PROP_CLASS_NAME)
        E2S(PROP_BLOCK_COLOR)
        E2S(PROP_BLOCK_BEAT)
        E2S(PROP_BLOCK_GUID)
        E2S(PROP_BLOCK_DURATION)
        E2S(PROP_BLOCK_SCRIPT_PATH)
    E2S_END()
}

QString ShadowBlockState::GetName() const
{
    return mRootState[Str(PROP_BLOCK_NAME)].toString();
}

QString ShadowBlockState::GetClassName() const
{
    return mRootState[Str(PROP_CLASS_NAME)].toString();
}

unsigned ShadowBlockState::GetColor() const
{
    return mRootState[Str(PROP_BLOCK_COLOR)].toUInt();
}

unsigned ShadowBlockState::GetGuid() const
{
    return mRootState[Str(PROP_BLOCK_GUID)].toUInt();
}

unsigned ShadowBlockState::GetBeat() const
{
    return mRootState[Str(PROP_BLOCK_BEAT)].toUInt();
}

unsigned ShadowBlockState::GetDuration() const
{
    return mRootState[Str(PROP_BLOCK_DURATION)].toUInt();
}

bool ShadowBlockState::HasBlockScript() const
{
    return mRootState.find(Str(PROP_BLOCK_SCRIPT_PATH)) != mRootState.end();
}

QString ShadowBlockState::GetBlockScriptPath() const
{
    return mRootState[Str(PROP_BLOCK_SCRIPT_PATH)].toString();
}

void ShadowBlockState::Build(const IBlockProxy* proxy)
{
    mRootState.insert(Str(PROP_BLOCK_NAME), QString(proxy->GetInstanceName()));
    mRootState.insert(Str(PROP_CLASS_NAME), QString(proxy->GetClassName()));
    mRootState.insert(Str(PROP_BLOCK_BEAT), (unsigned int)proxy->GetBeat());
    if (proxy->GetScript() != nullptr && proxy->GetScript()->GetOwnerAsset() != nullptr)
    {
        mRootState.insert(Str(PROP_BLOCK_SCRIPT_PATH), QString(proxy->GetScript()->GetOwnerAsset()->GetPath()));
    }
    unsigned char r, g, b;
    proxy->GetColor(r,g,b);
    unsigned int col = (r << 24) | (g << 16) | (b << 8);
    mRootState.insert(Str(PROP_BLOCK_COLOR), col);
    mRootState.insert(Str(PROP_BLOCK_GUID), proxy->GetGuid());
    unsigned int duration = proxy->GetDuration();
    mRootState.insert(Str(PROP_BLOCK_DURATION), duration);
}

void ShadowBlockState::SetScript(const QString& script)
{
    mRootState.insert(Str(PROP_BLOCK_SCRIPT_PATH), script);
}

void ShadowBlockState::RemoveScript()
{
    QVariantMap::iterator it = mRootState.find(Str(PROP_BLOCK_SCRIPT_PATH));
    if (it != mRootState.end())
    {
        mRootState.erase(it);
    }
}

//------------------------------------------------------------------------------

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

    timelineProxy->SetPlayMode(Pegasus::Timeline::PLAYMODE_STOPPED);
    ShadowTimelineState shadowState;
    shadowState.Build(timelineProxy);
    return shadowState.GetRootState();
}

const Pegasus::PegasusAssetTypeDesc** TimelineIOMessageController::GetTypeList() const
{
    static const Pegasus::PegasusAssetTypeDesc* sAssetTypeList[] = {
        &Pegasus::ASSET_TYPE_TIMELINE,
        nullptr
    };

    return sAssetTypeList;
}

void TimelineIOMessageController::OnRenderThreadProcessMessage(const TimelineIOMCMessage& msg)
{
    switch(msg.GetMessageType())
    {
    case TimelineIOMCMessage::SET_BLOCKSCRIPT:
        OnSetBlockscript(msg.GetString(), msg.GetBlockGuid());
        break;
    case TimelineIOMCMessage::CLEAR_BLOCKSCRIPT:
        OnClearBlockscript(msg.GetBlockGuid());
        break;
    case TimelineIOMCMessage::SET_MASTER_BLOCKSCRIPT:
        OnSetMasterBlockscript(msg.GetString());
        break;
    case TimelineIOMCMessage::CLEAR_MASTER_BLOCKSCRIPT:
        OnClearMasterBlockscript();
        break;
    case TimelineIOMCMessage::SET_MUSIC_FILE:
        OnSetMusicFile(msg.GetString());
        break;
    case TimelineIOMCMessage::CLEAR_MUSIC_FILE:
        OnClearMusicFile();
        break;
    case TimelineIOMCMessage::SET_DEBUG_ENABLE_MUSIC:
        OnDebugEnableMusic(msg.GetIsPlayMode());
        break;
    case TimelineIOMCMessage::SET_PARAMETER:
        OnSetParameter(msg.GetTarget(), msg.GetTimelineHandle(), msg.GetLaneId(), msg.GetParameterName(), msg.GetArg(), msg.GetObserver());
        break;
    case TimelineIOMCMessage::TOGGLE_PLAY_MODE:
        OnTogglePlayMode(msg.GetIsPlayMode(), msg.GetObserver(), msg.GetTimelineHandle());
        break;
    case TimelineIOMCMessage::BLOCK_OPERATION:
        OnBlockOp(msg.GetTimelineHandle(), msg.GetBlockOp(), msg.GetBlockGuid(), msg.GetLaneId(), msg.GetArg(), msg.GetMouseClickId(), msg.GetRequiresRefocus(), msg.GetObserver());
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
                Core::ISourceCodeProxy* sourceProxy = static_cast<Core::ISourceCodeProxy*>(object);
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
    IAssetLibProxy* assetLib = mApp->GetAssetLibProxy();
    QByteArray ba = str.toLocal8Bit();
    bool success = false;
    const char* asciiPath = ba.constData();
    bool isNew = false;
    IRuntimeAssetObjectProxy* object = assetLib->LoadObject(asciiPath, &isNew);
    if (object != nullptr)
    {                                                                                                         
        if(object->GetOwnerAsset()->GetTypeDesc()->mTypeGuid == Pegasus::ASSET_TYPE_BLOCKSCRIPT.mTypeGuid)    
        {                                                                                                     
            Core::ISourceCodeProxy* sourceProxy = static_cast<Core::ISourceCodeProxy*>(object);                           
            timeline->AttachScript(sourceProxy);                                                                 
            success = true;
        }                                                                                                     
        //no need to keep a reference of this in the asset lib.                                               
        if (isNew)
        {  
            assetLib->CloseObject(object); 
        }                                                                                                     
    }                                                                                                         
    emit(NotifyMasterScriptState(success, str));
        
}

void TimelineIOMessageController::OnSetMusicFile(const QString& file)
{
    ITimelineProxy* timeline = mApp->GetTimelineManagerProxy()->GetCurrentTimeline();
    QByteArray arr = file.toLocal8Bit();
    timeline->LoadMusic(arr.data());
}

void TimelineIOMessageController::OnClearMusicFile()
{
    ITimelineProxy* timeline = mApp->GetTimelineManagerProxy()->GetCurrentTimeline();
    timeline->UnloadMusic();
}

void TimelineIOMessageController::OnDebugEnableMusic(bool enableMusic)
{
    ITimelineProxy* timeline = mApp->GetTimelineManagerProxy()->GetCurrentTimeline();
    timeline->DebugEnableSound(enableMusic);
}

void TimelineIOMessageController::OnClearMasterBlockscript()
{
    ITimelineProxy* timeline = mApp->GetTimelineManagerProxy()->GetCurrentTimeline();
    timeline->ClearScript();
    emit(NotifyMasterScriptState(false, tr("")));
}

Pegasus::Timeline::ITimelineProxy* TimelineIOMessageController::ResolveTimeline(const AssetInstanceHandle& assetHandle)
{
    Pegasus::AssetLib::IRuntimeAssetObjectProxy* timelineAssetInstance = FindInstance(assetHandle);
    if (timelineAssetInstance != nullptr)
    {
        bool isTimeline = timelineAssetInstance->GetOwnerAsset()->GetTypeDesc()->mTypeGuid == Pegasus::ASSET_TYPE_TIMELINE.mTypeGuid;
        if (isTimeline)
        {
            return static_cast<Pegasus::Timeline::ITimelineProxy*>(timelineAssetInstance);
        }
    }
    return nullptr;
}

void TimelineIOMessageController::OnSetParameter(TimelineIOMCTarget targetObject, const AssetInstanceHandle& timelineHandle, unsigned laneId, unsigned parameterName, const QVariant& paramValue, TimelineIOMessageObserver* observer)
{

    Pegasus::Timeline::ITimelineProxy* timelineProxy = ResolveTimeline(timelineHandle); 
    bool success = false;
    if (timelineProxy != nullptr)
    {
        switch (targetObject)
        {
        case TIMELINE_OBJECT:
            {
                ShadowTimelineState::PropName propName = static_cast<ShadowTimelineState::PropName>(parameterName);
                ShadowTimelineState::FlushProp(propName, paramValue, timelineProxy);
                success = true;
            }
            break;
        case LANE_OBJECT:
            {
                if (laneId < timelineProxy->GetNumLanes())
                {
                    ShadowLaneState::PropName propName = static_cast<ShadowLaneState::PropName>(parameterName);
                    Timeline::ILaneProxy* laneProxy = timelineProxy->GetLane(laneId);
                    ShadowLaneState::FlushProp(propName, paramValue, laneProxy);
                    success = true;
                }
                else
                {
                    ED_FAILSTR("Invalid lane number!"); 
                }
            }
        default:
            ED_FAILSTR("Unimplemented message for timeline object");
        }
    }
    else
    {
        ED_FAILSTR("Cannot cast to timeline type!");
    }

    // request a frame
    emit NotifyRepaintTimeline();

    if (success && observer != nullptr)
    {
        emit observer->SignalParameterUpdated(timelineHandle, laneId, (unsigned)targetObject, parameterName, paramValue);
    }
}

static QString GetJsonState(Pegasus::AssetLib::IAssetLibProxy* assetLib, Pegasus::Timeline::IBlockProxy* block)
{
    //create a fake asset (lives in memory)
    Pegasus::AssetLib::IAssetProxy* asset = assetLib->CreateAsset("", true);
    block->DumpToAsset(asset);
    char* buffer = assetLib->SerializeAsset(asset);
    QString retVal = buffer;
    assetLib->UnloadAsset(asset);
    assetLib->DestroySerializationString(buffer);
    return retVal;
}

static void ReadFromJsonState(Pegasus::AssetLib::IAssetLibProxy* assetLib, Pegasus::Timeline::IBlockProxy* block, QString& stringBuffer)
{
    QByteArray ba = stringBuffer.toLocal8Bit();
    char* buffer = ba.data();
    Pegasus::AssetLib::IAssetProxy* asset = assetLib->CreateAsset("", true);
    if (assetLib->DeserializeAsset(asset, buffer))
    {
        block->LoadFromAsset(asset);
    }
    else
    {
        ED_FAIL();
    }
    assetLib->UnloadAsset(asset);
}

static bool CreateAndInsertNewBlock(Pegasus::AssetLib::IAssetLibProxy* assetLib, Pegasus::Timeline::ITimelineProxy* timeline, const QVariant& arg, bool requiresRefocus, unsigned newGuid, TimelineIOMCBlockOpResponse& response)
{
    QVariantMap vp = arg.toMap();

    //Step 1: Parse message
    QString className = vp[QString("className")].toString();
    unsigned initialBeat = vp[QString("initialBeat")].toUInt();
    unsigned initialDuration = vp[QString("initialDuration")].toUInt();
    unsigned targetLane = vp[QString("targetLane")].toUInt();
    QByteArray classNameBA = className.toLocal8Bit();
    const char * classNameCstr = classNameBA.data();
    Pegasus::Timeline::IBlockProxy* newBlock = timeline->CreateBlock(classNameCstr);

    newBlock->OverrideGuid(newGuid);

    //Step 2: Create and insert new lane
    Pegasus::Timeline::ILaneProxy* lane = timeline->GetLane(targetLane);
    //HACK HACK HACK HACK
    //todo: really shitty algorithm to fit a block in. The function InsertBlock must be fixed.
    bool itFits = false;
    while (!itFits)
    {
        itFits = lane->IsBlockFitting(newBlock, initialBeat, initialDuration);
        if (!itFits)
        {
            initialBeat += newBlock->GetDuration();
        }
    }

    lane->InsertBlock(newBlock, initialBeat, initialDuration);

    //rerun the asset category code, while the state is read from json
    assetLib->BeginCategory(newBlock->GetAssetCategory());
    //HACK END
    newBlock->Initialize();

    //check if there is a new state set.
    QVariantMap::iterator newBlockState = vp.find(QString("jsonState"));
    if (newBlockState != vp.end())
    {
        //recover the undo state by reading the raw json back into the block
        ReadFromJsonState(assetLib, newBlock, newBlockState.value().toString());
    }
    assetLib->EndCategory();


    //Step 3: Fill in response
    ShadowLaneState newLaneState;
    newLaneState.Build(lane);
    response.newLane = targetLane;
    response.newShadowLaneState = newLaneState;
    response.blockGuid = newBlock->GetGuid();
    response.requiresRefocus = requiresRefocus;
    response.success = true;
    return  true; //always send a response
}

static bool AskOrMoveBlockPosition(Pegasus::Timeline::ITimelineProxy* timeline, TimelineIOMCBlockOp blockOp,const QVariant& arg, unsigned targetLaneId, unsigned blockGuid, TimelineIOMCBlockOpResponse& response)
{
    bool performOperationsOnTimeline = false;
    bool performQuestioningOfTimeline = false;
    switch (blockOp)
    {
    case ASK_BLOCK_POSITION:
        {
            performOperationsOnTimeline = false;
            performQuestioningOfTimeline = true;
        }
        break;
    case MOVE_BLOCK:
        {
            performOperationsOnTimeline = true;
            performQuestioningOfTimeline = true;
        }
        break;
    default:
        ED_FAIL();
        break;
    }

    Pegasus::Timeline::IBlockProxy* block = timeline->FindBlockByGuid(blockGuid);
    Pegasus::Timeline::ILaneProxy* targetLane = timeline->GetLane(targetLaneId);
    if (block != nullptr && targetLane != nullptr)
    {
        unsigned newBeat = arg.toUInt();
        Pegasus::Timeline::ILaneProxy* oldLane = block->GetLane();
        if (targetLane->IsBlockFitting(block, newBeat, block->GetDuration()))
        {
            response.success = true;
            if (oldLane != targetLane)
            {
                if (performOperationsOnTimeline)
                    oldLane->MoveBlockToLane(block, targetLane, newBeat);
                response.newLane = (int)targetLaneId;
                response.newBeat = newBeat;
            }
            else
            {
                if (performOperationsOnTimeline)
                    targetLane->SetBlockBeat(block, newBeat);
                response.newBeat = newBeat;
            }
        }
        else 
        {
            //try to fit it on the same lane.
            if (oldLane->IsBlockFitting(block, newBeat, block->GetDuration()))
            {
                if (performOperationsOnTimeline)
                    oldLane->SetBlockBeat(block, newBeat);
                response.success = true;
                response.newBeat = newBeat;
            }
        }
        return true;//send response
    }
    return false;//dont bother sending response
}


static bool DeleteTargetBlock(Pegasus::Timeline::ITimelineProxy* timeline, const QVariant& arg, TimelineIOMCBlockOpResponse& response)
{
    QVariantList blockGuidList = arg.toList();
    QSet<int> lanesFound;
    foreach (QVariant el, blockGuidList)
    {
        unsigned int blockGuid = el.toUInt();
        int laneFound = timeline->DeleteBlock(blockGuid);
        if (laneFound != -1)
        {
            ShadowLaneState newLaneState;
            newLaneState.Build(timeline->GetLane(static_cast<unsigned int>(laneFound)));
            response.lanesFound.insert(laneFound);
            response.lanesFoundState[laneFound] = newLaneState;
        }
    }
    response.success = true;
    return true; //this sends a custom response
}

static bool AskDeleteTargetBlock(Pegasus::AssetLib::IAssetLibProxy* assetLib, Pegasus::Timeline::ITimelineProxy* timeline, const QVariant& arg, TimelineIOMCBlockOpResponse& response)
{
    QVariantList blockGuidList = arg.toList();
    QVariantList jsonStates;
    foreach (QVariant blockGuid, blockGuidList)
    {
        Pegasus::Timeline::IBlockProxy* block = timeline->FindBlockByGuid(blockGuid.toUInt());
        Pegasus::PropertyGrid::IPropertyGridObjectProxy* pgrid = block->GetPropertyGridProxy();
        //TODO: here dump the state of the block property grid to a json string
        jsonStates.push_back(GetJsonState(assetLib, block));
    }
    QVariantMap newArguments;
    newArguments.insert(QString("guids"), blockGuidList);
    newArguments.insert(QString("jsonStates"), jsonStates);
    response.arg = newArguments;
    response.success = true;
    return true; //this sends a custom response
}

static bool AskForNewBlock(Pegasus::Timeline::ITimelineManagerProxy* timelineMgr, const AssetInstanceHandle& timelineHandle, const QVariant& arg, TimelineIOMCBlockOpResponse& response)
{
    response.timelineHandle = timelineHandle;
    response.blockGuid = timelineMgr->GetNextBlockGuid(); //send the prediceted block guid so the undo command knows what to delete before creation.
    response.arg = arg;
    response.success = true;
    return true; // always send a response
}
           
void TimelineIOMessageController::OnTogglePlayMode(bool isPlayMode, TimelineIOMessageObserver* observer, const AssetInstanceHandle& timelineHandle)
{
    Pegasus::Timeline::ITimelineProxy* timeline = ResolveTimeline(timelineHandle);
    if (timeline != nullptr)
    {
        if (isPlayMode)
        {
            timeline->SetPlayMode(Pegasus::Timeline::PLAYMODE_REALTIME);
            emit NotifyRedrawAllViewports(); 
            emit observer->SignalRedrawTimelineBeat(timelineHandle, timeline->GetCurrentBeat());
        }
        else
        {
            timeline->SetPlayMode(Pegasus::Timeline::PLAYMODE_STOPPED);
        }
    }
}

void  TimelineIOMessageController::RequestNewFrameInPlayMode(TimelineIOMessageObserver* observer, AssetInstanceHandle timelineHandle)
{
    Pegasus::Timeline::ITimelineProxy* timeline = ResolveTimeline(timelineHandle);
    if (timeline != nullptr)
    {
        emit NotifyRedrawAllViewports(); 
        emit observer->SignalRedrawTimelineBeat(timelineHandle, timeline->GetCurrentBeat());
    }
}

void TimelineIOMessageController::OnBlockOp(const AssetInstanceHandle& timelineHandle,TimelineIOMCBlockOp blockOp, unsigned blockGuid, unsigned targetLaneId, const QVariant& arg, unsigned mouseClickId, bool requiresRefocus, TimelineIOMessageObserver* observer)
{
    // refactor into smaller functions
    Pegasus::Timeline::ITimelineProxy* timeline = ResolveTimeline(timelineHandle);
    if (timeline != nullptr)
    {
        TimelineIOMCBlockOpResponse response;
        response.op = blockOp;
        response.blockGuid = blockGuid;
        response.timelineHandle = timelineHandle;
        response.success = false;
        response.mouseClickId = mouseClickId;
        bool sendResponse = false;

        if (blockOp == NEW_BLOCK)
        {
            sendResponse = CreateAndInsertNewBlock(mApp->GetAssetLibProxy(), timeline, arg, requiresRefocus,  blockGuid, response);
        }
        else if (blockOp == DELETE_BLOCKS)
        {
            sendResponse = DeleteTargetBlock(timeline, arg, response);
        }
        else if (blockOp == ASK_DELETE_BLOCKS)
        {
            sendResponse = AskDeleteTargetBlock(mApp->GetAssetLibProxy(), timeline, arg, response);
        }
        else if (blockOp == ASK_NEW_BLOCK)
        {
            Pegasus::Timeline::ITimelineManagerProxy* timelineMgr = mApp->GetTimelineManagerProxy();
            sendResponse = AskForNewBlock(timelineMgr, timelineHandle, arg, response);
        }
        else
        {
            sendResponse = AskOrMoveBlockPosition(timeline, blockOp, arg, targetLaneId, blockGuid, response);
        }

        if (sendResponse)
        {
            emit observer->SignalBlockOpResponse(response);
        }
    }
}

TimelineIOMessageObserver::TimelineIOMessageObserver()
{
    connect(this,SIGNAL(SignalParameterUpdated(AssetInstanceHandle, unsigned, unsigned, unsigned, QVariant)),
            this, SLOT(SlotParameterUpdated(AssetInstanceHandle, unsigned, unsigned, unsigned, QVariant)),
            Qt::QueuedConnection);
    connect(this,SIGNAL(SignalBlockOpResponse(TimelineIOMCBlockOpResponse)),
            this, SLOT(SlotBlockOpResponse(TimelineIOMCBlockOpResponse)),
            Qt::QueuedConnection);
    connect(this, SIGNAL(SignalRedrawTimelineBeat(AssetInstanceHandle,float)),
            this, SLOT(SlotRedrawTimelineBeat(AssetInstanceHandle,float)),
            Qt::QueuedConnection);
}

void TimelineIOMessageObserver::SlotParameterUpdated(AssetInstanceHandle timelineHandle, unsigned laneId, unsigned parameterTarget, unsigned parameterName, QVariant parameterValue)
{
    OnParameterUpdated(timelineHandle, laneId, parameterTarget, parameterName, parameterValue);
}

void TimelineIOMessageObserver::SlotBlockOpResponse(TimelineIOMCBlockOpResponse response)
{
    OnBlockOpResponse(response);
}

void TimelineIOMessageObserver::SlotRedrawTimelineBeat(AssetInstanceHandle timelineHandle, float beat)
{
    if (OnRedrawTimelineBeat(timelineHandle, beat))
    {
        emit RequestNewFrameInPlayMode(this, timelineHandle);
    }
}



