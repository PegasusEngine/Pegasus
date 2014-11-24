/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	ScriptHelper.cpp
//! \author	Kleber Garcia
//! \date	1st November 2014
//! \brief	Script helper for scripting callbacks

#include "Pegasus/Timeline/ScriptHelper.h"
#include "Pegasus/Allocator/IAllocator.h"
#include "Pegasus/Window/IWindowContext.h"
#include "Pegasus/Core/Shared/CompilerEvents.h"
#include "Pegasus/Core/Assertion.h"
#include "Pegasus/BlockScript/BlockScript.h"
#include "Pegasus/BlockScript/FunCallback.h"
#include "Pegasus/Utils/String.h"
#include "Pegasus/Core/Log.h"

using namespace Pegasus;
using namespace Pegasus::Timeline;
using namespace Pegasus::Io;
using namespace Pegasus::BlockScript;
using namespace Pegasus::Alloc;
using namespace Pegasus::Core;

static int Pegasus_PrintString(const char * str)
{
    PG_LOG('TMLN', " {script-echo} %s", str);
    return 0;
}

static int Pegasus_PrintInt(int i)
{
    PG_LOG('TMLN', " {script-echo} %d", i);
    return 0;
}

static int Pegasus_PrintFloat(float f)
{
    PG_LOG('TMLN', " {script-echo} %f", f);
    return 0;
}

ScriptHelper::ScriptHelper(IAllocator* allocator, Io::IOManager* ioManager)
    :
    mSerialVersion(0),
    mAllocator(allocator),
    mScript(allocator),
    mIsDirty(false),
    mIoStatus(Io::ERR_FILE_NOT_FOUND), //no script opened
    mUpdateBindPoint(BlockScript::FUN_INVALID_BIND_POINT),
    mRenderBindPoint(BlockScript::FUN_INVALID_BIND_POINT),
    mScriptActive(false),
    mIoManager(ioManager)
#if PEGASUS_ENABLE_PROXIES
    ,mProxy(this)
#endif
{
    GRAPH_EVENT_INIT_DISPATCHER

    PG_ASSERT(allocator != nullptr);

    //setup the global system callbacks, if not initialized
    if (Pegasus::BlockScript::SystemCallbacks::gPrintStrCallback == nullptr)
    {
        Pegasus::BlockScript::SystemCallbacks::gPrintStrCallback = Pegasus_PrintString;
    }

    if (Pegasus::BlockScript::SystemCallbacks::gPrintIntCallback == nullptr)
    {
        Pegasus::BlockScript::SystemCallbacks::gPrintIntCallback  = Pegasus_PrintInt;
    }

    if (Pegasus::BlockScript::SystemCallbacks::gPrintFloatCallback == nullptr)
    {
        Pegasus::BlockScript::SystemCallbacks::gPrintFloatCallback = Pegasus_PrintFloat;
    }

    mScript.SetCompilerEventListener(this);
}

bool ScriptHelper::OpenScript(const char* scriptFile)
{
    //store the script name
    PG_ASSERTSTR(Utils::Strlen(scriptFile) < MAX_SCRIPT_NAME + 1, "Script name should be less than %d characters!", MAX_SCRIPT_NAME);
    mScriptName[0] = '\0';
    Utils::Strcat(mScriptName, scriptFile);

    InvalidateData();

    PG_ASSERTSTR(mScriptActive == false, "Script must be shutdown before you can reopen!");
    if (mScriptActive == false)
    {
        mIoStatus = Io::ERR_FILE_NOT_FOUND;

        mFileBuffer.DestroyBuffer();

        mIoStatus = mIoManager->OpenFileToBuffer(
            scriptFile,
            mFileBuffer,
            true,
            mAllocator
        );

        if (mIoStatus != Io::ERR_NONE)
        {
            PG_LOG('ERR_', "could not open block script file %s", scriptFile);

            GRAPH_EVENT_DISPATCH(
                this,
                CompilerEvents::FileOperationEvent, 
                // Event specific arguments:
                CompilerEvents::FileOperationEvent::IO_ERROR, 
                mIoStatus,
                GetScriptName(), 
                "Io error"
            );
        }
        else
        {
            GRAPH_EVENT_DISPATCH(
                this,
                CompilerEvents::SourceLoadedEvent, 
                // Event specific arguments:
                mFileBuffer.GetBuffer(), 
                mFileBuffer.GetFileSize()
            );
            return true;
        }
    }
    return false;
}

void ScriptHelper::Shutdown()
{
    mScript.Reset();
    mUpdateBindPoint = BlockScript::FUN_INVALID_BIND_POINT;
    mRenderBindPoint = BlockScript::FUN_INVALID_BIND_POINT;
    mScriptActive = false;

}

void ScriptHelper::CallGlobalScopeInit(BsVmState* state)
{
    if (mScriptActive)
    {
        mScript.Run(state);
    }
}

