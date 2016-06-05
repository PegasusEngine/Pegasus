/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineScriptRunner.h
//! \author	Kleber Garcia
//! \date	24th May 2016
//! \brief	Script runner, with state and coordination of Vm / compilation

#ifndef SCRIPT_RUNNER_H
#define SCRIPT_RUNNER_H

#include "Pegasus/Timeline/Shared/TimelineDefs.h"
#include "Pegasus/Timeline/TimelineScript.h"
#include "Pegasus/Core/IApplicationContext.h"
#include "Pegasus/AssetLib/RuntimeAssetObject.h"
#include "Pegasus/AssetLib/Category.h"
#include "Pegasus/PropertyGrid/PropertyGridObject.h"
#include "Pegasus/Timeline/BlockRuntimeScriptListener.h"

namespace Pegasus {

#if PEGASUS_ASSETLIB_ENABLE_CATEGORIES
    namespace AssetLib {
        class Category;
    }
#endif

    namespace Timeline {
        class Lane;
    }

    namespace Wnd {
        class Window;
    }

    namespace BlockScript {
        class BlockScript;
        class BlockScriptManager;
    }

    namespace PropertyGrid {
        class PropertyGridObject;
    }
}

namespace Pegasus {
namespace Timeline {


//! 
class TimelineScriptRunner
{
public:

    //! Constructor
    //! \param allocator Allocator used for all timeline allocations
    //! \param appContext Application context, providing access to the global managers
    TimelineScriptRunner(Alloc::IAllocator * allocator, Core::IApplicationContext* appContext, PropertyGrid::PropertyGridObject* propGrid
#if PEGASUS_ASSETLIB_ENABLE_CATEGORIES
        ,AssetLib::Category* category
#endif
    );

    //! Destructor
    virtual ~TimelineScriptRunner();

    //! Attempts to open and compile a script. True if success, false otherwise.
    //! \param script reference to attach
    void AttachScript(TimelineScriptInOut script);

    //! Returns the script of this block, null if none is attached.
    //! \return the script object, null if not attached
    TimelineScriptReturn GetScript() { return mTimelineScript; }

    //! Attempts to shutdown a script if it has been opened
    void ShutdownScript();

    //! Attempts to initialize a script
    void InitializeScript();

    //! Uninitializes a script
    void UninitializeScript();

    //! \return true if a script is present, false otherwise
    bool HasScript() const { return mTimelineScript != nullptr; }

    //! Call when we want to make a potential update to the current state.
    //! We will flush the internal state of the object property in the current index to the 
    //! Corresponding internal block script virtual machine state.
    //! \param index - index to update
    void NotifyInternalObjectPropertyUpdated(unsigned int index);

    //! Update the content of the block, called once at the beginning of each rendered frame
    //! \param beat Current beat relative to the beginning of the block,
    //!             can have fractional part (>= 0.0f)
    //! \param window Window in which the lane is being rendered
    //! \todo That dependency is ugly. Find a way to remove that dependency
    virtual void CallUpdate(float beat, Wnd::Window * window);

    //! Render the content of the block
    //! \param beat Current beat relative to the beginning of the block,
    //!             can have fractional part (>= 0.0f)
    //! \param window Window in which the lane is being rendered
    //! \todo That dependency is ugly. Find a way to remove that dependency
    virtual void CallRender(float beat, Wnd::Window * window);

    //Gets the property grid that this runner is using to dispatch externs
    PropertyGrid::PropertyGridObject* GetPropertyGrid() { return mPropertyGrid; }

private:

    //! Allocator used for all timeline allocations
    Alloc::IAllocator * mAllocator;

    //! Application context, providing access to the global managers
    Core::IApplicationContext* mAppContext;

    //! script helper object
    TimelineScriptRef mTimelineScript;

    //! virtual machine state
    BlockScript::BsVmState* mVmState;
    
    //! Property grid reference that holds properties passed down to the state. 
    PropertyGrid::PropertyGridObject* mPropertyGrid;

    //! object listener that manipulates a block state on any script runtime events
    BlockRuntimeScriptListener mRuntimeListener;

    //! version of the script, used for global variable initialization
    int mScriptVersion;

#if PEGASUS_ASSETLIB_ENABLE_CATEGORIES
    AssetLib::Category* mCategory;
#endif

#if PEGASUS_ENABLE_PROXIES

    class BlockScriptObserver : public ITimelineObserver
    {
    public:
        BlockScriptObserver(TimelineScriptRunner * runner) : mRunner(runner) {}
        virtual ~BlockScriptObserver(){}
        virtual void OnCompilationBegin(); 
        virtual void OnCompilationEnd(); 
    private:
        TimelineScriptRunner* mRunner;
    } mBlockScriptObserver;

#endif  // PEGASUS_ENABLE_PROXIES
};


}   // namespace Timeline
}   // namespace Pegasus

#endif
