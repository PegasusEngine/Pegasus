/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   FunCallback.cpp
//! \author Kleber Garcia
//! \date   October 28th 2014
//! \brief  Runtime utilities to register c++ callback functions into blockscript

#include "Pegasus/BlockScript/FunCallback.h"
#include "Pegasus/BlockScript/BlockScriptAst.h"
#include "Pegasus/BlockScript/BlockScriptBuilder.h"
#include "Pegasus/BlockScript/TypeTable.h"
#include "Pegasus/BlockScript/TypeDesc.h"
#include "Pegasus/BlockScript/BsVm.h"
#include "Pegasus/BlockScript/BlockScriptAst.h"
#include "Pegasus/BlockScript/Canonizer.h"
#include "Pegasus/BlockScript/FunTable.h"
#include "Pegasus/BlockScript/FunDesc.h"
#include "Pegasus/Utils/Memcpy.h"
#include "Pegasus/Utils/String.h"
#include "Pegasus/Core/Log.h"
#include "Pegasus/Core/Assertion.h"
#include "Pegasus/Core/Time.h"

using namespace Pegasus;
using namespace Pegasus::BlockScript;
using namespace Pegasus::BlockScript::Ast;

void* Pegasus::BlockScript::FunParamStream::NextArgument(int sz)
{
    PG_ASSERTSTR(mBufferPos + sz <= mContext->GetInputBufferSize(), "Invalid number of parameters have been read! make sure you read the proper parameter sizes!");
    void* outPtr = static_cast<char*>(mContext->GetRawInputBuffer()) + mBufferPos;
    mBufferPos += sz;
    return outPtr;
}

const char* Pegasus::BlockScript::FunParamStream::NextBsStringArgument()
{
    int& strRef = NextArgument<int>();
    BsVmState* state = mContext->GetVmState();
    return static_cast<const char*>(state->GetHeapElement(strRef).mObject);
}

Pegasus::BlockScript::PrintStringCallbackType   Pegasus::BlockScript::SystemCallbacks::gPrintStrCallback = nullptr;
Pegasus::BlockScript::PrintIntCallbackType      Pegasus::BlockScript::SystemCallbacks::gPrintIntCallback = nullptr;
Pegasus::BlockScript::PrintFloatCallbackType    Pegasus::BlockScript::SystemCallbacks::gPrintFloatCallback = nullptr;



FunBindPoint Pegasus::BlockScript::GetFunctionBindPoint(
    const BlockScriptBuilder* builder, 
    const Assembly& assembly,
    const char* funName,
    const char*const* argTypes,
    int argumentListCount
)
{
    const Container<FunMapEntry>* funEntries = assembly.mFunBlockMap;
    int funEntriesSize = funEntries->Size();

    for (int funCandidateId = 0; funCandidateId < funEntriesSize; ++funCandidateId)
    {
        const FunMapEntry funMapEntry = (*funEntries)[funCandidateId];
        const FunDesc* funDescEntry = funMapEntry.mFunDesc;
        PG_ASSERT(funDescEntry != nullptr);
        const Ast::StmtFunDec* funDecEntry = funDescEntry->GetDec();
        PG_ASSERT(funDecEntry != nullptr);

        if (Utils::Strcmp(funName, funDecEntry->GetName()) != 0)
        {
            continue;
        }

        Ast::ArgList* argList = funDecEntry->GetArgList();

        bool foundFun = true;

        for (int i = 0; i < argumentListCount; ++i)
        {
            if (argList == nullptr || argList->GetArgDec() == nullptr)
            {
                foundFun = false;
                break;
            }
        
            Ast::ArgDec* argDec = argList->GetArgDec();
            
            const char* argTypeName = argTypes[i];
            const TypeDesc* type = builder->GetTypeByName(argTypeName);

            if (type == nullptr)
            {
                return FUN_INVALID_BIND_POINT;
            }

            if (!type->Equals(argDec->GetType()))
            {
                foundFun = false;
                break;
            }

            argList = argList->GetTail();
        }

        if (foundFun && (argList == nullptr || argList->GetArgDec() == nullptr))
        {
            return funCandidateId;
        }
    }

    return FUN_INVALID_BIND_POINT;
}

//ideally we want to keep these hidden.. but this is an exception... as we will reuse some state code
extern void PushFrameCommand(const StackFrameInfo* info, BsVmState& state, const Container<GlobalMapEntry>* globalsInitData);
extern void PopFrameCommand(BsVmState& state);

