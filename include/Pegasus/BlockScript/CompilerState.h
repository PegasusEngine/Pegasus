/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   CompilerState.h
//! \author Kleber Garcia
//! \date   January 27, 2015
//! \brief  Blockscript Internal Compiler state.

#ifndef PEGASUS_BS_COMPILER_STATE_H
#define PEGASUS_BS_COMPILER_STATE_H

#include "Pegasus/BlockScript/Preprocessor.h"
#include "Pegasus/Core/Assertion.h"

//fwd declarations
namespace Pegasus
{
    namespace Io
    {
        class FileBuffer;
    }
    
    namespace BlockScript
    {
        class BlockScriptBuilder;
    }
}

namespace Pegasus
{
namespace BlockScript
{
    class CompilerState 
    {
    public:
        BlockScript::Preprocessor mPreprocessor;
        BlockScript::BlockScriptBuilder* mBuilder;
        int mBufferPosition;
        const Io::FileBuffer* mFileBuffer;
        char mStringAccumulator[512];
        int  mStringAccumulatorPos;
        int  mLexerStateCount;


        CompilerState()
        : mBuilder(nullptr),
          mBufferPosition(0),
          mFileBuffer(nullptr),
          mStringAccumulatorPos(0),
          mLexerStateCount(0)
        {
        }

        void PushLexerState(int id);
        
        int  PopLexerState();

        int  GetLexerState();

        BlockScript::Preprocessor& GetPreprocessor() { return mPreprocessor; }

    private:
        int  mLexerStack[512];
    };
}
}

#endif
