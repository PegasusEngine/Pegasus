/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   BlockScript.h
//! \author Kleber Garcia
//! \date   1st September 2014
//! \brief  BlockScript master class. Represents a blockscript

#include "Pegasus/BlockScript/BlockScript.h"
#include "Pegasus/BlockScript/BlockScriptAst.h"
#include "Pegasus/Core/Io.h"

#include <stdio.h>

using namespace Pegasus;

extern int BS_line;
extern char* BS_text;
extern BlockScript::BlockScriptBuilder* BS_GlobalBuilder;
void onError(const char * str) { BS_GlobalBuilder->IncErrorCount(); printf("%d: %s [around token \"%s\"]\n", BS_line, str, BS_text); }
extern void Bison_BlockScriptParse(const Io::FileBuffer* fileBuffer, BlockScript::BlockScriptBuilder* builder);


BlockScript::BlockScript::BlockScript(Alloc::IAllocator* allocator)
: mAllocator(allocator), mAst(nullptr)
{
    mBuilder.Initialize(mAllocator, onError);
    mVmState.Initialize(allocator);
}

BlockScript::BlockScript::~BlockScript()
{
}

bool BlockScript::BlockScript::Compile(const Io::FileBuffer* fb)
{
    mBuilder.BeginBuild(); 
    Bison_BlockScriptParse(fb, &mBuilder);
    BlockScriptBuilder::CompilationResult cr;
	mBuilder.EndBuild(cr);
    mAst = cr.mAst;
    mAsm = cr.mAsm;
    return mAst != nullptr && mBuilder.GetErrorCount() == 0;
}

void BlockScript::BlockScript::Reset()
{
    mBuilder.Reset();
    mVmState.Reset();
    mAst = nullptr;
}

void BlockScript::BlockScript::Run() 
{ 
    // rrrrrrrrun!! boy
    mVm.Run(GetAsm(), mVmState);
}

BlockScript::FunBindPoint BlockScript::BlockScript::GetFunctionBindPoint(
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

bool BlockScript::BlockScript::ExecuteFunction(
    FunBindPoint functionBindPoint,
    void* inputBuffer,
    int   inputBufferSize,
    void* outputBuffer,
    int   outputBufferSize
)
{
    return Pegasus::BlockScript::ExecuteFunction(
        functionBindPoint,
        &mBuilder,
        mAsm,
        mVmState,
        mVm,
        inputBuffer,
        inputBufferSize,
        outputBuffer,
        outputBufferSize
    );
}

