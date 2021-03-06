/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	FMODSound.cpp
//! \author	Karolyn Boulanger
//! \date	18th August 2014
//! \brief	Sound system entry point, FMOD implementation

#if PEGASUS_SAPI_FMOD

#include "Pegasus/Sound/Sound.h"
#include "Pegasus/Libs/FMOD/fmod.hpp"
#include "Pegasus/Utils/String.h"
#include "Pegasus/Core/Io.h"

#if PEGASUS_COMPILER_MSVC
#if PEGASUS_DEV
#pragma comment(lib, "FMOD/Win32/fmodL_vc")
#elif PEGASUS_REL
#pragma comment(lib, "FMOD/Win32/fmod_vc")
#endif
#endif

namespace Pegasus {
namespace Sound {


//! Size of a mixing buffer for FMOD. The lower, the more CPU it takes.
//! 1024 (21.5ms) is a recommended value. For longer latencies, it is better to increase the number of buffers
static const int DSP_BUFFER_SIZE = 1024;

//! Number of mixing buffers. The higher, the more stable the sound is, but the more latency.
//! The total latency of the output is DSP_BUFFER_SIZE * DSP_NUM_BUFFERS
static const int DSP_NUM_BUFFERS = 1;

//! Estimated latency in milliseconds between the mixing of the output buffer and the actual sound output.
//! This is completely hardware dependent, and until we have a reliable way to know it,
//! we have to keep an average value
static const int OUTPUT_ESTIMATED_LATENCY = 100;
    
//----------------------------------------------------------------------------------------

//! FMOD system global object
static FMOD::System * gSystem = nullptr;

struct InternalState
{
    FMOD::Sound * music;
    FMOD::Channel * channel;
    InternalState()
    : music(nullptr), channel(nullptr)
    {
    }
};

//----------------------------------------------------------------------------------------

void InitializeSystem()
{
    PG_LOG('SOUN', "Initializing sound system using FMOD");
    PG_ASSERTSTR(gSystem == nullptr, "Trying to initialize FMOD twice");
    
    // Create the FMOD system global object
    FMOD_RESULT result = FMOD::System_Create(&gSystem);
    if (result != FMOD_OK)
    {
        PG_LOG('SOUN', "Unable to create the FMOD sound system");
        gSystem = nullptr;
        return;
    }

    // Check the version of the DLL
    unsigned int version = 0;
    result = gSystem->getVersion(&version);
    if ((result != FMOD_OK) || (version < FMOD_VERSION))
    {
        PG_LOG('SOUN', "Unable to initialize the sound system because the FMOD DLL is the wrong version");
        gSystem->release();
        gSystem = nullptr;
        return;
    }

    //! \todo Give info about the sound card, its name and the number of speakers

    // Set the DSP buffer size to control the latency of the output
    // (the default values result in the music not syncing up well with the timeline)
    result = gSystem->setDSPBufferSize(DSP_BUFFER_SIZE, DSP_NUM_BUFFERS);
    if (result != FMOD_OK)
    {
        PG_LOG('SOUN', "Unable to set the DSP buffer size of the FMOD sound system. Latency will probably be high.");
    }

    // Initialize the FMOD system
    const int MAX_NUM_CHANNELS = 32;
    result = gSystem->init(MAX_NUM_CHANNELS, FMOD_INIT_NORMAL, 0);    // Initialize FMOD.
    if (result != FMOD_OK)
    {
        PG_LOG('SOUN', "Unable to initialize the FMOD sound system");
        gSystem->release();
        gSystem = nullptr;
    }
}

//----------------------------------------------------------------------------------------

void Update()
{
    if (gSystem != nullptr)
    {
        gSystem->update();
    }
}

//----------------------------------------------------------------------------------------

void ReleaseSystem()
{
    if (gSystem != nullptr)
    {
        gSystem->release();
        gSystem = nullptr;
    }
}

//----------------------------------------------------------------------------------------

Sound::Sound(Alloc::IAllocator* allocator, Io::IOManager* ioManager, const char * fileName)
:   mAllocator(allocator),
    mIoManager(ioManager)

{
    PG_ASSERT(gSystem != nullptr)
    PG_LOG('MUSC', "Loading music file %s", fileName);

    mState = PG_NEW(mAllocator, -1, "SoundTrack", Alloc::PG_MEM_PERM) InternalState;

    PG_ASSERT(Pegasus::Utils::Strlen(fileName)+1 < MAX_SOUND_STR);
    mTargetSoundFileName[0] = '\0';
    Pegasus::Utils::Strcat(mTargetSoundFileName, fileName);
    char soundStr[MAX_SOUND_STR];
    soundStr[0] = '\0';
    Pegasus::Utils::Strcat(soundStr, ioManager->GetRoot());
    Pegasus::Utils::Strcat(soundStr, fileName);

    // Load the sound file, with or without streaming
    unsigned int fmodFlags = 0;
    fmodFlags |= FMOD_2D;

#if PEGASUS_DEV
    fmodFlags |= FMOD_LOOP_NORMAL;
#else
    fmodFlags |= FMOD_LOOP_OFF;
#endif

#if PEGASUS_SOUND_STREAM_MUSIC
    fmodFlags |= FMOD_CREATESTREAM;
#endif

    FMOD_RESULT result = gSystem->createSound(soundStr, fmodFlags, 0, &mState->music);

    if (result != FMOD_OK)
    {
        PG_LOG('MUSC', "Unable to load the music file (%s)", fileName);
        return;
    }

    // Preload the music by filling the initial buffer and set the channel to pause
    result = gSystem->playSound(mState->music, 0, true, &mState->channel);
    if (result != FMOD_OK)
    {
        PG_LOG('MUSC', "Unable to play the loaded music file (%s)", fileName);
        mState->channel = nullptr;
        return;
    }
}

//----------------------------------------------------------------------------------------

void Sound::Play(double sampleLength)
{
    if (mState->channel != nullptr)
    {
        PG_LOG('MUSC', "Starting playing the music file");

        FMOD_RESULT result = mState->channel->setPaused(false);
        unsigned long long dspClock = 0;
        unsigned long long parentDspClock = 0;
        if (sampleLength > 0)
        {
            mState->channel->getDSPClock(&dspClock,&parentDspClock);
            float channelFreq = 0.0f;
            mState->channel->getFrequency(&channelFreq);
            parentDspClock += (unsigned long long)(sampleLength * channelFreq);
        }
        mState->channel->setDelay(0,parentDspClock,false);
        Update();
        if (result != FMOD_OK)
        {
            PG_LOG('MUSC', "Unable to play the music file, the channel cannot be unpaused");
            return;
        }
    }
}

//----------------------------------------------------------------------------------------

void Sound::Pause()
{
    if (mState->channel != nullptr)
    {
        PG_LOG('MUSC', "Pausing the music.");

        FMOD_RESULT result = mState->channel->setPaused(true);
        Update();
        if (result != FMOD_OK)
        {
            PG_LOG('MUSC', "Unable to pause the music file, the channel cannot be paused");
            return;
        }
    }
}

//----------------------------------------------------------------------------------------

void Sound::SetVolume(float volume)
{
    if (mState->channel != nullptr)
    {
        FMOD_RESULT result = mState->channel->setVolume(volume);
        Update();
        if (result != FMOD_OK)
        {
            PG_LOG('MUSC', "Unable to set the volume of the music file.");
            return;
        }
    }
}

//----------------------------------------------------------------------------------------

float Sound::GetVolume() const
{
    float volume = 0.0f;
    if (mState->channel != nullptr)
    {
        FMOD_RESULT result = mState->channel->getVolume(&volume);
        Update();
        if (result != FMOD_OK)
        {
            PG_LOG('MUSC', "Unable to get the volume of the music file.");
        }
    }
    return volume;
}

//----------------------------------------------------------------------------------------

#if PEGASUS_ENABLE_PROXIES
void Sound::SetMute(bool mute)
{
    if (mState->channel != nullptr)
    {
        FMOD_RESULT result = mState->channel->setMute(mute);
        Update();
        if (result != FMOD_OK)
        {
            PG_LOG('MUSC', "Unable to mute the music.");
            return;
        }
    }
}
#endif

//----------------------------------------------------------------------------------------

bool Sound::IsPlayingMusic() const
{
    if (mState->channel != nullptr)
    {
        bool paused = false;
        FMOD_RESULT result = mState->channel->getPaused(&paused);
        return (result == FMOD_OK) && !paused;
    }
    else
    {
        return false;
    }
}

//----------------------------------------------------------------------------------------

unsigned int Sound::GetPosition()
{
    if (mState->channel != nullptr)
    {
        unsigned int musicPosition = 0;
        FMOD_RESULT result = mState->channel->getPosition(&musicPosition, FMOD_TIMEUNIT_MS);
        if (result == FMOD_OK)
        {
            if (musicPosition > OUTPUT_ESTIMATED_LATENCY)
            {
                return musicPosition - OUTPUT_ESTIMATED_LATENCY;
            }
        }
    }

    return 0;
}

void Sound::SetPosition(unsigned int position)
{
    if (mState->channel != nullptr)
    {
        unsigned int musicPosition = 0;
        mState->channel->setPosition(position, FMOD_TIMEUNIT_MS);
    }
}

Sound::~Sound()
{
    if (mState->channel != nullptr)
    {
        mState->channel->stop();
        mState->channel = nullptr;
    }

    if (mState->music != nullptr)
    {
        mState->music->release();
        mState->music = nullptr;
    }


    PG_DELETE(mAllocator, mState);

}


}   // namespace Sound
}   // namespace Pegasus

#else
PEGASUS_AVOID_EMPTY_FILE_WARNING
#endif  // PEGASUS_SAPI_FMOD

