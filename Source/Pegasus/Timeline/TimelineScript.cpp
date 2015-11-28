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
#include "Pegasus/BlockScript/Container.h"
#include "Pegasus/BlockScript/BlockScriptAst.h"
#include "Pegasus/BlockScript/TypeDesc.h"
#include "Pegasus/PropertyGrid/Shared/PropertyDefs.h"
#include "Pegasus/Utils/String.h"
#include "Pegasus/Utils/Memset.h"
#include "Pegasus/Utils/Vector.h"
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

class TimelineScriptRuntimeListener : public Pegasus::BlockScript::IRuntimeListener
{
public:
    explicit TimelineScriptRuntimeListener(PropertyGrid::PropertyGridObject* prop, Pegasus::BlockScript::BlockScript* script) 
    : mPropGrid(prop)
    , mScript(script) {}

    virtual ~TimelineScriptRuntimeListener() {}

    //! Triggered when the runtime has not triggered the first instruction
    //! \param state the runtime vm state
    virtual void OnRuntimeBegin(Pegasus::BlockScript::BsVmState& state);

    //! Triggered when the first stack command has been triggered
    //! \param state - the runtime state that has been initialized
    virtual void OnStackInitalized(Pegasus::BlockScript::BsVmState& state);

    //! Triggered when the runtime has exited. The global scope is still alive at this point
    //! so functions can be called even after exiting the Vm.
    //! \param state the runtime vm state
    virtual void OnRuntimeExit(Pegasus::BlockScript::BsVmState& state);

    //! Triggered when there is a crash.
    //! \param state the state on the vm
    //! \param crash information structure
    virtual void OnCrash(BsVmState& state, const CrashInfo& crashInfo);

private:
    PropertyGrid::PropertyGridObject* mPropGrid;
    Pegasus::BlockScript::BlockScript* mScript;

};

void TimelineScriptRuntimeListener::OnRuntimeBegin(Pegasus::BlockScript::BsVmState& state)
{
    /*nop*/
}

