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

namespace Pegasus {
namespace Timeline {


const float Timeline::INVALID_BEAT = -1.0f;

//----------------------------------------------------------------------------------------

Timeline::Timeline(Alloc::IAllocator * allocator, Core::IApplicationContext* appContext)
:   mAllocator(allocator)
,   mAppContext(appContext)
,   mNumTicksPerBeat(128)
,   mNumTicksPerBeatFloat(128.0f)
,   mRcpNumTicksPerBeat(1.0f / 128.0f)
,   mBeatsPerMinute(138.0f)
,   mNumBeats(128)
,   mNumLanes(0)
,   mPlayMode(PLAYMODE_REALTIME)
,   mCurrentBeat(INVALID_BEAT)
,   mStartPegasusTime(0.0)
,   mSyncedToMusic(false)
#if PEGASUS_ENABLE_PROXIES
,   mProxy(this)
,   mRequiresStartTimeComputation(false)
#endif
{
    PG_ASSERTSTR(allocator != nullptr, "Invalid allocator given to the timeline object");
    PG_ASSERTSTR(appContext != nullptr, "Invalid application context given to the timeline object");

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

void Timeline::Clear()
{
    // Delete the existing lanes
    for (unsigned int lane = 0; lane < mNumLanes; ++lane)
    {
        PG_DELETE(mAllocator, mLanes[lane]);
    }
    mNumLanes = 0;

    // Create a default lane
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
    
void Timeline::Update(unsigned int musicPosition)
{
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
    }
}

//----------------------------------------------------------------------------------------

void Timeline::Render(Wnd::Window * window)
{
    if (window != nullptr)
    {
        unsigned int l;

        // Update the content of each lane from top to bottom
        for (l = 0; l < mNumLanes; ++l)
        {
            Lane * lane = GetLane(l);
            if (lane != nullptr)
            {
                lane->Update(mCurrentBeat, window);
            }
        }

        // Render the content of each lane from top to bottom
        //! \todo Add support for render passes
        for (l = 0; l < mNumLanes; ++l)
        {
            Lane * lane = GetLane(l);
            if (lane != nullptr)
            {
                lane->Render(mCurrentBeat, window);
            }
        }
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
                break;

            case PLAYMODE_STOPPED:
                PG_LOG('TMLN', "Switched to stopped mode for the timeline");
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

#if PEGASUS_ENABLE_PROXIES
    // If the play mode is real-time, the Pegasus start time needs to be recomputed
    if (mPlayMode == PLAYMODE_REALTIME)
    {
        mRequiresStartTimeComputation = true;
    }
#endif  // PEGASUS_ENABLE_PROXIES
}


bool Timeline::OnReadAsset(Pegasus::AssetLib::AssetLib* lib, AssetLib::Asset* asset)
{
    AssetLib::Object* root = asset->Root();
    if (root == nullptr)
    {
        return false;
    }

    int typeId = root->FindString("type");
    if (typeId == -1 || Utils::Strcmp("Timeline", root->GetString(typeId))) 
    {
        return false;
    }

    int beatsId = root->FindInt("num-beats");
    int tpbId = root->FindInt("ticks-per-beat");
    int pbmId = root->FindInt("beats-per-minute-bin");
    int lanesId = root->FindArray("lanes");

    if (beatsId == -1 || tpbId == -1 || pbmId == -1 || lanesId == -1)
    {
        return false;
    }
    
    mNumBeats = root->GetInt(beatsId);
    
    SetNumTicksPerBeat( root->GetInt(tpbId) );

    union fi {
        float f;
        int   i;
    } fie;
    fie.i = root->GetInt(pbmId);
    SetBeatsPerMinute(fie.f);

    AssetLib::Array* lanesArr = root->GetArray(lanesId);
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

    return true;
}

void Timeline::OnWriteAsset(Pegasus::AssetLib::AssetLib* lib, AssetLib::Asset* asset)
{
    AssetLib::Object* root = asset->NewObject();
    asset->SetRootObject(root);
    root->AddString("type", "Timeline");
    root->AddInt("num-beats", mNumBeats);
    
    root->AddInt("ticks-per-beat", mNumTicksPerBeat);

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
    
    
}


}   // namespace Timeline
}   // namespace Pegasus

