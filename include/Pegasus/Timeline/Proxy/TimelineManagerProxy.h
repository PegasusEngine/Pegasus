/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineProxy.h
//! \author	Karolyn Boulanger
//! \date	07th November 2013
//! \brief	Proxy object, used by the editor to interact with the timeline

#ifndef PEGASUS_TIMELINEMANAGER_PROXY_TIMELINEPROXY_H
#define PEGASUS_TIMELINEMANAGER_PROXY_TIMELINEPROXY_H

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/Timeline/Shared/ITimelineManagerProxy.h"
#include "Pegasus/Timeline/TimelineSource.h"
#include "Pegasus/Utils/Vector.h"

namespace Pegasus {
    namespace Timeline {
        class TimelineManager;
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
class TimelineManagerProxy : public ITimelineManagerProxy
{
public:

    // Constructor
    //! \param timeline Proxied timeline object, cannot be nullptr
    TimelineManagerProxy(TimelineManager * timeline);

    //! Destructor
    virtual ~TimelineManagerProxy();

    //! Get the timeline associated with the proxy
    //! \return Timeline associated with the proxy (!= nullptr)
    inline TimelineManager * GetTimelineManager() const { return mTimelineManager; }


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


    //! Gets the current timeline of the app
    //! \return the current timeline of the app
    virtual ITimelineProxy* GetCurrentTimeline() const;

    //! Updates the clock of the app based on the timeline state
    virtual void Update();

    //! Sets the event listener to be used for the shader
    //! \param event listener reference
    virtual void RegisterEventListener(Pegasus::Core::CompilerEvents::ICompilerEventListener * eventListener);
    
    //! Returns the next block's guid. The editor uses this function to predict the next block allocated guids, this is so the undo / redo stack can work properly and preemptively create a delete comand.
    //! \return the next block's guid
    virtual unsigned int GetNextBlockGuid() const;
    
private:

    int FindOpenedScript(TimelineSourceIn script);

    //! Proxied timeline object
    TimelineManager * const mTimelineManager;
};


}   // namespace Timeline
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_TIMELINE_PROXY_TIMELINEPROXY_H