void TimelineScriptRuntimeListener::OnStackInitalized(Pegasus::BlockScript::BsVmState& state)
{
    //The stack has been initialized. Now lets patch everything that has a property grid.
    Pegasus::BlockScript::Assembly a = mScript->GetAsm();
    Pegasus::BlockScript::Container<Pegasus::BlockScript::GlobalMapEntry>& bsGlobals = *a.mGlobalsMap;
    if (bsGlobals.Size() == 0)
    {
        return;
    }

    Utils::Vector<bool> foundInGrid;
    for (unsigned int i = 0; i < mPropGrid->GetNumObjectProperties(); ++i)
    {
        foundInGrid.PushEmpty() = false;
    }

    for (int i = 0; i < bsGlobals.Size(); ++i)
    {
        Pegasus::BlockScript::GlobalMapEntry& bsGlobalEntry = bsGlobals[i];
        const char* varName = bsGlobalEntry.mVar->GetName();
        Pegasus::BlockScript::TypeDesc::AluEngine typeAlu = bsGlobalEntry.mVar->GetTypeDesc()->GetAluEngine();
        Pegasus::PropertyGrid::PropertyType targetType = Pegasus::PropertyGrid::PROPERTYTYPE_INVALID;
        int typeByteSize = 0;
        const char* typeName = nullptr;
        switch (typeAlu)
        {
        case Pegasus::BlockScript::TypeDesc::E_INT:
            targetType = Pegasus::PropertyGrid::PROPERTYTYPE_INT;
            typeByteSize = sizeof(int);
            typeName = PropertyGrid::PropertyDefinition<int>::GetTypeName();
            break;
        case Pegasus::BlockScript::TypeDesc::E_FLOAT:
            targetType = Pegasus::PropertyGrid::PROPERTYTYPE_FLOAT;
            typeByteSize = sizeof(float);
            typeName = PropertyGrid::PropertyDefinition<float>::GetTypeName();
            break;
        case Pegasus::BlockScript::TypeDesc::E_FLOAT2:
            targetType = Pegasus::PropertyGrid::PROPERTYTYPE_VEC2;
            typeByteSize = sizeof(Math::Vec2);
            typeName = PropertyGrid::PropertyDefinition<Math::Vec2>::GetTypeName();
            break;
        case Pegasus::BlockScript::TypeDesc::E_FLOAT3:
            targetType = Pegasus::PropertyGrid::PROPERTYTYPE_VEC3;
            typeByteSize = sizeof(Math::Vec3);
            typeName = PropertyGrid::PropertyDefinition<Math::Vec3>::GetTypeName();
            break;
        case Pegasus::BlockScript::TypeDesc::E_FLOAT4:
            targetType = Pegasus::PropertyGrid::PROPERTYTYPE_VEC4;
            typeByteSize = sizeof(Math::Vec4);
            typeName = PropertyGrid::PropertyDefinition<Math::Vec4>::GetTypeName();
            break;
        default:
            PG_LOG('ERR_', "Unsupported pegasus type from blockscript extern. On variable: %s ", varName);
            continue;
        }

        //find the target property grid
        bool foundObject = false;
        for (unsigned int obPropIndex = 0; !foundObject && obPropIndex < mPropGrid->GetNumObjectProperties(); ++obPropIndex)
        {
            PropertyGrid::PropertyAccessor a = mPropGrid->GetObjectPropertyAccessor(obPropIndex);
            const PropertyGrid::PropertyRecord& r = mPropGrid->GetObjectPropertyRecord(obPropIndex);
            if (!Utils::Strcmp(varName, r.name))
            {
                foundObject = true;
                if (targetType == r.type)
                {
                    foundInGrid[obPropIndex] = true; 
                    float scratch[16];
                    PG_ASSERT(typeByteSize <= sizeof(scratch));
                    //initialize the runtime propertly
                    a.Read(scratch, typeByteSize);
                    mScript->WriteGlobalValue(&state, i, scratch, typeByteSize);
                }
                else
                { 
                    PG_ASSERTSTR(!foundInGrid[obPropIndex], "Cannot attempt to delete an object twice!");
                    mPropGrid->RemoveObjectProperty(obPropIndex);
                    foundInGrid.Delete(obPropIndex);
                    mPropGrid->AddObjectProperty(targetType, typeByteSize, varName, typeName, &bsGlobalEntry.mDefaultVal->GetVariant());
                    foundInGrid.PushEmpty() = true;
                    foundInGrid[mPropGrid->GetNumObjectProperties() - 1] = true;
                }
            }
        }

        //We never found this property, lets create it from scratch
        if (!foundObject)
        {
            mPropGrid->AddObjectProperty(targetType, typeByteSize, varName, typeName, &bsGlobalEntry.mDefaultVal->GetVariant());
            foundInGrid.PushEmpty() = true;
        }
    }

    //delete now all the properties that are unused
    for (unsigned int i = foundInGrid.GetSize(); i >= 1; --i)
    {
        if (!foundInGrid[i - 1])
        {
            mPropGrid->RemoveObjectProperty(i - 1);
        }
    }   
}

void TimelineScriptRuntimeListener::OnRuntimeExit(Pegasus::BlockScript::BsVmState& state)
{
    /*nop*/
}

void TimelineScriptRuntimeListener::OnCrash(Pegasus::BlockScript::BsVmState& state, const Pegasus::BlockScript::CrashInfo& crashInfo)
{
    /*TODO: handle this*/
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
    mUpdateBindPoint(Pegasus::BlockScript::FUN_INVALID_BIND_POINT),
    mRenderBindPoint(Pegasus::BlockScript::FUN_INVALID_BIND_POINT),
    mDestroyBindPoint(Pegasus::BlockScript::FUN_INVALID_BIND_POINT),
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

void TimelineScript::CallGlobalScopeInit(BsVmState* state, PropertyGrid::PropertyGridObject* obj)
{
    if (mScriptActive)
    {
        TimelineScriptRuntimeListener l(obj, mScript);
        state->SetRuntimeListener(&l);
        mScript->Run(state);
        state->SetRuntimeListener(nullptr);
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

