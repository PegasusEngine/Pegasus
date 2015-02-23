/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineProxy.h
//! \author	Kevin Boulanger
//! \date	07th November 2013
//! \brief	Proxy object, used by the editor to interact with the timeline

#ifndef PEGASUS_TIMELINE_PROXY_TIMELINEPROXY_H
#define PEGASUS_TIMELINE_PROXY_TIMELINEPROXY_H

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/Timeline/Shared/ITimelineProxy.h"
#include "Pegasus/Timeline/TimelineScript.h"
#include "Pegasus/Utils/Vector.h"

namespace Pegasus {
    namespace Timeline {
        class Timeline;
    }

    namespace AssetLib {
        class IAssetProxy;
    }

    namespace Core {
        class ISourceCodeProxy;
    }
}

namespace Pegasus {
namespace Timeline {


//! Proxy object, used by the editor to interact with the timeline
class TimelineProxy : public ITimelineProxy
{
public:

    // Constructor
    //! \param timeline Proxied timeline object, cannot be nullptr
    TimelineProxy(Timeline * timeline);

    //! Destructor
    virtual ~TimelineProxy();

    //! Get the timeline associated with the proxy
    //! \return Timeline associated with the proxy (!= nullptr)
    inline Timeline * GetTimeline() const { return mTimeline; }


    //! Get the list of registered block names (class and editor string)
    //! \param classNames Allocated 2D array of Timeline::MAX_NUM_REGISTERED_BLOCKS strings
    //!                   of length Timeline::MAX_BLOCK_CLASS_NAME_LENGTH + 1,
    //!                   containing the resulting class names
    //! \param editorStrings Allocated 2D array of Timeline::MAX_NUM_REGISTERED_BLOCKS strings
    //!                   of length Timeline::MAX_BLOCK_EDITOR_STRING_LENGTH + 1,
    //!                   containing the resulting editor strings (can be empty)
    //! \return Number of registered blocks (<= Timeline::MAX_NUM_REGISTERED_BLOCKS)
    virtual unsigned int GetRegisteredBlockNames(char classNames   [MAX_NUM_REGISTERED_BLOCKS][MAX_BLOCK_CLASS_NAME_LENGTH    + 1],
                                                 char editorStrings[MAX_NUM_REGISTERED_BLOCKS][MAX_BLOCK_EDITOR_STRING_LENGTH + 1]) const;


    //! Get the number of ticks per beat
    //! \return Number of ticks per beat, power of 2, >= 16, 128 by default
    virtual unsigned int GetNumTicksPerBeat() const;

    //! Get the number of ticks per beat in float format
    //! \return Number of ticks per beat in float format, power of 2, >= 16.0f, 128.0f by default
    virtual float GetNumTicksPerBeatFloat() const;

    //! Get the reciprocal of the number of ticks per beat
    //! \return Reciprocal of the number of ticks per beat
    virtual float GetRcpNumTicksPerBeat() const;


    //! Set the speed of the timeline in beats per minute
    //! \param bpm Beats per minute (30.0f <= bpm <= 500.0f)
    virtual void SetBeatsPerMinute(float bpm);

    //! Get the speed of the timeline in beats per minute
    //! \return Beats per minute (30.0f <= bpm <= 500.0f)
    virtual float GetBeatsPerMinute() const;


    //! Set the length of the timeline
    //! \warning If the length is not enough to contain all existing blocks, the blocks after the end line will be deleted
    //! \param numBeats Number of beats defining the new length of the timeline (> 0)
    virtual void SetNumBeats(unsigned int numBeats);

    //! Get the length of the timeline
    //! \return Number of beats defining the length of the timeline (>= 1)
    virtual unsigned int GetNumBeats() const;


    //! Create a new lane
    //! \return Proxy interface for a timeline lane, nullptr if the number of lanes is MAX_NUM_LANES
    virtual ILaneProxy * CreateLane();

    //! Get the current number of lanes
    //! \return Number of lanes (>= 1)
    virtual unsigned int GetNumLanes() const;

    //! Get one of the lanes
    //! \param laneIndex Index of the lane (< GetNumLanes())
    //! \return Pointer to the lane proxy, nullptr in case of error
    virtual ILaneProxy * GetLane(unsigned int laneIndex) const;


    //! Set the play mode of the timeline
    //! \param playMode New play mode of the timeline (PLAYMODE_xxx constant)
    virtual void SetPlayMode(PlayMode playMode);

    //! Update the current state of the timeline based on the play mode and the current time
    virtual void Update();

    //! Set the current beat of the timeline
    //! \param beat Current beat, measured in ticks, can have fractional part
    virtual void SetCurrentBeat(float beat);

    //! Get the current beat of the timeline
    //! \return Current beat, measured in ticks, can have fractional part
    virtual float GetCurrentBeat() const;

    //! Returns the number of sources this manager has
    //! \return source count in the application
    virtual int GetSourceCount() const;

    //! Gets the program based on its id. The id range goes from 0 to ProgramCount
    //! \return program proxy
    virtual Core::ISourceCodeProxy* GetSource(int id);

    //! Sets the event listener to be used for the shader
    //! \param event listener reference
    virtual void RegisterEventListener(Pegasus::Core::CompilerEvents::ICompilerEventListener * eventListener);

    //! loads a script from a file path
    //! \return the script proxy dispatched
    virtual Core::ISourceCodeProxy* OpenScript(const char* path);

    //! creates a script from an asset
    //! \return the source code proxy
    virtual Core::ISourceCodeProxy* OpenScript(AssetLib::IAssetProxy* asset);

    //! closes a script from editing
    virtual void CloseScript(Core::ISourceCodeProxy* script);

    //! \return true if its a blockscript, false otherwise
    virtual bool IsTimelineScript(const AssetLib::IAssetProxy* asset) const;

    //------------------------------------------------------------------------------------
    
private:

    int FindOpenedScript(TimelineScriptIn script);

    //! Proxied timeline object
    Timeline * const mTimeline;
    
    //list of opened scripts
    Utils::Vector<TimelineScriptRef> mOpenedScripts;
};


}   // namespace Timeline
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_TIMELINE_PROXY_TIMELINEPROXY_H
