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

using namespace Pegasus::BlockScript;


void CompilerState::PushLexerState(int id)
{
    PG_ASSERT(mLexerStateCount < 512);
    mLexerStack[mLexerStateCount++] = id;
}

int CompilerState::GetLexerState()
{
    PG_ASSERT(mLexerStateCount > 0);
    return mLexerStack[mLexerStateCount - 1];
}

int CompilerState::PopLexerState()
{
    PG_ASSERT(mLexerStateCount > 0);
    return mLexerStack[--mLexerStateCount];
}
