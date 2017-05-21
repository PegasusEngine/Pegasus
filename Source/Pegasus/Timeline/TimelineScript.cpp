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
#include "Pegasus/Timeline/Timeline.h"
#include "Pegasus/Core/IApplicationContext.h"
#include "Pegasus/Core/Assertion.h"
#include "Pegasus/BlockScript/BlockLib.h"
#include "Pegasus/BlockScript/FunCallback.h"
#include "Pegasus/BlockScript/BlockScriptManager.h"
#include "Pegasus/BlockScript/IFileIncluder.h"
#include "Pegasus/Utils/String.h"
#include "Pegasus/Utils/Memset.h"
#include "Pegasus/Utils/Vector.h"
#include "Pegasus/Core/Log.h"
#include "Pegasus/AssetLib/Asset.h"

#if PEGASUS_ENABLE_PROXIES
#include "Pegasus/BlockScript/SymbolTable.h"
#include "Pegasus/BlockScript/TypeDesc.h"
#endif

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

void TimelineScript::RegisterTypes(Pegasus::BlockScript::BlockLib* lib)
{
    const Pegasus::BlockScript::StructDeclarationDesc descriptionList[] = {
        {
            "UpdateInfo",
            {"float", "float", nullptr},
            {"beat", "relativeBeat", nullptr}
        },
        {
            "RenderInfo",
            {"float", "float",        "int",      "int",           "int",            "float",          "float",           "float", "float", nullptr},
            {"beat",  "relativeBeat", "windowId", "viewportWidth", "viewportHeight", "viewportWidthF", "viewportHeightF", "aspect", "aspectInv", nullptr}
        }
    };

    lib->CreateStructTypes(descriptionList, sizeof(descriptionList)/sizeof(descriptionList[0]));
#if PEGASUS_ENABLE_PROXIES

    //Sanity check, that the programmer has not forgotten to update the blockscript meta types
    const Pegasus::BlockScript::TypeDesc* updateTypeDesc = lib->GetSymbolTable()->GetTypeTable()->GetTypeByName("UpdateInfo");
    const Pegasus::BlockScript::TypeDesc* renderTypeDesc = lib->GetSymbolTable()->GetTypeTable()->GetTypeByName("RenderInfo");
    PG_ASSERT(updateTypeDesc != nullptr && updateTypeDesc->GetByteSize() == sizeof(UpdateInfo));
    PG_ASSERT(renderTypeDesc != nullptr && renderTypeDesc->GetByteSize() == sizeof(RenderInfo));
#endif
}

TimelineScript::TimelineScript(IAllocator* allocator, Core::IApplicationContext* appContext)
    :
    TimelineSource(allocator),
    mSerialVersion(0),
    mScript(nullptr),
    mIsDirty(true),
    mScriptActive(false),
    mAppContext(appContext),
    mHeaders(allocator)
#if PEGASUS_ENABLE_PROXIES
    ,mCompilationObservers(allocator)
#endif
{

    PG_ASSERT(allocator != nullptr);

    ClearBindPoints();

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
    Utils::Vector<BlockScript::BlockLib*>& libs = appContext->GetTimelineManager()->GetLibs();
    for (unsigned int i = 0; i < libs.GetSize(); ++i)
    {
        mScript->IncludeLib(libs[i]);
    }
    mScript->IncludeLib(appContext->GetTimelineManager()->GetTimelineLib());
    mScript->AddCompilerEventListener(this);
}

void TimelineScript::ClearBindPoints()
{
    //Initialize all bind points to invalid
    for (unsigned int i = 0; i < BIND_POINT_COUNT; ++i)
    {
        mBindPoints[i] = Pegasus::BlockScript::FUN_INVALID_BIND_POINT;
    }
}