bool Pegasus::BlockScript::ExecuteFunction(
    FunBindPoint bindPoint,
    BlockScriptBuilder* builder, 
    const Assembly& assembly,
    BsVmState& state,
    BsVm& vm,
    const void* inputBuffer,
    int   inputBufferSize,
    void* outputBuffer,
    int   outputBufferSize
)
{
    if (bindPoint == FUN_INVALID_BIND_POINT || state.GetExecutionState() != BsVmState::Alive)
    {
        return false;
    }

    PG_ASSERT(bindPoint >= 0 && bindPoint < assembly.mFunBlockMap->Size());

    const FunMapEntry funMapEntry = (*assembly.mFunBlockMap)[bindPoint];

    const FunDesc* funDesc = funMapEntry.mFunDesc;

    //if there is a function description and the current state is in the global pointer
    if (funDesc != nullptr && state.GetStackLevels() == 0)
    {
        const Ast::StmtFunDec* funDec = funDesc->GetDec();
        if (funDec->GetReturnType()->GetByteSize() == outputBufferSize &&
            funDesc->GetInputArgumentsByteSize() == inputBufferSize)
        {
            //we allocte a temporal buffer if the result is big.
            if (outputBufferSize > CANON_REGISTER_BYTESIZE)
            {
                state.SetReg(Canon::R_RET, Canon::R_ESP); //our pointer to the area to have the returned value
                state.Grow(outputBufferSize);
                state.SetReg(Canon::R_ESP, state.GetReg(Canon::R_ESP) + outputBufferSize);
            }
            
            //first push the new stack
            PushFrameCommand(funDec->GetFrame(), state, nullptr);

            //save ip
            int savedIp = state.GetReg(Canon::R_IP);

            //registers have been saved, lets now set the address of this function
            state.SetReg(Canon::R_B,  funMapEntry.mAssemblyBlock);
            state.SetReg(Canon::R_IP, 0);

            //get the pointer for the stack base
            char* stackBase = state.Ram() + state.GetReg(Canon::R_SBP);

            //copy the inputs to the stack
            Utils::Memcpy(stackBase, inputBuffer, inputBufferSize);

            //run until we are done
#if PEGASUS_ENABLE_PROXIES
            int loopCount = 0;
            const int CheckTimeLoopCount = 100;
            Pegasus::Core::UpdatePegasusTime();
            double capturedTime = Pegasus::Core::GetPegasusTime();
#endif
            while (state.GetStackLevels() != 0)
            {
                vm.StepExecution(assembly, state);
#if PEGASUS_ENABLE_PROXIES
                bool checkTime = loopCount == CheckTimeLoopCount;
                if (checkTime)
                {
                    loopCount = 0;
                    Pegasus::Core::UpdatePegasusTime();
                    double newTime = Pegasus::Core::GetPegasusTime();
                    if (newTime - capturedTime > 4.0)
                    {
                        state.SetReg(Canon::R_IP, savedIp);
                        PG_FAILSTR("Blockscript is taking too long to execute. Infinite loop? breaking execution. Warning: this can leave the VM in a devastated state.");
                        return false;
                    }
                }
                ++loopCount;
                
#endif
            }

            //copy the result to the output buffer
            if (outputBufferSize <= CANON_REGISTER_BYTESIZE)
            {
                int* retPtr = state.GetRegBuffer() + Canon::R_RET;
                Utils::Memcpy(outputBuffer, retPtr, outputBufferSize);
            }
            else
            {
                char* retPtr = state.Ram() + state.GetReg(Canon::R_RET);
                Utils::Memcpy(outputBuffer, retPtr, outputBufferSize);
                state.Shrink(outputBufferSize);
                state.SetReg(Canon::R_ESP, state.GetReg(Canon::R_ESP) - outputBufferSize);
            }

            //save ip
            state.SetReg(Canon::R_IP, savedIp);

            return true;
            
        }
        else
        { 
            return false;
        }

    }
    else
    {
        return false;
    }
}

int Pegasus::BlockScript::ReadGlobalValue(
    GlobalBindPoint bindPoint,
    const Assembly& assembly,
    BsVmState& state,
    void* destBuffer,
    int   destBufferSize
)
{
    PG_ASSERT(bindPoint != GLOBAL_INVALID_BIND_POINT);
    const Ast::Idd* var = (*assembly.mGlobalsMap)[bindPoint].mVar;
    int offset = state.GetReg(Canon::R_G) + var->GetOffset();
    void* memory = state.Ram() + offset;
    int typeSize = var->GetTypeDesc()->GetByteSize();

    PG_ASSERT(typeSize <= destBufferSize);
    
    Utils::Memcpy(destBuffer, memory, typeSize);

    return typeSize;
}

void Pegasus::BlockScript::WriteGlobalValue(
    GlobalBindPoint bindPoint,
    const Assembly& assembly,
    BsVmState& state,
    const void* srcBuffer,
    int srcBufferSize
)
{
    PG_ASSERT(bindPoint != GLOBAL_INVALID_BIND_POINT);

    const Ast::Idd* var = (*assembly.mGlobalsMap)[bindPoint].mVar;
    int offset = state.GetReg(Canon::R_G) + var->GetOffset();
    void* memory = state.Ram() + offset;
    int typeSize = var->GetTypeDesc()->GetByteSize();

    PG_ASSERT(srcBufferSize <= typeSize);

    Utils::Memcpy(memory, srcBuffer, srcBufferSize);
}
