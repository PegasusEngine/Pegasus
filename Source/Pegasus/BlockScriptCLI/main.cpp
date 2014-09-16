/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/
//! \file   main.cpp (BlockScriptCLI)
//! \author Kleber Garcia
//! \brief  main entry point for block script cli 

#include "Pegasus/BlockScript/BlockScriptAst.h"
#include "Pegasus/BlockScript/BlockScript.h"
#include "Pegasus/BlockScript/PrettyPrint.h"
#include "Pegasus/Core/Io.h"
#include "Pegasus/Memory/MemoryManager.h"
#include "Pegasus/Core/Shared/LogChannel.h"
#include "Pegasus/Core/Log.h"
#include "Pegasus/Core/Assertion.h"
#include "Pegasus/BlockScript/Container.h"
#include <stdio.h>

using namespace Pegasus::Io;
using namespace Pegasus::Memory;
using namespace Pegasus::Core;

void LogHandler(LogChannel channel, const char * msg)
{
    printf("log: %s\n", msg);
}

#if PEGASUS_ENABLE_ASSERT
AssertReturnCode AssertHandler(const char * testStr,
                               const char * fileStr,
                               int line,
                               const char * msgStr)
{
    printf("Assert!!\n line:%d\n file:%s\n msg:(%s) %s\n", line, fileStr, testStr, msgStr);
    return ASSERTION_CONTINUE ;
}
#endif

void printstr(const char * s)
{
    printf("%s",s);
}

void printint(int i)
{
    printf("%d",i);
}

void printfloat(float f)
{
    printf("%f",f);
}

int main(int argc, char* argv[])
{
#if PEGASUS_ENABLE_ASSERT
    LogManager::CreateInstance(GetGlobalAllocator());
    LogManager::GetInstance()->RegisterHandler(LogHandler);
    AssertionManager::CreateInstance(GetGlobalAllocator());
    AssertionManager::GetInstance()->RegisterHandler(AssertHandler);
#endif
	FileBuffer fb;
	IoError err;
    IOManager mgr("");
    if (argc == 2)
    {
        err = mgr.OpenFileToBuffer(
            argv[1],
            fb,
            true,
            GetGlobalAllocator()    
        );

		if (err == ERR_NONE)
        {
			Pegasus::BlockScript::BlockScript bs(GetGlobalAllocator());
			bool res = bs.Compile(&fb);
            if (!res)
            {
                printf("invalid program!");
            }
            else
            {
                Pegasus::BlockScript::PrettyPrint pp(printstr, printint, printfloat);
				pp.Print(bs.GetAst());
            }
			bs.Reset();	
		}
        else
        {
            printf("io error!\n");
        }
    }
    else
    {
        printf("usage: BlockScriptCLI.exe <bs_script>");
    }
	
}