void TimelineScript::Shutdown()
{
    mScript->Reset();
    ClearBindPoints();
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
    if (IsValidBindPoint(BIND_POINT_DESTROY))
    {
       int output = -1; //the dummy output
       bool res = mScript->ExecuteFunction(state, mBindPoints[BIND_POINT_DESTROY], nullptr, 0, &output, sizeof(output));
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
		#define __Q(x) #x
        #define STRINGIFY(x) __Q(x)
        static const char* defNames[] = {
            "MAX_WINDOW_COUNT"
        };
        static const char* defValues[] = {
            STRINGIFY(PEGASUS_MAX_WORLD_WINDOW_COUNT)
        };
        #undef STRINGIFY
		#undef __Q

        //Compilation speed optimization!
        //So, if we keep a reference of the headers before clearing the header list, it will speed up compilation since it will keep a copy of the file in memory. Otherwise it will have to re-open and parse the file underneath, which slows down compilation significantly.
        Utils::Vector<TimelineSourceRef> headersCopy = mHeaders;
        ClearHeaderList();
        ScriptIncluder includer(this, mAppContext->GetTimelineManager());

#if PEGASUS_ENABLE_PROXIES
        mScript->SetTitle(
           GetOwnerAsset() != nullptr ? GetOwnerAsset()->GetName() : "<Untilted>"
        );
#endif
        mScript->SetFileIncluder(&includer);
        mScript->RegisterDefinitions(defNames, defValues, sizeof(defNames)/sizeof(defNames[0]));
        mScriptActive = mScript->Compile(&mFileBuffer);

        headersCopy.Clear(); //don't need the copy anymore.

        mScript->SetFileIncluder(nullptr);
        const char* types[] = { "float" }; //the only type of this functions is the beat

        const struct BindPointDesc {
            const char* functionName;
            const char* types[10];
            int typesCount;
        } bindPointDescs[BIND_POINT_COUNT] = {
            /***********************************************************************************/
            /**/// Function name                |  parameter list     | parameter list count /**/
            /***********************************************************************************/
            /**/{ "Timeline_OnWindowCreated",   {"int"        },        1},                  /**/
            /**/{ "Timeline_OnWindowDestroyed", {"int"        },        1},                  /**/
            /**/{ "Timeline_Update",            {"UpdateInfo" },        1},                  /**/
            /**/{ "Timeline_Render",            {"RenderInfo" },        1},                  /**/
            /**/{ "Timeline_PostRender",        {"RenderInfo" },        1},                  /**/
            /**/{ "Timeline_Destroy",           {/*empty*/},            0}                   /**/
            /***********************************************************************************/
        };

        if (mScriptActive)
        {
            
            for (unsigned int bp = 0; bp < BIND_POINT_COUNT; ++bp)
            {
                const BindPointDesc& desc = bindPointDescs[bp];
                mBindPoints[bp] = mScript->GetFunctionBindPoint(desc.functionName, desc.types, desc.typesCount);
           }
        
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

void TimelineScript::CallFunction(BsVmState* state, TimelineScript::BindPoint funct, const void* inputBuffer, unsigned inputBufferSz, void* outputBuffer, unsigned outputBufferSz)
{
    if (IsValidBindPoint(funct))
    {
        bool res = mScript->ExecuteFunction(state, mBindPoints[funct], inputBuffer, inputBufferSz, &outputBuffer, outputBufferSz);
        if (!res)
       {
#if PEGASUS_ENABLE_PROXIES
            PG_LOG('ERR_', "Error executing function %d of script %s.", funct, GetDisplayName());
#endif
       }
    }
}

void TimelineScript::CallUpdate(const UpdateInfo& updateInfo, BsVmState* state)
{
    int output = -1;
    CallFunction(state, BIND_POINT_UPDATE, &updateInfo, sizeof(updateInfo), &output, sizeof(output));
}

void TimelineScript::CallRender(const RenderInfo& renderInfo, BsVmState* state)
{
    int output = -1;
    CallFunction(state, BIND_POINT_RENDER, &renderInfo, sizeof(renderInfo), &output, sizeof(output));
}

void TimelineScript::CallWindowCreated(int windowIndex, BsVmState* state)
{
    int output = -1;
    CallFunction(state, BIND_POINT_WINDOW_CREATED, &windowIndex, sizeof(windowIndex), &output, sizeof(output));
}

void TimelineScript::CallWindowDestroyed(int windowIndex, BsVmState* state)
{
    int output = -1;
    CallFunction(state, BIND_POINT_WINDOW_DESTROYED, &windowIndex, sizeof(windowIndex), &output, sizeof(output));
}

TimelineScript::~TimelineScript()
{
    ClearHeaderList();
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
        "", // unused
        0, // unused
        "" // unused
    );
}

void TimelineScript::OnCompilationError(const char* compilationUnitTitle, int line, const char* errorMessage, const char* token)
{
    PG_LOG('CERR', "[%s:%d]: %s. Around token %s", compilationUnitTitle, line, errorMessage, token);

    PEGASUS_EVENT_DISPATCH(
        this,
        CompilerEvents::CompilationNotification,
        // Shader Event specific arguments
        CompilerEvents::CompilationNotification::COMPILATION_ERROR,
        compilationUnitTitle,
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
        PG_LOG('CERR', "Compilation failed for script %s", GetDisplayName());
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

