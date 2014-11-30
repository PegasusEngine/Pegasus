/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   BlockScriptCompiler.cpp
//! \author Kleber Garcia
//! \date   November 28th, 2014
//! \brief  Base class defining the basic compiler operations

#include "Pegasus/BlockScript/BlockScriptCompiler.h"
#include "Pegasus/BlockScript/BlockScriptAst.h"
#include "Pegasus/BlockScript/IBlockScriptCompilerListener.h"
#include "Pegasus/Core/Io.h"

using namespace Pegasus;
using namespace Pegasus::BlockScript;

extern void Bison_BlockScriptParse(const Io::FileBuffer* fileBuffer, BlockScript::BlockScriptBuilder* builder);

BlockScriptCompiler::BlockScriptCompiler(Alloc::IAllocator* allocator)
: mAllocator(allocator), mAst(nullptr)
{
    mBuilder.Initialize(mAllocator);
}

BlockScriptCompiler::~BlockScriptCompiler()
{
}

bool BlockScriptCompiler::Compile(const Io::FileBuffer* fb)
{
    mBuilder.BeginBuild(); 
    Bison_BlockScriptParse(fb, &mBuilder);
    BlockScriptBuilder::CompilationResult cr;
	mBuilder.EndBuild(cr);
    mAst = cr.mAst;
    mAsm = cr.mAsm;
    return mAst != nullptr && mBuilder.GetErrorCount() == 0;
}

void BlockScriptCompiler::Reset()
{
    mBuilder.Reset();
    mAst = nullptr;
}

BlockScript::FunBindPoint BlockScriptCompiler::GetFunctionBindPoint(
    const char* funName,
    const char** argTypes,
    int argumentListCount
) const
{
    return Pegasus::BlockScript::GetFunctionBindPoint(
        &mBuilder,
        mAsm,
        funName,
        argTypes,
        argumentListCount
    );
}

void BlockScriptCompiler::SetCompilerEventListener(Pegasus::BlockScript::IBlockScriptCompilerListener* listener)
{
    mBuilder.SetEventListener(listener);
}

