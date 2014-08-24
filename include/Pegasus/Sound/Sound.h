/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Sound.h
//! \author	Kevin Boulanger
//! \date	18th August 2014
//! \brief	Sound system entry point

#ifndef PEGASUS_SOUND_SOUND_H
#define PEGASUS_SOUND_SOUND_H

namespace Pegasus {
namespace Sound {


//! Initialize the sound runtime, has to be done once in the application's lifetime
void Initialize();

//! Release the sound runtime, has to be done once in the application's lifetime
void Release();

//! Load a music file, and prebuffer it so it starts playing instantly
//! \param fileName Name of the music file to load
void LoadMusic(const char * fileName);

//! Play a music file specified with \a LoadMusic
//! \note Since the file is prebuffered, music starts instantly
void PlayMusic();

//! Test if music is currently playing
//! \return True if \a PlayMusic() has been called once
bool IsPlayingMusic();

//! Return the currently playing music's position
//! \return Position of the music in milliseconds
unsigned int GetMusicPosition();

//! Sound update function, to be called once per rendered frame
void Update();


}   // namespace Sound
}   // namespace Pegasus

#endif  // PEGASUS_SOUND_SOUND_H


