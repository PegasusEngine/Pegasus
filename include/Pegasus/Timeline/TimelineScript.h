/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineScript.h
//! \author	Kleber Garcia
//! \date	1st November 2014
//! \brief	Script helper for scripting callbacks

#ifndef SCRIPT_HELPER_H
#define SCRIPT_HELPER_H

#include "Pegasus/Timeline/TimelineSource.h"
#include "Pegasus/BlockScript/BlockScript.h"
#include "Pegasus/BlockScript/EventListeners.h"
#include "Pegasus/Core/Shared/EventDefs.h"
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

    namespace BlockScript {
        class BlockLib;
    }
    namespace Timeline {
        struct UpdateInfo;
        struct RenderInfo;
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

    //! Get the class instance name of this object
    virtual const char* GetClassInstanceName() const { return "TimelineScript" ; }

    //! Calls the script once, to call anything executing in the global scope
    //! \param state the state containing definitions
    //! \param propertyGrid the property grid that will fill in the state / or synchronize the state of this block
    void CallGlobalScopeInit(BlockScript::BsVmState* state);

    //! Calls the destruction of a script
    void CallGlobalScopeDestroy(BlockScript::BsVmState* state);

    //! Calls update on the script, If script does not implement Update, then this is a NOP
    //! the version parameter will be checked, if mismatched, it will call init globals and will 
    //! set the version to a new one.
    //! \param update information.
    //! \param state virtual machine state.
    void CallUpdate(const UpdateInfo& updateInfo, BlockScript::BsVmState* state);

    //! Call before update, this will reveal if the internal asset has changed. If so, the script gets recompiled, and
    //! the serial version is incremented.
    virtual void Compile();

    //! Calls render on the script. If scripts does not implement Render, then this is a NOP
    //! \param render information used.
    //! \param state the virtual machine state.
    void CallRender(const RenderInfo& renderInfo, BlockScript::BsVmState* state);

    //! Calls window created on a script.
    //! \param windowIndex - the index of the window being created
    //! \param state - the vs vm state
    void CallWindowCreated(int windowIndex, BlockScript::BsVmState* state);

    //! Calls window destroyed on a script.
    //! \param windowIndex - the index of the window being destroyed
    //! \param state - the vs vm state
    void CallWindowDestroyed(int windowIndex, BlockScript::BsVmState* state);

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

    //! Register types on a library for blockscript
    static void RegisterTypes(Pegasus::BlockScript::BlockLib* targetLib);

private:

    // Nodes cannot be copied, only references to them
    PG_DISABLE_COPY(TimelineScript)


    //! Attempts compilation of the opened file.
    //! \return true if successful, false otherwise
    bool CompileInternal();

    void ClearBindPoints();

    //! internal script structure
    BlockScript::BlockScript* mScript;

    //! status of last IO operation
    Io::IoError mIoStatus;

    //@{
    //! bind points for script 
    enum BindPoint
    {
        BIND_POINT_WINDOW_CREATED,
        BIND_POINT_WINDOW_DESTROYED,
        BIND_POINT_UPDATE,
        BIND_POINT_RENDER,
        BIND_POINT_POSTRENDER,
        BIND_POINT_DESTROY,
        BIND_POINT_COUNT
    };
    //@}

    void CallFunction(BlockScript::BsVmState* state, BindPoint funct, const void* inputBuffer, unsigned inputBufferSz, void* outputBuffer, unsigned outputBufferSz);
    bool IsValidBindPoint(BindPoint bp) const { return mScriptActive && mBindPoints[bp] != Pegasus::BlockScript::FUN_INVALID_BIND_POINT; }

    //! Set that contains bind points
    BlockScript::FunBindPoint mBindPoints[BIND_POINT_COUNT];


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
