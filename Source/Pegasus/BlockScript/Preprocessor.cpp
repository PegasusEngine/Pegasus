/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   Preprocessor.cpp
//! \author Kleber Garcia
//! \date   January 31, 2015
//! \brief  Blockscript Internal Compiler Preprocessor directive

#include "Pegasus/BlockScript/Preprocessor.h"
#include "Pegasus/BlockScript/IFileIncluder.h"
#include "Pegasus/Allocator/IAllocator.h"
#include "Pegasus/Utils/String.h"

using namespace Pegasus;
using namespace Pegasus::BlockScript;

Preprocessor::Preprocessor(Alloc::IAllocator* allocator)
:    
     mDefinitions(allocator),
     mIncludeDefs(allocator),
     mStateStack(allocator),
     mHasInclude(false),
     mNextIncludeDefinition(nullptr),
     mFileIncluder(nullptr)
{
    NewState();
    Top().mIsChosePath = true;
}

Preprocessor::~Preprocessor()
{
}

Preprocessor::State& Preprocessor::Top()
{
    return mStateStack[mStateStack.GetSize() - 1];
}

const Preprocessor::State& Preprocessor::Top() const
{
    return mStateStack[mStateStack.GetSize() - 1];
}

void Preprocessor::NewState()
{
    Preprocessor::State& s = mStateStack.PushEmpty();
    s.mCommand = Preprocessor::PP_CMD_NONE;
    s.mStringArg = nullptr;
    s.mCodeArg = nullptr;
    s.mIsIfEnabled = true;
    s.mIsChosePath = false;
}

void Preprocessor::SetIfActive(bool active)
{
    Top().mIsIfEnabled = active;
}

void Preprocessor::SetChosePath(bool chosepath)
{
    Top().mIsChosePath = chosepath;
}

bool Preprocessor::ChosePath() const
{
    return Top().mIsChosePath;
}

bool Preprocessor::IsIfActive() const
{
    return Top().mIsIfEnabled;
}

void Preprocessor::PopState()
{
    mStateStack.Pop();
}

void Preprocessor::IncludeCmd()
{
    Top().mCommand = Preprocessor::PP_CMD_INCLUDE;
}

void Preprocessor::DefineCmd()
{
    Top().mCommand = Preprocessor::PP_CMD_DEFINE;
}

void Preprocessor::PushString(const char* str)
{
    Top().mStringArg = str;
}
void Preprocessor::IfCmd()
{
    Top().mCommand = Preprocessor::PP_CMD_IF;
}

void Preprocessor::ElseIfCmd()
{
    Top().mCommand = Preprocessor::PP_CMD_ELSEIF;
}

void Preprocessor::ElseCmd()
{
    Top().mCommand = Preprocessor::PP_CMD_ELSE;
}

void Preprocessor::EndIfCmd()
{
    Top().mCommand = Preprocessor::PP_CMD_ENDIF;
}

void Preprocessor::NoneCmd()
{
    Top().mCommand = Preprocessor::PP_CMD_NONE;
}

void Preprocessor::PushCode(const char* str)
{
    Top().mCodeArg = str;
}

const Preprocessor::Definition* Preprocessor::FindDefinitionByName(const char* name) const
{
    for (int i = 0; i < mDefinitions.GetSize(); ++i)
    {
        if (!Utils::Strcmp(mDefinitions[i].mName, name))
        {
            return &mDefinitions[i];
        }        
    }
    return nullptr;
}

bool Preprocessor::FlushCommand(const char** errString)
{
    *errString = nullptr;
    bool result = false;

    mHasInclude = false;
    
    switch (Top().mCommand)
    {
    case Preprocessor::PP_CMD_INCLUDE:
        {
            if (IsIfActive())
            {
                if (mFileIncluder == nullptr)
                {
                    *errString = "No file includer handler passed in the compilation process.";
                    result = false; //nop
                }
                else
                {
                    mNextIncludeDefinition = &mIncludeDefs.PushEmpty();
                    mNextIncludeDefinition->mName = nullptr;
                    mNextIncludeDefinition->mValue = nullptr;
                    mNextIncludeDefinition->mBufferSize = 0;                    
                    mNextIncludeDefinition->mIsInclude = true;
                    
                    result = mFileIncluder->Open(Top().mCodeArg, &mNextIncludeDefinition->mValue, mNextIncludeDefinition->mBufferSize);
                    if (result)
                    {
                        mHasInclude = true;
                    }
                    else
                    {
                        mFileIncluder->Close(mNextIncludeDefinition->mValue);
                    }
                }
                
            }
            else
            {
                result = true; //nop
            }
            break;
        }
    case Preprocessor::PP_CMD_DEFINE:
        {
            if (Top().mStringArg == nullptr)
            {
                result = false;
                *errString = "define must have a name passed!";
            }
            else if (FindDefinitionByName(Top().mStringArg) != nullptr)
            {
                result = false;
                *errString = "Definition already exists";
            }
            else if (IsIfActive())
            {
                Preprocessor::Definition& newDef = mDefinitions.PushEmpty();
                newDef.mName = Top().mStringArg;
                newDef.mValue = Top().mCodeArg;
                newDef.mIsInclude = false;
                newDef.mBufferSize = newDef.mValue == nullptr ? 0 : Utils::Strlen(newDef.mValue) + 1;
                result = true;
            }
            else
            {
                result = true;
            }
            break;
        }
    default:
        break;
    }
    
    //flush the arguments for the next command to be executed
    Top().mCommand = Preprocessor::PP_CMD_NONE;
    Top().mStringArg = nullptr;
    Top().mCodeArg = nullptr;
    return result;
}




