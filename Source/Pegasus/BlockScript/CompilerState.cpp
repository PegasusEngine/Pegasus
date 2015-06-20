/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   CompilerState.cpp
//! \author Kleber Garcia
//! \date   January 31, 2015
//! \brief  Blockscript Internal Compiler state.
#include "Pegasus/BlockScript/CompilerState.h"
#include "Pegasus/Utils/String.h"


using namespace Pegasus;
using namespace Pegasus::BlockScript;


void CompilerState::PushLexerState(int id)
{
    mLexerStack.PushEmpty() = id;
}

int CompilerState::GetLexerState()
{
    return mLexerStack[mLexerStack.GetSize() - 1];
}

int CompilerState::PopLexerState()
{
    return mLexerStack.Pop();
}

void  CompilerState::PushDefineStack(void* bufferId, const BlockScript::Preprocessor::Definition* def)
{
    CompilerState::DefineBufferEl& el = *(new(&(mDefineBufferStack.PushEmpty()))CompilerState::DefineBufferEl); 
    el.mBufferPosition = 0;
    el.mLexerBufferId = bufferId;
    el.mDef = def;
}

int   CompilerState::GetDefineStackCount() const
{
    return mDefineBufferStack.GetSize();
}

CompilerState::DefineBufferEl* CompilerState::GetDefineStackTop()
{
    return &mDefineBufferStack[mDefineBufferStack.GetSize() - 1];
}

void*  CompilerState::PopDefineStack()
{
    return mDefineBufferStack.Pop().mLexerBufferId;
}
