/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Sound.h
//! \author	Karolyn Boulanger
//! \date	18th August 2014
//! \brief	Sound system entry point

#ifndef PEGASUS_SOUND_SOUND_H
#define PEGASUS_SOUND_SOUND_H

namespace Pegasus
{
namespace Io
{
    class IOManager;
}

namespace Alloc
{
    class IAllocator;
}
}

namespace Pegasus {
namespace Sound {

class Sound
{
public:
    //! Load a music file. Prebuffers it so it starts playing instantly
    //! \param allocator used for internal memory.
    //! \param ioManager wrapper of filesystem to load music file.
    //! \param fileName Name of the music file to load
   Sound(Alloc::IAllocator* allocator, Io::IOManager* ioManager, const char* targetSoundFile);

   ~Sound();

   //! plays the sound for a given amount of time. If sampleLength is negative, it will play indefinitely.
   //! \param sampleLength the amount of time to play. It will pause then automatically. If negative then plays indefinitely. It assumes the file being played is at 44khz.
   void Play(double sampleLength = -1.0);

   //! Pauses the current sound playing
   void Pause();

   //! Seeks the position of the current sound
   //! \param seekSpot - current system time of system position.
   void SetPosition(unsigned int seekSpot);

   //! Gets the current position
   //! \return the current position of this track
   unsigned int GetPosition();

   //! \return true if the music is playing, false otherwise
   bool IsPlayingMusic() const;

private:
    Alloc::IAllocator* mAllocator;
    Io::IOManager* mIoManager;
    struct InternalState* mState;
};

//! Initialize the sound runtime, has to be done once in the application's lifetime
void InitializeSystem();

//! Release the sound runtime, has to be done once in the application's lifetime
void ReleaseSystem();

//! Sound update function, to be called once per rendered frame
void Update();


}   // namespace Sound
}   // namespace Pegasus

#endif  // PEGASUS_SOUND_SOUND_H


