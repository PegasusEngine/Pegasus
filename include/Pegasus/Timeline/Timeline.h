/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	Timeline.h
//! \author	Kevin Boulanger
//! \date	07th November 2013
//! \brief	Timeline management, manages a set of blocks stored in lanes to sequence demo rendering

#ifndef PEGASUS_TIMELINE_TIMELINE_H
#define PEGASUS_TIMELINE_TIMELINE_H

#include "Pegasus/Timeline/Shared/TimelineDefs.h"
#include "Pegasus/Timeline/Proxy/TimelineProxy.h"
#include "Pegasus/Timeline/TimelineScript.h"
#include "Pegasus/Timeline/ScriptTracker.h"

namespace Pegasus {

    namespace Core {
        namespace CompilerEvents {
            class ICompilerEventListener;
        }
    }

    namespace Timeline {
        class Block;
        class Lane;
    }

    namespace Wnd {
        class Window;
        class IWindowContext;
    }
    
    namespace AssetLib {
        class Asset;
    }
}

namespace Pegasus {
namespace Timeline {


//! Macro to register a Pegasus-side timeline block
//! \note Requires the external timeline variable to be defined, a pointer to the application timeline
//! \param className Name of the class to register, without quotes
#if PEGASUS_ENABLE_PROXIES

#define REGISTER_BASE_TIMELINE_BLOCK(className)                                                        \
    timeline->RegisterBlock(#className, className::GetStaticEditorString(), className::CreateBlock);   \

#else

#define REGISTER_BASE_TIMELINE_BLOCK(className)                    \
    timeline->RegisterBlock(#className, className::CreateBlock);   \

#endif  // PEGASUS_ENABLE_PROXIES
    

//! Macro to register a timeline block inside the Application class
//! \param className Name of the class to register, without quotes
#if PEGASUS_ENABLE_PROXIES

#define REGISTER_TIMELINE_BLOCK(className)                                                                  \
    GetTimeline()->RegisterBlock(#className, className::GetStaticEditorString(), className::CreateBlock);   \

#else

#define REGISTER_TIMELINE_BLOCK(className)                              \
    GetTimeline()->RegisterBlock(#className, className::CreateBlock);   \

#endif  // PEGASUS_ENABLE_PROXIES

//----------------------------------------------------------------------------------------

//! Timeline management, manages a set of blocks stored in lanes to sequence demo rendering
class Timeline
{
public:

    //! Constructor
    //! \param allocator Allocator used for all timeline allocations
    //! \param appContext Application context, providing access to the global managers
    Timeline(Alloc::IAllocator * allocator, Wnd::IWindowContext * appContext);

    //! Destructor
    virtual ~Timeline();

    //! Get the allocator used for all timeline allocations
    inline Alloc::IAllocator * GetAllocator() const { return mAllocator; }


    //! Creation function type, to be defined once per timeline block class
    //! \param allocator Allocator used for block internal data
    //! \param appContext Application context, providing access to the global managers
    typedef Block * (* CreateBlockFunc)(Alloc::IAllocator * allocator,
                                        Wnd::IWindowContext * appContext);

#if PEGASUS_ENABLE_PROXIES

    //! Register a block class, to be called before any timeline block of this type is created
    //! \param className String of the block class (maximum length MAX_BLOCK_CLASS_NAME_LENGTH)
    //! \param editorString String for the editor (more readable than the class name), can be empty but != nullptr
    //! \param createBlockFunc Pointer to the block member function that instantiates the block
    //! \warning If the number of registered block classes reaches MAX_NUM_REGISTERED_BLOCKS,
    //!          an assertion is thrown and the class does not get registered.
    //!          If that happens, increase the value of MAX_NUM_REGISTERED_BLOCKS
    //! \note Use the \a REGISTER_TIMELINE_BLOCK macro as convenience
    void RegisterBlock(const char * className, const char * editorString, CreateBlockFunc createBlockFunc);

    //! Get the list of registered block names (class and editor string)
    //! \param classNames Allocated 2D array of MAX_NUM_REGISTERED_BLOCKS strings
    //!                   of length MAX_BLOCK_CLASS_NAME_LENGTH + 1,
    //!                   containing the resulting class names
    //! \param editorStrings Allocated 2D array of MAX_NUM_REGISTERED_BLOCKS strings
    //!                   of length MAX_BLOCK_EDITOR_STRING_LENGTH + 1,
    //!                   containing the resulting editor strings (can be empty)
    //! \return Number of registered blocks (<= MAX_NUM_REGISTERED_BLOCKS)
    unsigned int GetRegisteredBlockNames(char classNames   [MAX_NUM_REGISTERED_BLOCKS][MAX_BLOCK_CLASS_NAME_LENGTH    + 1],
                                         char editorStrings[MAX_NUM_REGISTERED_BLOCKS][MAX_BLOCK_EDITOR_STRING_LENGTH + 1]) const;

#else

    //! Register a block class, to be called before any timeline block of this type is created
    //! \param className String of the block class (maximum length MAX_BLOCK_CLASS_NAME_LENGTH)
    //! \param createBlockFunc Pointer to the block member function that instantiates the block
    //! \warning If the number of registered block classes reaches MAX_NUM_REGISTERED_BLOCKS,
    //!          an assertion is thrown and the class does not get registered.
    //!          If that happens, increase the value of MAX_NUM_REGISTERED_BLOCKS
    //! \note Use the \a REGISTER_TIMELINE_BLOCK macro as convenience
    void RegisterBlock(const char * className, CreateBlockFunc createBlockFunc);

#endif  // PEGASUS_ENABLE_PROXIES

    //! Create a block by class name
    //! \param className Name of the block class to instantiate
    //! \return Pointer to the created block, nullptr if an error occurred
    Block * CreateBlock(const char * className);


    //! Clear the entire timeline and create a default lane
    void Clear();


    //! Set the number of ticks per beat
    //! \param numTicksPerBeat New number of ticks per beat, power of 2, >= 16
    void SetNumTicksPerBeat(unsigned int numTicksPerBeat);

    //! Get the number of ticks per beat
    //! \return Number of ticks per beat, power of 2, >= 16, 128 by default
    inline unsigned int GetNumTicksPerBeat() const { return mNumTicksPerBeat; }

    //! Get the number of ticks per beat in float format
    //! \return Number of ticks per beat in float format, power of 2, >= 16.0f, 128.0f by default
    inline float GetNumTicksPerBeatFloat() const { return mNumTicksPerBeatFloat; }

    //! Get the reciprocal of the number of ticks per beat
    //! \return Reciprocal of the number of ticks per beat
    inline float GetRcpNumTicksPerBeat() const { return mRcpNumTicksPerBeat; }


    //! Set the speed of the timeline in beats per minute
    //! \param bpm Beats per minute (30.0f <= bpm <= 500.0f)
    void SetBeatsPerMinute(float bpm);

    //! Get the speed of the timeline in beats per minute
    //! \return Beats per minute (30.0f <= bpm <= 500.0f)
    inline float GetBeatsPerMinute() const { return mBeatsPerMinute; }


    //! Set the length of the timeline
    //! \warning If the length is not enough to contain all existing blocks, the blocks after the end line will be deleted
    //! \param numBeats Number of beats defining the new length of the timeline (> 0)
    void SetNumBeats(unsigned int numBeats);

    //! Get the length of the timeline
    //! \return Number of beats defining the length of the timeline (>= 1)
    inline unsigned int GetNumBeats() const { return mNumBeats; }


    //! Create a new lane
    //! \return New lane, nullptr if the number of lanes is MAX_NUM_LANES
    Lane * CreateLane();

    //! Get the current number of lanes
    //! \return Number of lanes (>= 1)
    inline unsigned int GetNumLanes() const { return mNumLanes; }

    //! Get one of the lanes
    //! \param laneIndex Index of the lane (< GetNumLanes())
    //! \return Pointer to the lane, nullptr in case of error
    Lane * GetLane(unsigned int laneIndex) const;


    // Tell all the blocks of the timeline to initialize their content (calling their Initialize() function)
    void InitializeBlocks();


    //! Set the play mode of the timeline
    //! \param playMode New play mode of the timeline (PLAYMODE_xxx constant)
    void SetPlayMode(PlayMode playMode);

    //! Update the current state of the timeline based on the play mode and the current time
    //! \param musicPosition Currently heard position of the music (in milliseconds), 0 if unknown
    void Update(unsigned int musicPosition = 0);

    //! Render the content of the timeline for the given window
    //! \param window Window in which the timeline is being rendered
    //! \todo That dependency is ugly. Find a way to remove that dependency
    void Render(Wnd::Window * window);

    //! Set the current beat of the timeline
    //! \param beat Current beat, in number of ticks, can have fractional part
    void SetCurrentBeat(float beat);

    //! Get the current beat of the timeline
    //! \return Current beat, in number of ticks, can have fractional part
    inline float GetCurrentBeat() const { return mCurrentBeat; }

#if PEGASUS_USE_GRAPH_EVENTS
    //! Register a compiler event listener for blocks that have scripts and get compiled.
    //! \param eventListener - the listener to events during compilation
    void RegisterEventListener(Pegasus::Core::CompilerEvents::ICompilerEventListener * eventListener);

    //! Gets the event listener registered
    //! \return the event listener registered.
    Pegasus::Core::CompilerEvents::ICompilerEventListener* GetEventListener() const { return mEventListener; }
#endif

    //! Test wether this asset is a script or not
    //! \return true if its a blockscript false otherwise
    bool IsTimelineScript(const AssetLib::Asset* asset) const;

    //! Loads a script from a file
    //! \param the file of the script name
    //! \return the timeline script reference
    TimelineScriptRef LoadScript(const char* scriptName);

    //! Creates a script from an asset
    //! \param the asset to use to create this script
    //! \return the timeline script reference
    TimelineScriptRef CreateScript(AssetLib::Asset* asset);

    //! Gets the script tracker registered
    //! \return the script tracker
    ScriptTracker* GetScriptTracker() { return &mScriptTracker; }


#if PEGASUS_ENABLE_PROXIES

    //! Get the proxy associated with the timeline
    //! \return Proxy associated with the timeline
    //@{
    inline TimelineProxy * GetProxy() { return &mProxy; }
    inline const TimelineProxy * GetProxy() const { return &mProxy; }
    //@}

#endif  // PEGASUS_ENABLE_PROXIES

    //------------------------------------------------------------------------------------

private:

    // The timeline cannot be copied
    PG_DISABLE_COPY(Timeline)

    //! Find a registered block by name
    //! \param className Name of the class of the block to find
    //! \return Index of the block in the \a mRegisteredBlocks array if found,
    //!         mNumRegisteredBlocks if not found
    unsigned int GetRegisteredBlockIndex(const char * className) const;


    //! Allocator used for all timeline allocations
    Alloc::IAllocator * mAllocator;

    //! Application context, providing access to the global managers
    Wnd::IWindowContext * mAppContext;

    //! Structure describing one registered block class
    struct BlockEntry
    {
        char className[MAX_BLOCK_CLASS_NAME_LENGTH + 1];        //!< Name of the class
#if PEGASUS_ENABLE_PROXIES
        char editorString[MAX_BLOCK_EDITOR_STRING_LENGTH + 1];  //!< Editor string (more readable than the class name)
#endif
        CreateBlockFunc createBlockFunc;                        //!< Factory function of the block

        BlockEntry() { className[0] = '\0'; createBlockFunc = nullptr; }    //!< Default constructor
    };

    //! List of registered blocks, only the first mNumRegisteredBlocks ones are valid
    BlockEntry mRegisteredBlocks[MAX_NUM_REGISTERED_BLOCKS];

    //! Number of currently registered blocks (<= MAX_NUM_REGISTERED_BLOCKS)
    unsigned int mNumRegisteredBlocks;


#if PEGASUS_ENABLE_PROXIES

    //! Proxy associated with the timeline
    TimelineProxy mProxy;

    //! True when the real-time play mode has been selected, to update mStartPegasusTime
    //! on the next call to Update() (used by the editor to set the current beat)
    bool mRequiresStartTimeComputation;

#endif  // PEGASUS_ENABLE_PROXIES


    //! Number of ticks per beat, power of 2, >= 16, 128 by default
    unsigned int mNumTicksPerBeat;

    //! Number of ticks per beat in float format, power of 2, >= 16.0f, 128.0f by default
    float mNumTicksPerBeatFloat;

    //! Reciprocal of the number of ticks per beat
    float mRcpNumTicksPerBeat;


    //! Speed of the timeline in beats per minute (30.0f <= bpm <= 500.0f)
    float mBeatsPerMinute;

    //! Number of beats defining the length of the timeline (>= 1)
    unsigned int mNumBeats;

    //! Set of lanes, only the first mNumLanes are defined
    Lane * mLanes[MAX_NUM_LANES];

    //! Number of used lanes (<= MAX_NUM_LANES)
    unsigned int mNumLanes;

    //! Current play mode of the timeline (PLAYMODE_xxx constant, PLAYMODE_REALTIME by default)
    PlayMode mPlayMode;

    //! Current beat of the timeline, in number of ticks, can have fractional part.
    //! INVALID_BEAT before the first call to Update()
    float mCurrentBeat;

    //! Invalid beat, used to detect the Update() function has not been called yet
    static const float INVALID_BEAT;

    //! Pegasus time returned by \a GetPegasusTime() when the timeline was started being played
    double mStartPegasusTime;

    //! True if the start time has been modified to synchronize the beat of the timeline with the music
    bool mSyncedToMusic;

#if PEGASUS_USE_GRAPH_EVENTS
    //! Reference to the event listener
    Core::CompilerEvents::ICompilerEventListener* mEventListener;
#endif
    
    ScriptTracker mScriptTracker;
};


}   // namespace Timeline
}   // namespace Pegasus

#endif  // PEGASUS_TIMELINE_TIMELINE_H
