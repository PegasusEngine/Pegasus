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
void onError(const char * str) { printf("%d: %s [around token \"%s\"]\n", BS_line, str, BS_text); }
extern void Bison_BlockScriptParse(const Io::FileBuffer* fileBuffer, BlockScript::BlockScriptBuilder* builder, BlockScript::IddStrPool* iddStrPool);

BlockScript::BlockScript::BlockScript(Alloc::IAllocator* allocator)
: mAllocator(allocator), mAst(nullptr)
{
    mIddStrPool.Initialize(mAllocator);
    mBuilder.Initialize(mAllocator, onError);
}

BlockScript::BlockScript::~BlockScript()
{
    mIddStrPool.Clear();
}

bool BlockScript::BlockScript::Compile(const Io::FileBuffer* fb)
{
    mBuilder.BeginBuild(); 
    Bison_BlockScriptParse(fb, &mBuilder, &mIddStrPool);
    BlockScriptBuilder::CompilationResult cr;
	mBuilder.EndBuild(cr);
    mAst = cr.mAst;
    return mAst != nullptr && mBuilder.GetErrorCount() == 0;
}

void BlockScript::BlockScript::Reset()
{
    mBuilder.Reset();
    mIddStrPool.Clear();
    mAst = nullptr;
}

void BlockScript::BlockScript::Run() { /*todo implement*/ }


