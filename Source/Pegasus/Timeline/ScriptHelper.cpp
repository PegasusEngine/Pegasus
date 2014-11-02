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

ScriptHelper::ScriptHelper(IAllocator* allocator, Wnd::IWindowContext* appContext)
    :
    mAllocator(allocator),
    mScript(nullptr),
    mAppContext(appContext),
    mIsDirty(false),
    mIoStatus(Io::ERR_FILE_NOT_FOUND), //no script opened
    mUpdateBindPoint(BlockScript::FUN_INVALID_BIND_POINT),
    mRenderBindPoint(BlockScript::FUN_INVALID_BIND_POINT),
    mScriptActive(false)
{
    PG_ASSERT(allocator != nullptr && appContext != nullptr);

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
}

bool ScriptHelper::OpenScript(const char* scriptFile)
{
    PG_ASSERTSTR(mScriptActive == false, "Script must be shutdown before you can reopen!");
    mIsDirty = true;
    if (mScriptActive == false)
    {
        mIoStatus = Io::ERR_FILE_NOT_FOUND;

        if (mScript == nullptr)
        {
            mScript = PG_NEW(mAllocator, -1, "BlockScript(timeline)", Alloc::PG_MEM_PERM ) BlockScript::BlockScript(mAllocator);
        }

        mFileBuffer.DestroyBuffer();

        mIoStatus = mAppContext->GetIOManager()->OpenFileToBuffer(
            scriptFile,
            mFileBuffer,
            true,
            mAllocator
        );

        if (mIoStatus != Io::ERR_NONE)
        {
            PG_LOG('ERR_', "could not open block script file %s", scriptFile);
        }
        else
        {
            //store the script name
            PG_ASSERTSTR(Utils::Strlen(scriptFile) < MAX_SCRIPT_NAME + 1, "Script name should be less than %d characters!", MAX_SCRIPT_NAME);
            mScriptName[0] = '\0';
            Utils::Strcat(mScriptName, scriptFile);
            return true;
        }
    }
    return false;
}

void ScriptHelper::Shutdown()
{
    if (mScriptActive)
    {
        mScript->Reset();
        mUpdateBindPoint = BlockScript::FUN_INVALID_BIND_POINT;
        mRenderBindPoint = BlockScript::FUN_INVALID_BIND_POINT;
        mScriptActive = false;
    }
}

bool ScriptHelper::CompileScript()
{
    if (mScript != nullptr && mScriptActive == false)
    {
        if (mIoStatus == Io::ERR_NONE)
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

                //run the script once, to initialize everything
                mScript->Run();
            }
        }
    }
    mIsDirty = false;
    return mScriptActive;
}

void ScriptHelper::CallUpdate(float beat)
{
    if (mIsDirty)
    {
        Shutdown();
        CompileScript();
    }

    if (mScriptActive && mUpdateBindPoint != BlockScript::FUN_INVALID_BIND_POINT)
    {
        int output = -1; //the dummy output
        bool res = mScript->ExecuteFunction(mUpdateBindPoint, &beat, sizeof(beat), &output, sizeof(output));
        if (!res)
        {
            PG_LOG('ERR_', "Error executing Update function of script %s.", GetScriptName());
        }
    }
}

void ScriptHelper::CallRender(float beat)
{
    if (mScriptActive && mRenderBindPoint != BlockScript::FUN_INVALID_BIND_POINT)
    {
        int output = -1; //the dummy output
        bool res = mScript->ExecuteFunction(mRenderBindPoint, &beat, sizeof(beat), &output, sizeof(output));
        if (!res)
        {
            PG_LOG('ERR_', "Error executing Render function of script %s.", GetScriptName());
        }
    }
}

ScriptHelper::~ScriptHelper()
{
    if (mScript != nullptr)
    {
        PG_DELETE(mAllocator, mScript);
    }
}
