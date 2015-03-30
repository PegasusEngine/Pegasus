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
#include "Pegasus/Utils/Vector.h"
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
        class IFileIncluder;
    }

    namespace Alloc
    {
        class IAllocator;
    }
}

namespace Pegasus
{
namespace BlockScript
{
    class CompilerState 
    {
    public:
        struct DefineBufferEl
        {
            int  mBufferPosition;
            void* mLexerBufferId;
            const BlockScript::Preprocessor::Definition* mDef;
        };

        BlockScript::Preprocessor mPreprocessor;
        BlockScript::BlockScriptBuilder* mBuilder;
        int mBufferPosition;
        const Io::FileBuffer* mFileBuffer;
        char mStringAccumulator[512];
        int  mStringAccumulatorPos;

        CompilerState(Alloc::IAllocator* allocator)
        : mBuilder(nullptr),
          mBufferPosition(0),
          mFileBuffer(nullptr),
          mStringAccumulatorPos(0),
          mPreprocessor(allocator),
          mLexerStack(allocator),
          mDefineBufferStack(allocator)
        {
        }

        void PushLexerState(int id);
        
        int  PopLexerState();

        int  GetLexerState();

        BlockScript::Preprocessor& GetPreprocessor() { return mPreprocessor; }

        void PushDefineStack(void* bufferId, const BlockScript::Preprocessor::Definition* def);

        int  GetDefineStackCount() const;

        DefineBufferEl* GetDefineStackTop();

        void* PopDefineStack();

    private:

        Utils::Vector<int>  mLexerStack;
        Utils::Vector<DefineBufferEl>     mDefineBufferStack;
    };
}
}

#endif
