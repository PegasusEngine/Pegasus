/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	ScriptHelper.cpp
//! \author	Kevin Boulanger
//! \date	1st November 2014
//! \brief	Script helper for scripting callbacks

#ifndef SCRIPT_HELPER_H
#define SCRIPT_HELPER_H

#include "Pegasus/BlockScript/BlockScript.h"
#include "Pegasus/BlockScript/IBlockScriptCompilerListener.h"
#include "Pegasus/Graph/Shared/GraphEventDefs.h"
#include "Pegasus/Core/Shared/CompilerEvents.h"
#include "Pegasus/Core/Io.h"

#if PEGASUS_ENABLE_PROXIES
#include "Pegasus/Timeline/Proxy/ScriptProxy.h"
#endif

#define MAX_SCRIPT_NAME 64


//! fwd dec
namespace Pegasus {
    namespace Alloc{
        class IAllocator;
    }

    namespace Io {
        class IOManager;
    }
}

namespace Pegasus {
namespace Timeline{

//!script helper for timeline blocks
class ScriptHelper : public BlockScript::IBlockScriptCompilerListener
{
    GRAPH_EVENT_DECLARE_DISPATCHER(Core::CompilerEvents::ICompilerEventListener);

public:
    //! Constructor
    ScriptHelper(Alloc::IAllocator* alloc, Io::IOManager* ioManager);

    //! Destructor
    ~ScriptHelper();

    //! opens a script file
    //! \param scriptFile the path of the script to open
    //! \return true if successful, false otherwise
    bool OpenScript(const char* scriptFile);
    
    //! Shuts down a script. It keeps a copy of the last opened script, so use Compile to revive this script again.
    void Shutdown();

    //! Attempts compilation of the opened file.
    //! \return true if successful, false otherwise
    bool CompileScript();

    //! Calls the script once, to call anything executing in the global scope
    void CallGlobalScopeInit(BlockScript::BsVmState* state);

    //! Calls update on the script, If script does not implement Update, then this is a NOP
    //! the version parameter will be checked, if mismatched, it will call init globals and will 
    //! set the version to a new one.
    void CallUpdate(float beat, BlockScript::BsVmState* state, int& version);

    //! Calls render on the script. If scripts does not implement Render, then this is a NOP
    void CallRender(float beat, BlockScript::BsVmState* state);

    //! Returns true of the script is active. False if it is not
    bool IsScriptActive() const { return mScriptActive; }

    const char* GetScriptName() const { return mScriptName; }

    //! Gets the source of the source code file
    //! \param outSrc output param to be filled with a string pointer containing the src
    //! \param outSize output param to an int, to be filled with the size of outSize
    void GetSource(const char ** outSrc, int& outSize) const;

    //! Sets the source and marks source code file as dirty
    //! \param source string. Doesn't need to be null terminated
    //! \param source size to copy 
    void SetSource(const char * source, int sourceSize);

    void InvalidateData() { mIsDirty = true; }

#if PEGASUS_ENABLE_PROXIES
    //! Gets the proxy 
    //! \return Proxy to this script
    ScriptProxy* GetProxy() { return &mProxy; };

    void SaveScriptToFile();
#endif

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

private:

    //! internal allocator
    Alloc::IAllocator* mAllocator;

    //! internal script structure
    BlockScript::BlockScript mScript;

    //! status of last IO operation
    Io::IoError mIoStatus;

    //! update callback bind point for script
    BlockScript::FunBindPoint mUpdateBindPoint;

    //! render callback bind point for script
    BlockScript::FunBindPoint mRenderBindPoint; 

    //! File data
    Io::FileBuffer mFileBuffer;

    //! state of current script
    bool mScriptActive;

    //! triggers lazy compilation if tagged as dirty
    bool mIsDirty;

    //! copy of the script name
    char mScriptName[MAX_SCRIPT_NAME];

    //! Serial version
    int mSerialVersion;

    //! The io manager, for file manipulation
    Io::IOManager * mIoManager;

#if PEGASUS_ENABLE_PROXIES
    ScriptProxy mProxy;
#endif

};

}
}

#endif
