/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineScript.cpp
//! \author	Kleber Garcia
//! \date	1st November 2014
//! \brief	Script helper for scripting callbacks

#include "Pegasus/Timeline/TimelineScript.h"
#include "Pegasus/Allocator/IAllocator.h"
#include "Pegasus/Window/IWindowContext.h"
#include "Pegasus/Core/Shared/CompilerEvents.h"
#include "Pegasus/Core/Assertion.h"
#include "Pegasus/BlockScript/BlockScriptManager.h"
#include "Pegasus/BlockScript/BlockScript.h"
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

TimelineScript::TimelineScript(IAllocator* allocator, const char* name, FileBuffer* fileBuffer, Wnd::IWindowContext* appContext)
    :
    mSerialVersion(0),
    mAllocator(allocator),
    mScript(nullptr),
    mIsDirty(true),
    mUpdateBindPoint(BlockScript::FUN_INVALID_BIND_POINT),
    mRenderBindPoint(BlockScript::FUN_INVALID_BIND_POINT),
    mDestroyBindPoint(BlockScript::FUN_INVALID_BIND_POINT),
    mScriptActive(false),
    mAppContext(appContext),
    mRefCount(0)
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

    mScriptName[0] = '\0';
    PG_ASSERT(Utils::Strlen(name) < MAX_SCRIPT_NAME);
    Utils::Strcat(mScriptName, name);
    mScript = appContext->GetBlockScriptManager()->CreateBlockScript();
    mScript->SetCompilerEventListener(this);
    mFileBuffer.OwnBuffer(
        mAllocator,
        PG_NEW_ARRAY(mAllocator, -1, "blockscript src", Pegasus::Alloc::PG_MEM_PERM, char, fileBuffer->GetFileSize()),
        fileBuffer->GetFileSize()
    );
    Utils::Memcpy(mFileBuffer.GetBuffer(), fileBuffer->GetBuffer(), fileBuffer->GetFileSize());
}

void TimelineScript::Shutdown()
{
    mScript->Reset();
    mUpdateBindPoint = BlockScript::FUN_INVALID_BIND_POINT;
    mRenderBindPoint = BlockScript::FUN_INVALID_BIND_POINT;
    mScriptActive = false;

}

void TimelineScript::CallGlobalScopeInit(BsVmState* state)
{
    if (mScriptActive)
    {
        mScript->Run(state);
    }
}

void TimelineScript::CallGlobalScopeDestroy(BsVmState* state)
{
    if (mDestroyBindPoint != BlockScript::FUN_INVALID_BIND_POINT)
    {
       int output = -1; //the dummy output
       bool res = mScript->ExecuteFunction(state, mDestroyBindPoint, nullptr, 0, &output, sizeof(output));
       if (!res)
       {
           PG_LOG('ERR_', "Error executing Timeline_Destroy function of script %s.", GetScriptName());
       }
    }
}

bool TimelineScript::CompileScript()
{
    if (mScriptActive == false)
    {
        mScriptActive = mScript->Compile(&mFileBuffer);
        const char* types[] = { "float" }; //the only type of this functions is the beat

        if (mScriptActive)
        {
            mUpdateBindPoint = mScript->GetFunctionBindPoint(
                "Timeline_Update",
                types,
                1
            );

            mRenderBindPoint = mScript->GetFunctionBindPoint(
                "Timeline_Render",
                types,
                1
            );
    
            mDestroyBindPoint = mScript->GetFunctionBindPoint(
                "Timeline_Destroy",
                nullptr,
                0
            );
        
            ++mSerialVersion;
        }
        else
        {
            mScript->Reset(); //cleanup, ready for next compilation attempt
        }
    }
    mIsDirty = false;
    return mScriptActive;
}

void TimelineScript::CheckAndUpdateCompilationState()
{
    if (mIsDirty)
    {
        Shutdown();
        CompileScript();
    }
}

void TimelineScript::CallUpdate(float beat, BsVmState* state)
{

    if (mScriptActive)
    {
        if (mUpdateBindPoint != BlockScript::FUN_INVALID_BIND_POINT)
        {
            int output = -1; //the dummy output
            bool res = mScript->ExecuteFunction(state, mUpdateBindPoint, &beat, sizeof(beat), &output, sizeof(output));
            if (!res)
            {
                PG_LOG('ERR_', "Error executing Update function of script %s.", GetScriptName());
            }
        }
    }
}

void TimelineScript::GetSource(const char** outSrc, int& outSize) const
{
    *outSrc = mFileBuffer.GetBuffer();
    outSize = mFileBuffer.GetFileSize();
}

void TimelineScript::SetSource(const char* source, int sourceSize)
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

void TimelineScript::CallRender(float beat, BsVmState* state)
{
    if (mScriptActive && mRenderBindPoint != BlockScript::FUN_INVALID_BIND_POINT)
    {
        int output = -1; //the dummy output
        bool res = mScript->ExecuteFunction(state, mRenderBindPoint, &beat, sizeof(beat), &output, sizeof(output));
        if (!res)
        {
            PG_LOG('ERR_', "Error executing Render function of script %s.", GetScriptName());
        }
    }
}

void TimelineScript::Release()
{
    PG_ASSERTSTR(mRefCount > 0, "Invalid reference counter (%d), it should have a positive value", mRefCount);
    --mRefCount;

    if (mRefCount <= 0)
    {
        PG_DELETE(mAllocator, this);
    }
}

TimelineScript::~TimelineScript()
{
    mAppContext->GetTimeline()->GetScriptTracker()->UnregisterScript(this);
    mAppContext->GetBlockScriptManager()->DestroyBlockScript(mScript);
    GRAPH_EVENT_DESTROY_USER_DATA(&mProxy, "BlockScript", GetEventListener());
}

void TimelineScript::OnCompilationBegin()
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

void TimelineScript::OnCompilationError(int line, const char* errorMessage, const char* token)
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

void TimelineScript::OnCompilationEnd(bool success)
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
void TimelineScript::SaveScriptToFile()
{
/*
    Io::IOManager* ioManager = mAppContext->GetIOManager();
    Pegasus::Io::IoError err = ioManager->SaveFileToBuffer(mScriptName, mFileBuffer);
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
*/
}
#endif
