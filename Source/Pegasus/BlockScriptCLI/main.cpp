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
#include "Pegasus/BlockScript/FunCallback.h"
#include "Pegasus/BlockScript/PrettyPrint.h"
#include "Pegasus/Core/Io.h"
#include "Pegasus/Memory/MemoryManager.h"
#include "Pegasus/Core/Shared/LogChannel.h"
#include "Pegasus/Core/Log.h"
#include "Pegasus/Core/Assertion.h"
#include "Pegasus/BlockScript/Container.h"
#include "Pegasus/BlockScript/BlockScriptManager.h"
#include "Pegasus/BlockScript/EventListeners.h"
#include <stdio.h>

using namespace Pegasus::Io;
using namespace Pegasus::Memory;
using namespace Pegasus::Core;

class CompilerEventListener : public Pegasus::BlockScript::IBlockScriptCompilerListener
{
public:
    virtual void OnCompilationBegin()
    {}
    
    virtual void OnCompilationError(const char* compilationUnitTitle, int line, const char* errorMessage, const char* token)
    {
        printf("[%s:%d]: '%s'\n", compilationUnitTitle, line ,errorMessage);
    }
    
    virtual void OnCompilationEnd(bool success)
    {}
} gCompilerEventListener;


void LogHandler(LogChannel channel, const char * msg)
{
    if (channel == 'TEMP')
    {
        printf("log: %s\n", msg);
    }
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

int printstr(const char * s)
{
    return printf("%s",s);
}

int printint(int i)
{
    return printf("%d",i);
}

int printfloat(float f)
{
    return printf("%f",f);
}

struct Options
{
public:
    bool printAssembly;
    bool printAst;
    bool runScript;
    bool requestHelp;
    char* fileToParse;
    Options() : 
        printAssembly(false),
        printAst(false),
        runScript(true),
        requestHelp(false),
        fileToParse(nullptr)
    {
    }
};

bool ParseCommandLineOptions(char** argv, int argc, Options& output)
{
    if (argc < 2)
    {
        return false;
    }

    for (int i = 1; i < argc; ++i)
    {
        char* candidate = argv[i];
        if (candidate[0] == '-')
        {
            if (candidate[1] == 'a')
            {
                output.printAssembly = true;
            }
            else if (candidate[1] == 't')
            {
                output.printAst = true;
            }
            else if (candidate[1] == 'n')
            {
                output.runScript = false;
            }
            else if (candidate[1] == 'h')
            {
                output.requestHelp = true;
            }
            else
            {
                return false;
            }
        }
        else if (output.fileToParse == nullptr)
        {
            output.fileToParse = candidate;
        }
        else
        {
            return false;
        }
    }
    return true;
}

void printHelp()
{
    printf("###################################################\n");
    printf("############# Block Script CLI  ###################\n");
    printf("###################################################\n");
    printf("#########  by Kleber Garcia (c) 2014 ##############\n");
    printf("---------------------------------------------------\n\n");
    printf("usage: BlockScriptCLI.exe <bs_script> [<options>]\n");
    printf("Available options:\n");
    printf("-h print this help menu.\n");
    printf("-a print assembly.\n");
    printf("-t print the abstract syntax tree.\n");
    printf("-n Do not attempt to run the program.\n");
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
    Options opts;
    Pegasus::BlockScript::BlockScriptManager bsManager(GetGlobalAllocator());
    bool res = ParseCommandLineOptions(argv, argc, opts);
    if (!res)
    {
        printf("Invalid command line option! use -h command for help.");
        return -1;
    }
    else
    {
        if (opts.requestHelp)
        {
            printHelp();
        }
        else
        {
            err = mgr.OpenFileToBuffer(
                argv[1],
                fb,
                true,
                GetGlobalAllocator()    
            );
            Pegasus::BlockScript::BsVmState vmState;
            vmState.Initialize(GetGlobalAllocator());
		    if (err == ERR_NONE)
            {
                Pegasus::BlockScript::BlockScript* bs = bsManager.CreateBlockScript();
                bs->AddCompilerEventListener(&gCompilerEventListener);
                bool res = bs->Compile(&fb);
	
                if (!res)
                {
                    printf("invalid program!");
                    return -1;
                }
                else
                {
                    //setup IO for the actual virtual machine:
                    Pegasus::BlockScript::SystemCallbacks::gPrintStrCallback = printstr;
                    Pegasus::BlockScript::SystemCallbacks::gPrintIntCallback = printint;
                    Pegasus::BlockScript::SystemCallbacks::gPrintFloatCallback = printfloat;

                    Pegasus::BlockScript::PrettyPrint pp(printstr, printint, printfloat);
                    if (opts.printAst)
                    {
                        printf("----------------- SRC -------------------\n");
		    		    pp.Print(bs->GetAst());
                        printf("\n");
                    }

                    if (opts.printAssembly)
                    {
                        printf("\n----------------- ASM -------------------\n");
                        pp.PrintAsm(bs->GetAsm());
                        printf("\n");
                    }

                    if (opts.runScript)
                    {
                        bs->Run(&vmState);
                    }
                }
		    	
                bs->Reset();	
                vmState.Reset();
                bsManager.DestroyBlockScript(bs);
		    }
        }
    }
    
    return 0;
	
}


