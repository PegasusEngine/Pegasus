/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineScript.cpp
//! \author	Karolyn Boulanger
//! \date	1st November 2014
//! \brief	Script helper for scripting callbacks

#ifndef SCRIPT_HELPER_H
#define SCRIPT_HELPER_H

#include "Pegasus/Timeline/TimelineSource.h"
#include "Pegasus/BlockScript/BlockScript.h"
#include "Pegasus/BlockScript/IBlockScriptCompilerListener.h"
#include "Pegasus/Graph/Shared/GraphEventDefs.h"
#include "Pegasus/Core/Shared/CompilerEvents.h"
#include "Pegasus/Core/Io.h"
#include "Pegasus/Core/Ref.h"
#include "Pegasus/AssetLib/RuntimeAssetObject.h"

#if PEGASUS_ENABLE_PROXIES
#include "Pegasus/Timeline/Proxy/TimelineScriptProxy.h"
#endif

#define MAX_SCRIPT_NAME 64


//! fwd dec
namespace Pegasus {
    namespace Alloc{
        class IAllocator;
    }

    namespace Core {
        class IApplicationContext;
    }

}

namespace Pegasus {
namespace Timeline{

//Since only in dev mode blockscripts are mutable, we will need observers to check on
//states of compilation
#if PEGASUS_ENABLE_PROXIES
class ITimelineObserver
{
public:
    ITimelineObserver() {}
    virtual ~ITimelineObserver() {}
    
    virtual void OnCompilationBegin() = 0; 

    virtual void OnCompilationEnd() = 0; 
};
#endif

//!script helper for timeline blocks
class TimelineScript : public TimelineSource, public BlockScript::IBlockScriptCompilerListener
{
    template<class C> friend class Pegasus::Core::Ref;

public:

    //! Constructor
    TimelineScript(Alloc::IAllocator* alloc, Core::IApplicationContext* appContext);

    //! Destructor
    virtual ~TimelineScript();
    
    //! Shuts down a script. It keeps a copy of the last opened script, so use Compile to revive this script again.
    void Shutdown();

    //! Calls the script once, to call anything executing in the global scope
    void CallGlobalScopeInit(BlockScript::BsVmState* state);

    //! Calls the destruction of a script
    void CallGlobalScopeDestroy(BlockScript::BsVmState* state);

    //! Calls update on the script, If script does not implement Update, then this is a NOP
    //! the version parameter will be checked, if mismatched, it will call init globals and will 
    //! set the version to a new one.
    void CallUpdate(float beat, BlockScript::BsVmState* state);

    //! Call before update, this will reveal if the internal asset has changed. If so, the script gets recompiled, and
    //! the serial version is incremented.
    virtual void Compile();

    //! Calls render on the script. If scripts does not implement Render, then this is a NOP
    void CallRender(float beat, BlockScript::BsVmState* state);

    //! Returns true of the script is active. False if it is not
    bool IsScriptActive() const { return mScriptActive; }

    virtual void InvalidateData() { mIsDirty = true; }

    int GetSerialVersion() const { return mSerialVersion; }

    //! Triggered when compilation starts
    virtual void OnCompilationBegin();

    //! Triggered when there is a compilation error
    //! \param line the actual
    //! \param errorMessage the actual error message
    virtual void OnCompilationError(int line, const char* errorMessage, const char* token);

    //! Called at the end of a compilation
    //! \param success true if it was successful, false otherwise
    virtual void OnCompilationEnd(bool success);

    //! \return true if the script has a compilation pending, false otherwise
    bool IsDirty() const { return mIsDirty; }

    //! Adds a reference to a header
    void AddHeader(TimelineSourceIn header);

    //! Clears all references to headers
    void ClearHeaderList();

    BlockScript::BlockScript* GetBlockScript() const { return mScript; } 

    virtual void LockHeaders(bool shouldLock) { mLockHeaders = shouldLock; }

#if PEGASUS_ENABLE_PROXIES
    //! Registers an observer for compilation events
    void RegisterObserver(ITimelineObserver* observer);

    //! Deletes an observer for compilation events
    void UnregisterObserver(ITimelineObserver* observer);
#endif

private:

    // Nodes cannot be copied, only references to them
    PG_DISABLE_COPY(TimelineScript)

    //! Attempts compilation of the opened file.
    //! \return true if successful, false otherwise
    bool CompileInternal();

    //! internal script structure
    BlockScript::BlockScript* mScript;

    //! status of last IO operation
    Io::IoError mIoStatus;

    //! update callback bind point for script
    BlockScript::FunBindPoint mUpdateBindPoint;

    //! render callback bind point for script
    BlockScript::FunBindPoint mRenderBindPoint; 
    
    //! render callback for script destruction
    BlockScript::FunBindPoint mDestroyBindPoint; 

    //! state of current script
    bool mScriptActive;

    //! triggers lazy compilation if tagged as dirty
    bool mIsDirty;

    //! if true, headers are not modified. This is used when blockscript is compiled from a header
    bool mLockHeaders;

    //! Serial version
    int mSerialVersion;

    //! IWindowContext reference to access application data
    Core::IApplicationContext* mAppContext;

    //! list of headers
    Utils::Vector<TimelineSourceRef> mHeaders;

#if PEGASUS_ENABLE_PROXIES
    Utils::Vector<ITimelineObserver*> mCompilationObservers;
#endif
};

//! Reference to a Node, typically used when declaring a variable of reference type
typedef       Pegasus::Core::Ref<TimelineScript>   TimelineScriptRef;

//! Const reference to a reference to a Node, typically used as input parameter of a function
typedef const Pegasus::Core::Ref<TimelineScript> & TimelineScriptIn;

//! Reference to a reference to a Node, typically used as output parameter of a function
typedef       Pegasus::Core::Ref<TimelineScript> & TimelineScriptInOut;

//! Reference to a Node, typically used as the return value of a function
typedef       Pegasus::Core::Ref<TimelineScript>   TimelineScriptReturn;

}
}

#endif
