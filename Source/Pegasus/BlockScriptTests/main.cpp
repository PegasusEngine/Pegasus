/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/
//! \file   main.cpp (BlockScriptTests)
//! \author Kleber Garcia
//! \brief  block script unit tests.

#include "Pegasus/BlockScript/BlockScriptAst.h"
#include "Pegasus/BlockScript/BlockScript.h"
#include "Pegasus/BlockScript/FunCallback.h"
#include "Pegasus/BlockScript/PrettyPrint.h"
#include "Pegasus/Utils/ByteStream.h"
#include "Pegasus/Utils/String.h"
#include "Pegasus/Core/Io.h"
#include "Pegasus/Memory/MemoryManager.h"
#include "Pegasus/Core/Shared/LogChannel.h"
#include "Pegasus/Core/Log.h"
#include "Pegasus/Core/Assertion.h"
#include "Pegasus/BlockScript/Container.h"
#include "Pegasus/BlockScript/BlockScriptManager.h"

#include <sstream>
#include <string>
#include <iostream>

using namespace std;
using namespace Pegasus::Io;
using namespace Pegasus::Memory;
using namespace Pegasus::Core;
using namespace Pegasus::BlockScript;
using namespace Pegasus::Utils;

bool gUseCout = false;
ByteStream* gSs;

#define DEFAULT_ROOT "../../../../Source/Pegasus/BlockScriptTests/Tests/"

struct CmdLineOptions
{
    bool mPrintHelp;
    bool mDisableCR;
    const char* mSingleScript;
    const char* mRootFolder;
    CmdLineOptions() : mPrintHelp(false), mDisableCR(false), mSingleScript(nullptr), mRootFolder(nullptr) 
    {
    }

} gCmdLineOpts;

void PrintHelp()
{
    cout << "Options: " << std::endl;
    cout << "-h Print help menu." << std::endl;
    cout << "-s Single script test, followed by the target script" << std::endl;
    cout << "-r Root folder to load scripts. Default is hard coded as" << DEFAULT_ROOT << std::endl;
    cout << "-c Disable carriage return, flat new lines." << std::endl;
    
}

bool ParseCmdLine(int argc, const char** argv, CmdLineOptions& outCmdLine)
{
    int i = 1;
    while (i < argc)
    {
        if (argv[i][0] == '-')
        {
            if (argv[i][1] == 'h')
            {
                outCmdLine.mPrintHelp = true;
                ++i;
            }
            else if (argv[i][1] == 's')
            {
                if (i == argc - 1) return false;
                ++i;
                outCmdLine.mSingleScript = argv[i];
                ++i;
            }
            else if (argv[i][1] == 'c')
            {
                ++i;
                outCmdLine.mDisableCR = true;
            }
            else if (argv[i][1] == 'r')
            {
                if (i == argc - 1) return false;
                ++i;
                outCmdLine.mRootFolder = argv[i];
                ++i;
            }
        }
        else
        {
            return false;
        }
    }

    return true;
}



// **** Regular Tests ****
// Add here all the scripts that will just run dumbly.
// **** **** ****
const struct TestScript { const char* script; const char* output; } gTestScripts[] = {
    { "HelloWorld.bs",     "OutputHelloWorld.txt" },
    { "Fibonacci.bs",      "OutputFibonacci.txt" },
    { "Structs.bs",        "OutputStructs.txt" },
    { "Branching.bs",      "OutputBranching.txt" },    
    { "Loops.bs",          "OutputLoops.txt" },
    { "2dArray.bs",        "Output2dArray.txt" },
    { "Math.bs",           "OutputMath.txt" }
};
//


// **** C++ Library Tests ****
// Add here all the tests that will require an extra library to be linked (library coming from c++)
// **** **** ****
/////

// **** BlockScript Fun call tests ****
// Add here all the tests that call a fuction from blockscript
// Using the answer structure
// **** **** ****
/////


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

void appendnl()
{
    if (gCmdLineOpts.mDisableCR)
    {
        char nl = '\n';
        gSs->Append(&nl, 1);
    }
    else
    {
        char nl[] = { '\r', '\n' };
        gSs->Append(nl, 2);
    }
}

int printstr(const char * s)
{
    gSs->Append(s, Strlen(s));
    
    char nl[] = { '\r', '\n' };
    appendnl();
    return 0;
}

int printint(int i)
{
    char buff[64];
    sprintf_s(buff, 64, "%d", i);
    gSs->Append(buff, Strlen(buff));
    appendnl();
    return 0;
}

