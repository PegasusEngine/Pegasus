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
#include "Pegasus/BlockScript/EventListeners.h"
#include "Pegasus/BlockScript/IFileIncluder.h"
#include "Pegasus/Utils/String.h"
#include "Pegasus/Utils/Memcpy.h"
#include "Pegasus/Core/Io.h"

using namespace Pegasus;
using namespace Pegasus::BlockScript;

#define BLOCKSCRIPT_MAX_DEFINE_STR_LEN 64

extern void Bison_BlockScriptParse(const Io::FileBuffer* fileBuffer, BlockScript::BlockScriptBuilder* builder, BlockScript::IFileIncluder* fileIncluder, BlockScript::Container<BlockScript::Preprocessor::Definition>* definitionList);

BlockScriptCompiler::BlockScriptCompiler(Alloc::IAllocator* allocator)
: mAllocator(allocator), mAst(nullptr), mFileIncluder(nullptr), mTitle("<No-Title>")
{
    mDefinitionList.Initialize(allocator);
    mBuilder.Initialize(mAllocator);
    mStrAllocator.Initialize(BLOCKSCRIPT_MAX_DEFINE_STR_LEN, mAllocator);
}

BlockScriptCompiler::~BlockScriptCompiler()
{
}

bool BlockScriptCompiler::Compile(const Io::FileBuffer* fb)
{
    mBuilder.BeginBuild(mTitle); 
    Bison_BlockScriptParse(fb, &mBuilder, mFileIncluder, &mDefinitionList);
    BlockScriptBuilder::CompilationResult cr;
	mBuilder.EndBuild(cr);
    mAst = cr.mAst;
    mAsm = cr.mAsm;
    return mAst != nullptr && mBuilder.GetErrorCount() == 0;
}

void BlockScriptCompiler::RegisterDefinitions(const char* definitionNames[], const char* definitionValues[], int definitionCounts)
{
    for (int i = 0; i < definitionCounts; ++i)
    {
        int nameLen = Utils::Strlen(definitionNames[i]) + 1;
        int valLen  = Utils::Strlen(definitionValues[i]) + 1;
        PG_ASSERT(nameLen < BLOCKSCRIPT_MAX_DEFINE_STR_LEN && valLen < BLOCKSCRIPT_MAX_DEFINE_STR_LEN);

        Preprocessor::Definition newDef;
        char* nameCpy = (char*)mStrAllocator.Alloc(nameLen, Alloc::PG_MEM_TEMP);
        Utils::Memcpy(nameCpy, definitionNames[i], nameLen);

        char* nameValCpy = (char*)mStrAllocator.Alloc(valLen, Alloc::PG_MEM_TEMP);
        Utils::Memcpy(nameValCpy, definitionValues[i], valLen);

        newDef.mName = nameCpy;
        newDef.mValue = nameValCpy;
        newDef.mBufferSize = valLen;
        mDefinitionList.PushEmpty() = newDef;
    }
}

void BlockScriptCompiler::Reset()
{
    mBuilder.Reset();
    mDefinitionList.Reset();
    mStrAllocator.Reset();
    mAst = nullptr;
}

BlockScript::FunBindPoint BlockScriptCompiler::GetFunctionBindPoint(
    const char* funName,
    const char* const* argTypes,
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


void BlockScriptCompiler::AddCompilerEventListener(Pegasus::BlockScript::IBlockScriptCompilerListener* listener)
{
    mBuilder.AddEventListener(listener);
}

