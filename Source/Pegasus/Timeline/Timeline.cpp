/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Timeline.cpp
//! \author	refactored by Kleber Garcia (original from Karolyn Boulanger)
//! \date	July 18, 2015
//! \brief	Timeline container, for lanes, and functions for playback

#include "Pegasus/Timeline/Timeline.h"
#include "Pegasus/Timeline/TimelineManager.h"
#include "Pegasus/Timeline/Block.h"
#include "Pegasus/Timeline/Lane.h"
#include "Pegasus/AssetLib/AssetLib.h"
#include "Pegasus/AssetLib/Asset.h"
#include "Pegasus/AssetLib/ASTree.h"
#include "Pegasus/Core/Time.h"
#include "Pegasus/Math/Scalar.h"
#include "Pegasus/Utils/String.h"
#include "Pegasus/Utils/Memcpy.h"
#include "Pegasus/Core/IApplicationContext.h"
#include "Pegasus/Window/Window.h"
#include "Pegasus/Utils/Memset.h"
#include "Pegasus/Sound/Sound.h"

namespace Pegasus {
namespace Timeline {


const float Timeline::INVALID_BEAT = -1.0f;

//----------------------------------------------------------------------------------------

Timeline::Timeline(Alloc::IAllocator * allocator, Core::IApplicationContext* appContext)
:   
    Core::RefCounted(allocator)
,   AssetLib::RuntimeAssetObject(this)
,   mAllocator(allocator)
,   mAppContext(appContext)
,   mNumTicksPerBeat(128)
,   mNumTicksPerBeatFloat(128.0f)
,   mRcpNumTicksPerBeat(1.0f / 128.0f)
,   mBeatsPerMinute(138.0f)
,   mNumBeats(128)
,   mNumLanes(0)
,   mPlayMode(PLAYMODE_REALTIME)
,   mCurrentBeat(0.0)
,   mStartPegasusTime(0.0)
,   mSyncedToMusic(false)
,   mMusic(nullptr)
,   mGlobalCache(allocator)
,   mScriptRunner(allocator, appContext, &mPropertyGrid
#if PEGASUS_ASSETLIB_ENABLE_CATEGORIES
    ,   &mCategory
#endif
    )
#if PEGASUS_ENABLE_PROXIES
,   mProxy(this)
,   mRequiresStartTimeComputation(false)
#endif
{
    PG_ASSERTSTR(allocator != nullptr, "Invalid allocator given to the timeline object");
    PG_ASSERTSTR(appContext != nullptr, "Invalid application context given to the timeline object");

    mScriptRunner.SetGlobalCache(&mGlobalCache, true);

    // Create the initial default lane
    Clear();
}

//----------------------------------------------------------------------------------------

Timeline::~Timeline()
{
    for (unsigned int lane = 0; lane < mNumLanes; ++lane)
    {
        PG_DELETE(mAllocator, mLanes[lane]);
    }
}

//----------------------------------------------------------------------------------------

Block * Timeline::CreateBlock(const char * className)
{
    return mAppContext->GetTimelineManager()->CreateBlock(className);
}

//----------------------------------------------------------------------------------------

#if PEGASUS_ENABLE_PROXIES
int Timeline::DeleteBlock(unsigned int blockGuid)
{
    for (unsigned int laneIdx = 0; laneIdx < mNumLanes; ++laneIdx)
    {
        Lane* l = mLanes[laneIdx];
        int foundBlockIdx = l->FindBlockByGuid(blockGuid);
        if (foundBlockIdx != Lane::INVALID_RECORD_INDEX)
        {
            //get target block
            Block* b = l->FindBlockByIndex(foundBlockIdx);

            //destroy all rendering resources
            b->Shutdown();

            //remove from timeline
            l->RemoveBlock(foundBlockIdx);

#if PEGASUS_ASSETLIB_ENABLE_CATEGORIES
            //Remove asset category to prevent a dangling pointer.
            //TODO: make asset categories an autoref.
            mAppContext->GetAssetLib()->RemoveAssetCategory(b->GetAssetCategory());
#endif

            //completely delete memory
            mAppContext->GetTimelineManager()->DestroyBlock(b);

            //return lane it was found
            return static_cast<int>(laneIdx);
        }
    }
    return -1;
}

#endif

//----------------------------------------------------------------------------------------

void Timeline::InternalClear()
{
    for (unsigned int lane = 0; lane < mNumLanes; ++lane)
    {
        PG_DELETE(mAllocator, mLanes[lane]);
    }
    mNumLanes = 0;

#if PEGASUS_ENABLE_PROXIES
    Utils::Memset8(mWindowIsInitialized, 0, sizeof(mWindowIsInitialized));
#endif
}

//----------------------------------------------------------------------------------------

void Timeline::Clear()
{
    // Delete the existing lanes
    InternalClear();

    //Create default lane
    CreateLane();
}

//----------------------------------------------------------------------------------------

void Timeline::SetNumTicksPerBeat(unsigned int numTicksPerBeat)
{
    if (numTicksPerBeat < 16)
    {
        PG_FAILSTR("Invalid number of ticks per beat (%u), it must be >= 16", numTicksPerBeat);
        numTicksPerBeat = 16;
    }
    else if (!Math::IsPowerOf2((Math::PUInt32)numTicksPerBeat))
    {
        PG_FAILSTR("Invalid number of ticks per beat (%u), it must be a power of 2", numTicksPerBeat);
        numTicksPerBeat = Math::NextPowerOf2((Math::PUInt32)numTicksPerBeat);
    }
    else
    {
        mNumTicksPerBeat = numTicksPerBeat;
    }

    PG_LOG('TMLN', "Setting the number of ticks per beat to %u", mNumTicksPerBeat);

    mNumTicksPerBeatFloat = static_cast<float>(mNumTicksPerBeat);
    mRcpNumTicksPerBeat = 1.0f / mNumTicksPerBeatFloat;
}

//----------------------------------------------------------------------------------------

void Timeline::SetBeatsPerMinute(float bpm)
{
    if (bpm < 30.0f)
    {
        PG_FAILSTR("Invalid speed for the timeline (%.2f BPM). It should be >= 30.0f", bpm);
        mBeatsPerMinute = 30.0f;
    }
    else if (bpm > 500.0f)
    {
        PG_FAILSTR("Invalid speed for the timeline (%.2f BPM). It should be <= 500.0f", bpm);
        mBeatsPerMinute = 500.0f;
    }
    else
    {
        mBeatsPerMinute = bpm;
    }

    PG_LOG('TMLN', "Setting the tempo to %.2f BPM", mBeatsPerMinute);

#if PEGASUS_ENABLE_PROXIES
    // If the play mode is real-time, the Pegasus start time needs to be recomputed
    // to avoid the cursor jumping all over the place
    if (mPlayMode == PLAYMODE_REALTIME)
    {
        mRequiresStartTimeComputation = true;
    }
#endif  // PEGASUS_ENABLE_PROXIES
}

//----------------------------------------------------------------------------------------

void Timeline::SetNumBeats(unsigned int numBeats)
{
    if (numBeats >= 1)
    {
        //! \todo If the length is not enough to contain all existing blocks, the blocks after the end line will be deleted

        mNumBeats = numBeats;
    }
    else
    {
        PG_FAILSTR("Invalid number of beats (%d) to define the length of the timeline");
    }
}

//----------------------------------------------------------------------------------------

Lane * Timeline::CreateLane()
{
    PG_LOG('TMLN', "Creating a new lane (index %d)", mNumLanes);

    if (mNumLanes < MAX_NUM_LANES)
    {
        mLanes[mNumLanes] = PG_NEW(mAllocator, -1, "Timeline::Lane", Alloc::PG_MEM_PERM) Lane(mAllocator, this);
        ++mNumLanes;

        return mLanes[mNumLanes - 1];
    }
    else
    {
        PG_FAILSTR("Unable to create a new lane, the maximum number of lane has been reached (%d)", MAX_NUM_LANES);
        return nullptr;
    }
}

//----------------------------------------------------------------------------------------

Lane * Timeline::GetLane(unsigned int laneIndex) const
{
    if (laneIndex < mNumLanes)
    {
        PG_ASSERTSTR(mLanes[laneIndex] != nullptr, "Invalid lane in the timeline (index %d)", laneIndex);
        return mLanes[laneIndex];
    }
    else
    {
        PG_FAILSTR("Invalid lane index (%d), it should be < %d", laneIndex, mNumLanes);
        return nullptr;
    }
}

//----------------------------------------------------------------------------------------

void Timeline::InitializeBlocks()
{
    mScriptRunner.InitializeScript();

    for (unsigned int l = 0; l < mNumLanes; ++l)
    {
        Lane * lane = GetLane(l);
        if (lane != nullptr)
        {
            lane->InitializeBlocks();
        }
    }
}

//----------------------------------------------------------------------------------------

void Timeline::ShutdownBlocks()
{
    for (unsigned int l = 0; l < mNumLanes; ++l)
    {
        Lane * lane = GetLane(l);
        if (lane != nullptr)
        {
            lane->UninitializeBlocks();
        }
    }
}

//----------------------------------------------------------------------------------------
    
void Timeline::Update()
{
    unsigned int musicPosition = 0;
    if (mMusic != nullptr)
    {
        musicPosition = mMusic->GetPosition();
    }
    if ((mCurrentBeat == INVALID_BEAT) || (mCurrentBeat < 0.0f))
    {
        mCurrentBeat = 0.0f;
        Core::UpdatePegasusTime();
        mStartPegasusTime = Core::GetPegasusTime();
    }
    else
    {
        if (mPlayMode == PLAYMODE_REALTIME)
        {
            if (mMusic != nullptr && !mMusic->IsPlayingMusic())
            {
                mMusic->Play();
            }
            // Update the internal clock for the next frame
            Core::UpdatePegasusTime();

            // Attempt to synchronize the timeline with the music
            // (do not do it on the first frame to avoid potential invalid synchronization).
            if (!mSyncedToMusic && (musicPosition > 0))
            {
                // Current time that we should have got from the timer for the current beat and the current tempo
                double requiredCurrentTime = static_cast<double>(musicPosition) * 0.001;

                // Move the start Pegasus time to simulate a non-stop execution of the timeline until the current beat
                mStartPegasusTime = static_cast<float>(Core::GetPegasusTime() - requiredCurrentTime);

                mSyncedToMusic = true;
            }

#if PEGASUS_ENABLE_PROXIES
            // If the start time needs to be recomputed (typically when the cursor position is changed,
            // or when the play mode has just been set to real-time)
            if (mRequiresStartTimeComputation)
            {
                // Current time that we should have got from the timer for the current beat and the current tempo
                double requiredCurrentTime = static_cast<double>(mCurrentBeat / (mBeatsPerMinute * (1.0f / 60.0f)));

                // Move the start Pegasus time to simulate a non-stop execution of the timeline until the current beat
                mStartPegasusTime = static_cast<float>(Core::GetPegasusTime() - requiredCurrentTime);

                mRequiresStartTimeComputation = false;
            }
#endif  // PEGASUS_ENABLE_PROXIES

            // Update the current beat from the current time
            const double currentTime = Core::GetPegasusTime() - mStartPegasusTime;
            mCurrentBeat = static_cast<float>(currentTime * (mBeatsPerMinute * (1.0f / 60.0f)));

#if PEGASUS_ENABLE_PROXIES
            // Fix precision issues when starting play mode in the editor
            if (mCurrentBeat < 0.0f)
            {
                mCurrentBeat = 0.0f;
            }
#endif  // PEGASUS_ENABLE_PROXIES

        }

        UpdateInfo updateInfo(mCurrentBeat);

        updateInfo.relativeBeat = updateInfo.beat;

        mScriptRunner.CallUpdate(updateInfo);
        // Update the content of each lane from top to bottom
        for (unsigned int l = 0; l < mNumLanes; ++l)
        {
            Lane * lane = GetLane(l);
            if (lane != nullptr)
            {
                lane->Update(updateInfo);
            }
        }

    }
}

//----------------------------------------------------------------------------------------

void Timeline::Render(int windowIndex, Wnd::Window* window)
{
    if (window != nullptr)
    {
        Render::BeginMarker("Timeline");
#if PEGASUS_ENABLE_PROXIES
        //lazy initialization in case we missed the initial call, because of live editing.
        //In this context, live editing means the user creating a new timeline from scratch from the editor.
        if (!mWindowIsInitialized[windowIndex])
        {
            OnWindowCreated(windowIndex);
        }
#endif
        RenderInfo renderInfo(
           mCurrentBeat
          ,windowIndex
          ,static_cast<int>(window->GetWidth())
          ,static_cast<int>(window->GetHeight())
          ,static_cast<float>(window->GetWidth())
          ,static_cast<float>(window->GetHeight())
          ,window->GetRatio()
          ,window->GetRatioInv()
        );

        Render::BeginMarker("MasterScript");
        renderInfo.relativeBeat = renderInfo.beat;
        // Render the content of each lane from top to bottom
        mScriptRunner.CallRender(renderInfo);
        Render::EndMarker();

        //! \todo Add support for render passes
        for (unsigned int l = 0; l < mNumLanes; ++l)
        {
            Lane * lane = GetLane(l);
            if (lane != nullptr)
            {
                lane->Render(renderInfo);
            }
        }
        Render::EndMarker();
    }
    else
    {
        PG_FAILSTR("Invalid window given to the timeline for rendering");
    }
}

//----------------------------------------------------------------------------------------

void Timeline::SetPlayMode(PlayMode playMode)
{
    if (playMode < NUM_PLAYMODES)
    {
        switch (playMode)
        {
            case PLAYMODE_REALTIME:
                PG_LOG('TMLN', "Switched to real-time mode for the timeline");
#if PEGASUS_ENABLE_PROXIES
                // The Pegasus start time needs to be recomputed to avoid the cursor jumping all over the place
                mRequiresStartTimeComputation = true;
#endif 
                if (mMusic != nullptr)
                {
                    mMusic->Play();
                }
                break;

            case PLAYMODE_STOPPED:
                PG_LOG('TMLN', "Switched to stopped mode for the timeline");
                if (mMusic != nullptr)
                {
                    mMusic->Pause();
                }
                break;

            default:
                PG_LOG('TMLN', "Switched to an unknown mode for the timeline");
        }

        mPlayMode = playMode;
    }
    else
    {
        PG_FAILSTR("Invalid play mode (%d) for the timeline. The maximum value is %d", playMode, NUM_PLAYMODES - 1);
    }
}

//----------------------------------------------------------------------------------------

void Timeline::SetCurrentBeat(float beat)
{
    //! \todo Implement proper behavior (more safety, handles play modes)
    mCurrentBeat = beat;

    PG_LOG('TMLN', "Set the current beat of the timeline to %f", mCurrentBeat);

    if (mMusic != nullptr)
    {
        float musicPosition = beat /(mBeatsPerMinute * 1.0f/60.0f);
        //compute new time
        unsigned int requiredcurrenttime = static_cast<unsigned int>(musicPosition* 1000.0);
        mMusic->SetPosition(requiredcurrenttime);
        mMusic->Play(mPlayMode == PLAYMODE_REALTIME ? -1.0 : 0.2);
        Sound::Update();
    }
#if PEGASUS_ENABLE_PROXIES
    // If the play mode is real-time, the Pegasus start time needs to be recomputed
    if (mPlayMode == PLAYMODE_REALTIME)
    {
        mRequiresStartTimeComputation = true;
    }
#endif  // PEGASUS_ENABLE_PROXIES

}

void Timeline::AttachScript(TimelineScriptInOut script)
{
    mScriptRunner.AttachScript(script);
}

void Timeline::ShutdownScript()
{
    mScriptRunner.ShutdownScript();
}

TimelineScriptReturn Timeline::GetScript()
{
    return mScriptRunner.GetScript();
}

void Timeline::LoadMusic(const char* path)
{
    if (mMusic != nullptr)
    {
        UnloadMusic();
    }

    mMusic = PG_NEW(mAllocator, -1, "TimelineSound", Alloc::PG_MEM_PERM) Pegasus::Sound::Sound(
       mAllocator, mAppContext->GetIOManager(), path
    );
}

void Timeline::UnloadMusic()
{
    PG_DELETE(mAllocator, mMusic);
    mMusic = nullptr;
}

float Timeline::GetVolume() const
{
    if (mMusic != nullptr)
    {
        return mMusic->GetVolume();
    }
    else
    {
        return 1.0f;
    }
}

void Timeline::SetVolume(float volume)
{
    if (mMusic != nullptr)
    {
        mMusic->SetVolume(volume);
    }
}

void Timeline::OnWindowCreated(int windowIndex)
{
#if PEGASUS_ENABLE_PROXIES
    mWindowIsInitialized[windowIndex] = true;
#endif
    mScriptRunner.CallWindowCreated(windowIndex);

    for (unsigned int l = 0; l < mNumLanes; ++l)
    {
        Lane * lane = GetLane(l);
        if (lane != nullptr)
        {
            lane->OnWindowCreated(windowIndex);
        }
    }
}

void Timeline::OnWindowDestroyed(int windowIndex)
{
    for (unsigned int l = 0; l < mNumLanes; ++l)
    {
        Lane * lane = GetLane(l);
        if (lane != nullptr)
        {
            lane->OnWindowCreated(windowIndex);
        }
    }
    mScriptRunner.CallWindowDestroyed(windowIndex);
#if PEGASUS_ENABLE_PROXIES
    mWindowIsInitialized[windowIndex] = false;
#endif
}

#if PEGASUS_ENABLE_PROXIES
void Timeline::DebugEnableSound(bool enableSound)
{
    if (mMusic != nullptr)
    {
        mMusic->SetMute(!enableSound);
    }
}
#endif

bool Timeline::OnReadAsset(Pegasus::AssetLib::AssetLib* lib, const AssetLib::Asset* asset)
{
    InternalClear();
    const AssetLib::Object* root = asset->Root();
    if (root == nullptr)
    {
        return false;
    }

    int beatsId = root->FindInt("num-beats");
    int tpbId = root->FindInt("ticks-per-beat");
    int pbmId = root->FindInt("beats-per-minute-bin");
    int lanesId = root->FindArray("lanes");
    int musicTrackId = root->FindString("music-track");
    int musicTrackVolumeId = root->FindFloat("music-track-vol");

    if (beatsId == -1 || tpbId == -1 || pbmId == -1)
    {
        return false;
    }
    
    if (musicTrackId != -1)
    {
        const char* musicTrackPath = root->GetString(musicTrackId);
        LoadMusic(musicTrackPath);
    }

    if (musicTrackVolumeId != -1 && mMusic != nullptr)
    {
        float musicVolume = root->GetFloat(musicTrackVolumeId);
        if (mMusic != nullptr)
        {
            mMusic->SetVolume(musicVolume);
        }
    }

    mNumBeats = root->GetInt(beatsId);
    
    SetNumTicksPerBeat( root->GetInt(tpbId) );

    union fi {
        float f;
        int   i;
    } fie;
    fie.i = root->GetInt(pbmId);
    SetBeatsPerMinute(fie.f);

    bool createDefaultLane = false;

    if (lanesId != -1)
    {
        AssetLib::Array* lanesArr = root->GetArray(lanesId);
        if (lanesArr->GetSize() > 0)
        {
            if (lanesArr->GetType() != AssetLib::Array::AS_TYPE_OBJECT)
            {
                return false;
            }

            for (int i = 0; i < lanesArr->GetSize(); ++i)
            {
                AssetLib::Object* laneObj = lanesArr->GetElement(i).o;
                Lane* l = CreateLane();
                if (!l->OnReadObject(lib, asset, laneObj))
                {
                    return false;
                }
            } 
        }
        else
        {
            createDefaultLane = true;
        }
    }
    else
    {
        createDefaultLane = true;
    }

    if (createDefaultLane)
    {
        CreateLane();
    }

    int assetIndex = root->FindAsset("master-script");
    if (assetIndex != -1)
    {
        AssetLib::RuntimeAssetObjectRef assetScriptFile = root->GetAsset(assetIndex);
        if (assetScriptFile->GetOwnerAsset()->GetTypeDesc()->mTypeGuid == Pegasus::ASSET_TYPE_BLOCKSCRIPT.mTypeGuid)
        {
            TimelineScriptRef loadedScript = assetScriptFile;
            AttachScript(loadedScript);
        }
    }

    int propertyGridObjectId = root->FindObject("properties");
    if (propertyGridObjectId != -1)
    {
        AssetLib::Object* propGridObjObj = root->GetObject(propertyGridObjectId);
        mPropertyGrid.ReadFromObject(asset, propGridObjObj);
    }
    return true;
}

void Timeline::OnWriteAsset(Pegasus::AssetLib::AssetLib* lib, AssetLib::Asset* asset)
{
    AssetLib::Object* root = asset->Root();

    if (GetScript() != nullptr)
    {
        root->AddAsset("master-script", GetScript());
    }
    
    root->AddInt("num-beats", mNumBeats);
    
    root->AddInt("ticks-per-beat", mNumTicksPerBeat);

    if (mMusic != nullptr)
    {
        root->AddString("music-track", mMusic->GetSoundName());
        root->AddFloat("music-track-vol", mMusic->GetVolume());
    }

    union fi{
        int i;
        float f;
    } fie;
    fie.f = mBeatsPerMinute;
    root->AddInt("beats-per-minute-bin", fie.i);
    root->AddFloat("beats-per-minute-float", fie.f);

    AssetLib::Array* laneArray = asset->NewArray();
    laneArray->CommitType(AssetLib::Array::AS_TYPE_OBJECT);
    root->AddArray("lanes", laneArray);

    for (unsigned i = 0; i < mNumLanes; ++i)
    {
        AssetLib::Object* laneObj = asset->NewObject();
        mLanes[i]->OnWriteObject(lib, asset, laneObj);
        AssetLib::Array::Element e;
        e.o = laneObj;
        laneArray->PushElement(e);
    }

    AssetLib::Object* propertyObject = asset->NewObject();
    mPropertyGrid.WriteToObject(asset, propertyObject);

    root->AddObject("properties", propertyObject);
    
    
}


}   // namespace Timeline
}   // namespace Pegasus

