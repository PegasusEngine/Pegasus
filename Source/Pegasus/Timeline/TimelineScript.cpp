/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineScript.cpp
//! \author	Kleber Garcia
//! \date	1st November 2014
//! \brief	Script helper for scripting callbacks
#include "Pegasus/Timeline/Timeline.h"
#include "Pegasus/Timeline/TimelineScript.h"
#include "Pegasus/Core/IApplicationContext.h"
#include "Pegasus/Core/Assertion.h"
#include "Pegasus/BlockScript/BlockScriptManager.h"
#include "Pegasus/BlockScript/BlockScript.h"
#include "Pegasus/BlockScript/IFileIncluder.h"
#include "Pegasus/Utils/String.h"
#include "Pegasus/Core/Log.h"

using namespace Pegasus;
using namespace Pegasus::Timeline;
using namespace Pegasus::Io;
using namespace Pegasus::BlockScript;
using namespace Pegasus::Alloc;
using namespace Pegasus::Core;

//Helper class to do importing of scripts
class ScriptIncluder : public IFileIncluder
{
public:
    ScriptIncluder(TimelineScript* timelineScript, Pegasus::Timeline::Timeline* timeline)
    : mTimelineScript(timelineScript), mTimeline(timeline) {}

    virtual ~ScriptIncluder(){}

    virtual bool Open (const char* filePath, const char** outBuffer, int& outBufferSize);

    virtual void Close(const char* buffer);

private:
    TimelineScript* mTimelineScript;
    Pegasus::Timeline::Timeline* mTimeline;
};

bool ScriptIncluder::Open(const char* filePath, const char** outBuffer, int& outBufferSize)
{
    TimelineSourceRef t = mTimeline->LoadHeader(filePath);
    if (t != nullptr)
    {
        mTimelineScript->AddHeader(t);
        t->RegisterParent(mTimelineScript);
        t->GetSource(outBuffer, outBufferSize);
        return true;
    }
    return false;
}

void ScriptIncluder::Close(const char* buffer)
{
    //do nothing
}

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

TimelineScript::TimelineScript(IAllocator* allocator, const char* name, FileBuffer* fileBuffer, Core::IApplicationContext* appContext)
    :
    TimelineSource(allocator, name, fileBuffer),
    mSerialVersion(0),
    mScript(nullptr),
    mIsDirty(true),
    mUpdateBindPoint(BlockScript::FUN_INVALID_BIND_POINT),
    mRenderBindPoint(BlockScript::FUN_INVALID_BIND_POINT),
    mDestroyBindPoint(BlockScript::FUN_INVALID_BIND_POINT),
    mScriptActive(false),
    mAppContext(appContext),
    mHeaders(allocator)
{

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
    mScript = appContext->GetBlockScriptManager()->CreateBlockScript();
    mScript->SetCompilerEventListener(this);
}

void TimelineScript::Shutdown()
{
    mScript->Reset();
    mUpdateBindPoint = BlockScript::FUN_INVALID_BIND_POINT;
    mRenderBindPoint = BlockScript::FUN_INVALID_BIND_POINT;
    mScriptActive = false;

}

void TimelineScript::AddHeader(TimelineSourceIn header)
{
    mHeaders.PushEmpty() = header;
}

void TimelineScript::ClearHeaderList()
{
    for (int i = 0; i < mHeaders.GetSize(); ++i)
    {
        mHeaders[i]->UnregisterParent(this);
        mHeaders[i] = nullptr;
    }

    mHeaders.Clear();
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

bool TimelineScript::CompileInternal()
{
    if (mScriptActive == false)
    {
        ClearHeaderList();
        ScriptIncluder includer(this, mAppContext->GetTimeline());
        mScript->SetFileIncluder(&includer);
        mScriptActive = mScript->Compile(&mFileBuffer);
        mScript->SetFileIncluder(nullptr);
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

void TimelineScript::Compile()
{
    if (mIsDirty)
    {
        Shutdown();
        CompileInternal();
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

TimelineScript::~TimelineScript()
{
    ClearHeaderList();
    mAppContext->GetTimeline()->GetScriptTracker()->UnregisterScript(this);
    mAppContext->GetBlockScriptManager()->DestroyBlockScript(mScript);
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

