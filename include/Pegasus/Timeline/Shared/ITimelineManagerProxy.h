/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	ITimelineManagerProxy.h
//! \author	Kevin Boulanger
//! \date	07th November 2013
//! \brief	Proxy interface, used by the editor to interact with the timeline

#ifndef PEGASUS_TIMELINEMANAGER_SHARED_ITIMELINEPROXY_H
#define PEGASUS_TIMELINEMANAGER_SHARED_ITIMELINEPROXY_H

#if PEGASUS_ENABLE_PROXIES

#include "Pegasus/Timeline/Shared/TimelineDefs.h"
#include "Pegasus/Core/Shared/ISourceCodeManagerProxy.h"

namespace Pegasus {
    namespace Timeline {
        class ITimelineProxy;
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


//! Proxy interface, used by the editor to interact with the timeline
class ITimelineManagerProxy : public Core::ISourceCodeManagerProxy
{
public:

    //! Destructor
    virtual ~ITimelineManagerProxy() {};


    //! Get the list of registered block names (class and editor string)
    //! \param classNames Allocated 2D array of Timeline::MAX_NUM_REGISTERED_BLOCKS strings
    //!                   of length Timeline::MAX_BLOCK_CLASS_NAME_LENGTH + 1,
    //!                   containing the resulting class names
    //! \param editorStrings Allocated 2D array of Timeline::MAX_NUM_REGISTERED_BLOCKS strings
    //!                   of length Timeline::MAX_BLOCK_EDITOR_STRING_LENGTH + 1,
    //!                   containing the resulting editor strings (can be empty)
    //! \return Number of registered blocks (<= Timeline::MAX_NUM_REGISTERED_BLOCKS)
    virtual unsigned int GetRegisteredBlockNames(char classNames   [MAX_NUM_REGISTERED_BLOCKS][MAX_BLOCK_CLASS_NAME_LENGTH    + 1],
                                                 char editorStrings[MAX_NUM_REGISTERED_BLOCKS][MAX_BLOCK_EDITOR_STRING_LENGTH + 1]) const = 0;


    //! Gets the current timeline of the app
    //! \return the current timeline of the app
    virtual ITimelineProxy* GetCurrentTimeline() const = 0;

    //! Updates the clock of the app based on the timeline state
    virtual void Update() = 0;

    //! loads a script from a file path
    //! \return the script proxy dispatched
    virtual Core::ISourceCodeProxy* OpenScript(const char* path) = 0;

    //! creates a script from an asset
    //! \return the source code proxy
    virtual Core::ISourceCodeProxy* OpenScript(AssetLib::IAssetProxy* asset) = 0;

    //! closes a script from editing
    virtual void CloseScript(Core::ISourceCodeProxy* script) = 0;

    //! \return true if its a blockscript, false otherwise
    virtual bool IsTimelineScript(const AssetLib::IAssetProxy* asset) const = 0;
};


}   // namespace Timeline
}   // namespace Pegasus

#endif  // PEGASUS_ENABLE_PROXIES
#endif  // PEGASUS_TIMELINE_SHARED_ITIMELINEPROXY_H
