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
#include "Pegasus/BlockScript/IFileIncluder.h"
#include "Pegasus/Utils/String.h"
#include "Pegasus/Core/Io.h"

using namespace Pegasus;
using namespace Pegasus::BlockScript;

extern void Bison_BlockScriptParse(const Io::FileBuffer* fileBuffer, BlockScript::BlockScriptBuilder* builder, BlockScript::IFileIncluder* fileIncluder);

BlockScriptCompiler::BlockScriptCompiler(Alloc::IAllocator* allocator)
: mAllocator(allocator), mAst(nullptr), mFileIncluder(nullptr)
{
    mBuilder.Initialize(mAllocator);
}

BlockScriptCompiler::~BlockScriptCompiler()
{
}

bool BlockScriptCompiler::Compile(const Io::FileBuffer* fb)
{
    mBuilder.BeginBuild(); 
    Bison_BlockScriptParse(fb, &mBuilder, mFileIncluder);
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

GlobalBindPoint BlockScriptCompiler::GetGlobalBindPoint(const char* globalName) const
{
    if (mAsm.mGlobalsMap != nullptr)
    {
        for (int i = 0; i < mAsm.mGlobalsMap->Size(); ++i)
        {
            const Ast::Idd* var = (*mAsm.mGlobalsMap)[i].mVar;
            if (!Utils::Strcmp(globalName, var->GetName()))
            {
                return i;
            }
        }
    }
    
    return GLOBAL_INVALID_BIND_POINT;
}

const TypeDesc* BlockScriptCompiler::GetTypeDesc(GlobalBindPoint bindPoint) const
{
    PG_ASSERT(mAsm.mGlobalsMap != nullptr);
    if (mAsm.mGlobalsMap != nullptr && bindPoint >= 0 && bindPoint < mAsm.mGlobalsMap->Size())
    {
        return (*mAsm.mGlobalsMap)[bindPoint].mVar->GetTypeDesc();
    }
    return nullptr;
}


void BlockScriptCompiler::SetCompilerEventListener(Pegasus::BlockScript::IBlockScriptCompilerListener* listener)
{
    mBuilder.SetEventListener(listener);
}

