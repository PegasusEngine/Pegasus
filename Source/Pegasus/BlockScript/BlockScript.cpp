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
#include "Pegasus/BlockScript/IBlockScriptCompilerListener.h"
#include "Pegasus/Core/Io.h"
#include "Pegasus/BlockScript/SymbolTable.h"
#include "Pegasus/BlockScript/BlockLib.h"

#include <stdio.h>

using namespace Pegasus;

BlockScript::BlockScript::BlockScript(Alloc::IAllocator* allocator, BlockLib* runtimeLib)
: BlockScript::BlockScriptCompiler(allocator), mRuntimeLib(runtimeLib)
{
}

BlockScript::BlockScript::~BlockScript()
{
}

bool BlockScript::BlockScript::Compile(const Io::FileBuffer* fb)
{
    //prepare runtime library
    mBuilder.GetSymbolTable()->RegisterChild(mRuntimeLib->GetSymbolTable());

    //compile
    return BlockScriptCompiler::Compile(fb);
}

void BlockScript::BlockScript::Run(BsVmState* vmState) 
{ 
    // rrrrrrrrun!! boy
    mVm.Run(GetAsm(), *vmState);
}

bool BlockScript::BlockScript::ExecuteFunction(
    BsVmState* vmState,
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
        GetAsm(),
        *vmState,
        mVm,
        inputBuffer,
        inputBufferSize,
        outputBuffer,
        outputBufferSize
    );
}
