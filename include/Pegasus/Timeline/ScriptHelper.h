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
#include "Pegasus/Core/Io.h"

#define MAX_SCRIPT_NAME 64


//! fwd dec
namespace Pegasus {
    namespace Alloc{
        class IAllocator;
    }

    namespace Wnd {
        class IWindowContext;
    }
}

namespace Pegasus {
namespace Timeline{

//!script helper for timeline blocks
class ScriptHelper
{
public:
    //! Constructor
    ScriptHelper(Alloc::IAllocator* alloc, Wnd::IWindowContext* context);

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

    //! Calls update on the script, If script does not implement Update, then this is a NOP
    void CallUpdate(float beat);

    //! Calls render on the script. If scripts does not implement Render, then this is a NOP
    void CallRender(float beat);

    //! Returns true of the script is active. False if it is not
    bool IsScriptActive() const { return mScriptActive; }

    const char* GetScriptName() const { return mScriptName; }

private:

    //! internal allocator
    Alloc::IAllocator* mAllocator;

    //! internal script structure
    BlockScript::BlockScript* mScript;

    //! manager for IO
    Wnd::IWindowContext* mAppContext;

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

};

}
}

#endif
