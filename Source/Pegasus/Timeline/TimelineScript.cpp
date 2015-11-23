/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file	TimelineScript.cpp
//! \author	Kleber Garcia
//! \date	1st November 2014
//! \brief	Script helper for scripting callbacks
#include "Pegasus/Timeline/TimelineManager.h"
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
    ScriptIncluder(TimelineScript* timelineScript, Pegasus::Timeline::TimelineManager* timelineManager)
    : mTimelineScript(timelineScript), mTimelineManager(timelineManager) {}

    virtual ~ScriptIncluder(){}

    virtual bool Open (const char* filePath, const char** outBuffer, int& outBufferSize);

    virtual void Close(const char* buffer);

private:
    TimelineScript* mTimelineScript;
    Pegasus::Timeline::TimelineManager* mTimelineManager;
};

bool ScriptIncluder::Open(const char* filePath, const char** outBuffer, int& outBufferSize)
{
    TimelineSourceRef t = mTimelineManager->LoadHeader(filePath);
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

TimelineScript::TimelineScript(IAllocator* allocator, Core::IApplicationContext* appContext)
    :
    TimelineSource(allocator),
    mSerialVersion(0),
    mScript(nullptr),
    mIsDirty(true),
    mUpdateBindPoint(BlockScript::FUN_INVALID_BIND_POINT),
    mRenderBindPoint(BlockScript::FUN_INVALID_BIND_POINT),
    mDestroyBindPoint(BlockScript::FUN_INVALID_BIND_POINT),
    mScriptActive(false),
    mAppContext(appContext),
    mHeaders(allocator)
#if PEGASUS_ENABLE_PROXIES
    ,mCompilationObservers(allocator)
#endif
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
    for (unsigned int i = 0; i < mHeaders.GetSize(); ++i)
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
    #if PEGASUS_ENABLE_PROXIES
           PG_LOG('ERR_', "Error executing Timeline_Destroy function of script %s.", GetDisplayName());
    #endif
       }
    }
}

bool TimelineScript::CompileInternal()
{
    if (mScriptActive == false)
    {
        ClearHeaderList();
        ScriptIncluder includer(this, mAppContext->GetTimelineManager());
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
#if PEGASUS_ENABLE_PROXIES
    //Once compilation is done, go ahead and call all observers
    for (unsigned int i = 0; i < mCompilationObservers.GetSize(); ++i)
    {
        mCompilationObservers[i]->OnCompilationBegin();
    }
#endif

    if (mIsDirty)
    {
        Shutdown();
        CompileInternal();
    }

#if PEGASUS_ENABLE_PROXIES
    //Once compilation is done, go ahead and call all observers
    for (unsigned int i = 0; i < mCompilationObservers.GetSize(); ++i)
    {
        mCompilationObservers[i]->OnCompilationEnd();
    }
#endif
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
#if PEGASUS_ENABLE_PROXIES
                PG_LOG('ERR_', "Error executing Update function of script %s.", GetDisplayName());
#endif
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
#if PEGASUS_ENABLE_PROXIES
            PG_LOG('ERR_', "Error executing Render function of script %s.", GetDisplayName());
#endif
        }
    }
}

TimelineScript::~TimelineScript()
{
    ClearHeaderList();
    mAppContext->GetTimelineManager()->GetScriptTracker()->UnregisterScript(this);
    mAppContext->GetBlockScriptManager()->DestroyBlockScript(mScript);
}

void TimelineScript::OnCompilationBegin()
{
#if PEGASUS_ENABLE_PROXIES
    PG_LOG('TMLN', "Compilation started for blockscript: %s", GetDisplayName());    
#endif

    PEGASUS_EVENT_DISPATCH(
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

    PEGASUS_EVENT_DISPATCH(
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
#if PEGASUS_ENABLE_PROXIES
        PG_LOG('TMLN', "Compilation succeeded for script %s", GetDisplayName());
#endif
    }
    else
    {
#if PEGASUS_ENABLE_PROXIES
        PG_LOG('ERR_', "Compilation failed for script %s", GetDisplayName());
#endif
    }

    PEGASUS_EVENT_DISPATCH (
        this,
        CompilerEvents::CompilationEvent,
        // Event specific arguments
        success ? true : false, //compilation success status
        success ? "" : "Blockscript Compilation Failed."
    );

}

#if PEGASUS_ENABLE_PROXIES
void TimelineScript::RegisterObserver(ITimelineObserver* observer)
{
    mCompilationObservers.PushEmpty() = observer; 
}

void TimelineScript::UnregisterObserver(ITimelineObserver* observer)
{
    for (unsigned int i = 0; i < mCompilationObservers.GetSize(); ++i)
    {
        if (observer == mCompilationObservers[i])
        {
            mCompilationObservers.Delete(i);
            return;
        }
    }

    PG_FAILSTR("Failed to delete observer from registration list.");
}
#endif

