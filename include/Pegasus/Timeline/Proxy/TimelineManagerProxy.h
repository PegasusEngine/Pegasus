/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineProxy.h
//! \author	Kevin Boulanger
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

    //! Returns the number of sources this manager has
    //! \return source count in the application
    virtual int GetSourceCount() const;

    //! Gets the program based on its id. The id range goes from 0 to ProgramCount
    //! \return program proxy
    virtual Core::ISourceCodeProxy* GetSource(int id);

    //! Updates the clock of the app based on the timeline state
    virtual void Update();

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
    
private:

    int FindOpenedScript(TimelineSourceIn script);

    //! Proxied timeline object
    TimelineManager * const mTimelineManager;
    
    //list of opened scripts
    Utils::Vector<TimelineSourceRef> mOpenedScripts;
};


}   // namespace Timeline
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_TIMELINE_PROXY_TIMELINEPROXY_H
