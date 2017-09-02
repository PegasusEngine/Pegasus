/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineProxy.h
//! \author	refactored by Kleber Garcia (original from Karolyn Boulanger)
//! \date	July 18, 2015
//! \brief	Timeline container, for lanes, and functions for playback

#ifndef PEGASUS_TIMELINE_IMP_PROXY_TIMELINEPROXY_H
#define PEGASUS_TIMELINE_IMP_PROXY_TIMELINEPROXY_H

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/Timeline/Shared/ITimelineProxy.h"
#include "Pegasus/Timeline/TimelineSource.h"
#include "Pegasus/Utils/Vector.h"
#include "Pegasus/Timeline/Proxy/BlockProxy.h"

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

    //! Create a new block. Pass the class name of such block.
    //! If the block does not exist then this function returns null.
    //! \param the class name of the new block.
    //! \return the block proxy to use.
    virtual IBlockProxy* CreateBlock(const char* className);

    //! Deletes a block and shuts it down.
    //! \param blockGuid the block guid to delete.
    //! \return int indicating the lane it was found. -1 otherwise
    virtual int DeleteBlock(unsigned int blockGuid);

    //! Set the current beat of the timeline
    //! \param beat Current beat, measured in ticks, can have fractional part
    virtual void SetCurrentBeat(float beat);

    //! Get the current beat of the timeline
    //! \return Current beat, measured in ticks, can have fractional part
    virtual float GetCurrentBeat() const;

    //! Gets a block from a guid. 
    //! \param blockGuid the guid to query this block from
    //! \return the block proxy if found, nullptr otherwise
    virtual IBlockProxy* FindBlockByGuid(unsigned blockGuid);

    //! Gets the script proxy if available, null if not available
    //! \return the source code proxy, null if it does not exist.
    virtual Core::ISourceCodeProxy* GetScript() const;

    //! Sets a script proxy as a timeline element.
    //! \param code - the code to attach
    virtual void AttachScript(Core::ISourceCodeProxy* code);

    //! Clears blockscript if there is one.
    virtual void ClearScript();

    //! Sets a music file, were the string is the path.
    //! \param musicFileName - the path
    virtual void LoadMusic(const char* musicFileName);

    //! Removes any sound / music file.
    virtual void UnloadMusic();

    //! Returns volume scale (0.0 to Max scale).
    //! \returns volume scale
    virtual float GetVolume() const;

    //! Gets volume scale 
    //! \param volume scale
    virtual void SetVolume(float volume);

    //! true if you want to unmute.
    //! \param enableSound - true if sound wants to get enabled.
    virtual void DebugEnableSound(bool enableSound);

    //! If this asset runtime object has a property attached, the return it.
    //! \return the property grid object of this proxy. If it doesn't exist then it returns null.
    virtual PropertyGrid::IPropertyGridObjectProxy* GetPropertyGrid() { return &mPropertyGridDecorator; }
   
protected:
    virtual AssetLib::IRuntimeAssetObjectProxy* GetDecoratedObject() const;

private:
    //! Proxied timeline object
    Timeline * const mTimeline;

    // property grid decorator to pass arguments from editor to runtime for live editing
    PropertyFlusherPropertyGridObjectDecorator mPropertyGridDecorator;

};


}   // namespace Timeline
}   // namespace Pegasus

#endif  
#endif  