int printfloat(float f)
{
    char buff[64];
    sprintf_s(buff, 64, "%f", f);
    appendnl();
    gSs->Append(buff, Strlen(buff));
    appendnl();
    return 0;
}

bool RunTest(IOManager& ioMgr, const char* script, const char* outputFile, bool dumpOutput = false)
{
    Pegasus::BlockScript::BlockScriptManager bsManager(GetGlobalAllocator());
    Pegasus::BlockScript::BlockScript* bs = bsManager.CreateBlockScript();
    FileBuffer filebuffer;
    IoError err = ioMgr.OpenFileToBuffer(script, filebuffer, true, GetGlobalAllocator());
    bool result = false;
    if (err == Pegasus::Io::ERR_NONE)
    {
        Pegasus::BlockScript::BsVmState vmState;
        vmState.Initialize(GetGlobalAllocator());
        bool compilerRes = bs->Compile(&filebuffer);
        if (compilerRes)
        {       
            bs->Run(&vmState);

            char z = '\0';
            gSs->Append(&z,1);
            if (dumpOutput)
            {
                cout << static_cast<const char*>(gSs->GetBuffer());
            }

            if (outputFile!=nullptr)
            {
                FileBuffer answerBuffer;
                err = ioMgr.OpenFileToBuffer(outputFile, answerBuffer, true, GetGlobalAllocator());
                if (err == Pegasus::Io::ERR_NONE)
                {
                    result = answerBuffer.GetFileSize() == gSs->GetSize() - 1;
                    for (int i = 0; result && i < answerBuffer.GetFileSize(); ++i)
                    {
                        if (static_cast<const char*>(gSs->GetBuffer())[i] != answerBuffer.GetBuffer()[i])
                        {
                            result = false;
                        }
                    }
                }
                else
                {
                    cout << "Error opening output file: " << outputFile << "." << std::endl;
                }
            }
            gSs->Reset();
        }
        else
        {
            cout << "Compilation Error." << std::endl;
        }
    }
    else
    {
        cout << "Unable to open script file: " << script << std::endl;
    }

    bsManager.DestroyBlockScript(bs);
    return result;
    
}


int main(int argc, const char** argv)
{
#if PEGASUS_ENABLE_ASSERT
    LogManager::CreateInstance(GetGlobalAllocator());
    LogManager::GetInstance()->RegisterHandler(LogHandler);
    AssertionManager::CreateInstance(GetGlobalAllocator());
    AssertionManager::GetInstance()->RegisterHandler(AssertHandler);
#endif
    ByteStream ss(GetGlobalAllocator());
    gSs = &ss;
    //setup IO for the actual virtual machine:
    Pegasus::BlockScript::SystemCallbacks::gPrintStrCallback = printstr;
    Pegasus::BlockScript::SystemCallbacks::gPrintIntCallback = printint;
    Pegasus::BlockScript::SystemCallbacks::gPrintFloatCallback = printfloat;
    bool res = ParseCmdLine(argc, argv, gCmdLineOpts);
    if (!res)
    {
        cout << "Incorrect command line options" << std::endl;
        PrintHelp();
        return -1;
    }

    if (gCmdLineOpts.mPrintHelp)
    {
        PrintHelp();
        return 0;
    }

    if (gCmdLineOpts.mSingleScript == nullptr)
    {
        cout << "###############################################################" << std::endl;
        cout << "################ Pegasus BlockScript Unit Tests ###############" << std::endl;
        cout << "###############################################################" << std::endl;
        cout << std::endl;
    }

	FileBuffer fb;
	IoError err = ERR_NONE;
    IOManager mgr(gCmdLineOpts.mRootFolder == nullptr ? DEFAULT_ROOT : gCmdLineOpts.mRootFolder);
    int total = 0;
    int passTests = 0;
    if (gCmdLineOpts.mSingleScript != nullptr)
    {
        RunTest(mgr, gCmdLineOpts.mSingleScript, nullptr, true);
    }
    else
    {
        for (int i = 0; i < sizeof(gTestScripts)/sizeof(gTestScripts[0]); ++i)
        {
            cout << " Testing: " << gTestScripts[i].script  << std::endl;
            bool res = RunTest(mgr, gTestScripts[i].script, gTestScripts[i].output);
            passTests += res ? 1 : 0;
            ++total;
            cout << " Result: " << ( res ? "Pass" : "Fail")  <<  std::endl;
            cout << std::endl;
        }
    }

    if (gCmdLineOpts.mSingleScript == nullptr)
    {
        cout <<  "Passed " <<  passTests << " out of " << total << std::endl;
    }

    return 0;
}
