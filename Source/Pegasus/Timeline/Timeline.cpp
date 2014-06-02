/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Timeline.cpp
//! \author	Kevin Boulanger
//! \date	07th November 2013
//! \brief	Timeline management, manages a set of blocks stored in lanes to sequence demo rendering

#include "Pegasus/Timeline/Timeline.h"
#include "Pegasus/Timeline/Lane.h"
#include "Pegasus/Core/Time.h"
#include "Pegasus/Math/Scalar.h"
#include "Pegasus/Utils/String.h"
#include "Pegasus/Utils/Memcpy.h"

#if PEGASUS_ENABLE_PROXIES
#include "Pegasus/Timeline/TimelineProxy.h"
#endif  // PEGASUS_ENABLE_PROXIES

#include <string.h>

namespace Pegasus {
namespace Timeline {


const float Timeline::INVALID_BEAT = -1.0f;

//----------------------------------------------------------------------------------------

Timeline::Timeline(Alloc::IAllocator * allocator, Wnd::IWindowContext * appContext)
:   mAllocator(allocator),
    mAppContext(appContext),
    mNumRegisteredBlocks(0),
    mNumTicksPerBeat(128),
    mNumTicksPerBeatFloat(128.0f),
    mRcpNumTicksPerBeat(1.0f / 128.0f),
    mBeatsPerMinute(120.0f),
    mNumBeats(128),
    mNumLanes(0),
    mPlayMode(PLAYMODE_REALTIME),
    mCurrentBeat(INVALID_BEAT),
    mStartPegasusTime(0.0)
{
    PG_ASSERTSTR(allocator != nullptr, "Invalid allocator given to the timeline object");
    PG_ASSERTSTR(appContext != nullptr, "Invalid application context given to the timeline object");

#if PEGASUS_ENABLE_PROXIES
    //! Create the proxy associated with the timeline
    mProxy = PG_NEW(allocator, -1, "Timeline::mProxy", Pegasus::Alloc::PG_MEM_PERM) TimelineProxy(this);

    mRequiresStartTimeComputation = false;
#endif  // PEGASUS_ENABLE_PROXIES

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

#if PEGASUS_ENABLE_PROXIES
    //! Destroy the proxy associated with the timeline
    PG_DELETE(mAllocator, mProxy);
#endif
}

//----------------------------------------------------------------------------------------

#if PEGASUS_ENABLE_PROXIES
void Timeline::RegisterBlock(const char * className, const char * editorString, CreateBlockFunc createBlockFunc)
#else
void Timeline::RegisterBlock(const char * className, CreateBlockFunc createBlockFunc)
#endif
{
    if (className == nullptr)
    {
        PG_FAILSTR("Trying to register a block class but the name is undefined");
        return;
    }
#if PEGASUS_ENABLE_PROXIES
    if (editorString == nullptr)
    {
        PG_FAILSTR("Trying to register a block class but the editor string is undefined");
        return;
    }
#endif


    if (Pegasus::Utils::Strlen(className) < 4)
    {
        PG_FAILSTR("Trying to register a block class but the name (%s) is too short", className);
        return;
    }

    if (createBlockFunc == nullptr)
    {
        PG_FAILSTR("Trying to register a block class but the factory function is undefined");
        return;
    }

    if (mNumRegisteredBlocks >= MAX_NUM_REGISTERED_BLOCKS)
    {
        PG_FAILSTR("Unable to register a block, the maximum number of blocks (%d) has been reached", MAX_NUM_REGISTERED_BLOCKS);
        return;
    }

    // After the parameters have been validated, register the class
    BlockEntry & entry = mRegisteredBlocks[mNumRegisteredBlocks];
#if PEGASUS_COMPILER_MSVC

    strncpy_s(entry.className, MAX_BLOCK_CLASS_NAME_LENGTH + 1,
              className, MAX_BLOCK_CLASS_NAME_LENGTH + 1);
#if PEGASUS_ENABLE_PROXIES
    strncpy_s(entry.editorString, MAX_BLOCK_EDITOR_STRING_LENGTH + 1,
              editorString, MAX_BLOCK_EDITOR_STRING_LENGTH + 1);
#endif

#else

    entry.className[MAX_BLOCK_CLASS_NAME_LENGTH] = '\0';
    strncpy(entry.className, className, MAX_BLOCK_CLASS_NAME_LENGTH);
#if PEGASUS_ENABLE_PROXIES
    strncpy(entry.editorString, editorString, MAX_BLOCK_EDITOR_STRING_LENGTH);
#endif

#endif  // PEGASUS_COMPILER_MSVC

    entry.createBlockFunc = createBlockFunc;
    ++mNumRegisteredBlocks;
}

//----------------------------------------------------------------------------------------

#if PEGASUS_ENABLE_PROXIES

unsigned int Timeline::GetRegisteredBlockNames(char classNames   [MAX_NUM_REGISTERED_BLOCKS][MAX_BLOCK_CLASS_NAME_LENGTH + 1],
                                               char editorStrings[MAX_NUM_REGISTERED_BLOCKS][MAX_BLOCK_CLASS_NAME_LENGTH + 1]) const
{
    if ((classNames != nullptr) && (editorStrings != nullptr))
    {
        for (unsigned int b = 0; b < mNumRegisteredBlocks; ++b)
        {
            if ((classNames[b] != nullptr) && (editorStrings[b] != nullptr))
            {
                const BlockEntry & entry = mRegisteredBlocks[b];
#if PEGASUS_COMPILER_MSVC
                strncpy_s(classNames[b], MAX_BLOCK_CLASS_NAME_LENGTH + 1,
                          entry.className, MAX_BLOCK_CLASS_NAME_LENGTH + 1);
                strncpy_s(editorStrings[b], MAX_BLOCK_EDITOR_STRING_LENGTH + 1,
                          entry.editorString, MAX_BLOCK_EDITOR_STRING_LENGTH + 1);
#else
                classNames[b][MAX_BLOCK_CLASS_NAME_LENGTH] = '\0';
                strncpy(classNames[b], entry.className, MAX_BLOCK_CLASS_NAME_LENGTH);
                strncpy(editorStrings[b], entry.editorString, MAX_BLOCK_EDITOR_STRING_LENGTH);
#endif  // PEGASUS_COMPILER_MSVC
            }
            else
            {
                PG_FAILSTR("Invalid output array when getting the list of registered timeline blocks");
                return 0;
            }
        }

        return mNumRegisteredBlocks;
    }
    else
    {
        PG_FAILSTR("Invalid output array when getting the list of registered timeline blocks");
        return 0;
    }
}

#endif  // PEGASUS_ENABLE_PROXIES

//----------------------------------------------------------------------------------------

Block * Timeline::CreateBlock(const char * className)
{
    const unsigned int registeredBlockIndex = GetRegisteredBlockIndex(className);
    if (registeredBlockIndex < mNumRegisteredBlocks)
    {
        BlockEntry & entry = mRegisteredBlocks[registeredBlockIndex];
        PG_ASSERT(entry.createBlockFunc != nullptr);
        return entry.createBlockFunc(mAllocator, mAppContext);
    }
    else
    {
        if (className == nullptr)
        {
            PG_FAILSTR("Unable to create a block because the provided class name is invalid");
        }
        else
        {
            PG_FAILSTR("Unable to create the block of class %s because it has not been registered", className);
        }

        return nullptr;
    }
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
    
void Timeline::Update()
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
            Core::UpdatePegasusTime();

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
        // Render the content of each lane from top to bottom
        for (unsigned int l = 0; l < mNumLanes; ++l)
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

//----------------------------------------------------------------------------------------

unsigned int Timeline::GetRegisteredBlockIndex(const char * className) const
{
    if (className == nullptr)
    {
        PG_FAILSTR("Trying to find a block class by name but the name is undefined");
        return mNumRegisteredBlocks;
    }

    unsigned int index;
    for (index = 0; index < mNumRegisteredBlocks; ++index)
    {
        if (Pegasus::Utils::Strcmp(mRegisteredBlocks[index].className, className) == 0)
        {
            // Block found
            return index;
        }
    }

    // Block not found
    PG_ASSERT(index == mNumRegisteredBlocks);
    return mNumRegisteredBlocks;
}


}   // namespace Timeline
}   // namespace Pegasus