bool ScriptHelper::CompileScript()
{
    if (mScriptActive == false)
    {
        if (mIoStatus == Io::ERR_NONE)
        {
            mScriptActive = mScript.Compile(&mFileBuffer);
            const char* types[] = { "float" }; //the only type of this functions is the beat

            if (mScriptActive)
            {
                mUpdateBindPoint = mScript.GetFunctionBindPoint(
                    "Timeline_Update",
                    types,
                    1
                );

                mRenderBindPoint = mScript.GetFunctionBindPoint(
                    "Timeline_Render",
                    types,
                    1
                );
            
                ++mSerialVersion;
            }
            else
            {
                mScript.Reset(); //cleanup, ready for next compilation attempt
            }
        }
    }
    mIsDirty = false;
    return mScriptActive;
}

void ScriptHelper::CallUpdate(float beat, BsVmState* state, int& version)
{
    if (mIsDirty)
    {
        Shutdown();
        CompileScript();
    }

    if (mSerialVersion != version)
    {
        version = mSerialVersion;
        state->Reset();
        mScript.Run(state);
    }

    if (mScriptActive && mUpdateBindPoint != BlockScript::FUN_INVALID_BIND_POINT)
    {
        int output = -1; //the dummy output
        bool res = mScript.ExecuteFunction(state, mUpdateBindPoint, &beat, sizeof(beat), &output, sizeof(output));
        if (!res)
        {
            PG_LOG('ERR_', "Error executing Update function of script %s.", GetScriptName());
        }
    }
}

void ScriptHelper::GetSource(const char** outSrc, int& outSize) const
{
    *outSrc = mFileBuffer.GetBuffer();
    outSize = mFileBuffer.GetFileSize();
}

void ScriptHelper::SetSource(const char* source, int sourceSize)
{
    InvalidateData();
    if (sourceSize > mFileBuffer.GetFileSize())
    {
        mFileBuffer.DestroyBuffer();
        mFileBuffer.OwnBuffer(
            mAllocator, 
            PG_NEW_ARRAY(mAllocator, -1, "blockscript src", Pegasus::Alloc::PG_MEM_PERM, char, sourceSize),
            sourceSize
        );
    }
    mFileBuffer.SetFileSize(sourceSize);
    Pegasus::Utils::Memcpy(mFileBuffer.GetBuffer(), source, sourceSize);
}

void ScriptHelper::CallRender(float beat, BsVmState* state)
{
    if (mScriptActive && mRenderBindPoint != BlockScript::FUN_INVALID_BIND_POINT)
    {
        int output = -1; //the dummy output
        bool res = mScript.ExecuteFunction(state, mRenderBindPoint, &beat, sizeof(beat), &output, sizeof(output));
        if (!res)
        {
            PG_LOG('ERR_', "Error executing Render function of script %s.", GetScriptName());
        }
    }
}

ScriptHelper::~ScriptHelper()
{
}

void ScriptHelper::OnCompilationBegin()
{
    PG_LOG('TMLN', "Compilation started for blockscript: %s", GetScriptName());    

    GRAPH_EVENT_DISPATCH(
        this,
        CompilerEvents::CompilationNotification, 
        // Event specific arguments:
        CompilerEvents::CompilationNotification::COMPILATION_BEGIN, 
        0, // unused
        "" // unused
    );
}

void ScriptHelper::OnCompilationError(int line, const char* errorMessage, const char* token)
{
    PG_LOG('ERR_', "Compilation error, line %d, %s. Around token %s", line, errorMessage, token);

    GRAPH_EVENT_DISPATCH(
        this,
        CompilerEvents::CompilationNotification,
        // Shader Event specific arguments
        CompilerEvents::CompilationNotification::COMPILATION_ERROR,
        line,
        errorMessage
    );
}

void ScriptHelper::OnCompilationEnd(bool success)
{
    if (success)
    {
        PG_LOG('TMLN', "Compilation succeeded for script %s", GetScriptName());
    }
    else
    {
        PG_LOG('ERR_', "Compilation failed for script %s", GetScriptName());
    }

    GRAPH_EVENT_DISPATCH (
        this,
        CompilerEvents::CompilationEvent,
        // Event specific arguments
        success ? true : false, //compilation success status
        success ? "" : "Blockscript Compilation Failed."
    );
}


#if PEGASUS_ENABLE_PROXIES
void ScriptHelper::SaveScriptToFile()
{
    Pegasus::Io::IoError err = mIoManager->SaveFileToBuffer(mScriptName, mFileBuffer);
    if (err == Pegasus::Io::ERR_NONE)
    {
        GRAPH_EVENT_DISPATCH(
            this,
            CompilerEvents::FileOperationEvent, 
            // Event specific arguments:
            CompilerEvents::FileOperationEvent::IO_FILE_SAVE_SUCCESS,
            err,
            mScriptName,
            ""
        );
    }
    else
    {
        GRAPH_EVENT_DISPATCH (
            this,
            CompilerEvents::FileOperationEvent, 
            // Event specific arguments:
            CompilerEvents::FileOperationEvent::IO_FILE_SAVE_ERROR,
            err,
            mScriptName,
            "Error saving file :/"
        );
    }
}
#endif
