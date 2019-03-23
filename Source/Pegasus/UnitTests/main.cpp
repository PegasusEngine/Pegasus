/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   main.cpp (unit tests entry point)
//! \author Kleber Garcia
//! \date   3/30/2014
//! \brief  Set of unit tests, used to prove soundness of 
//!         any data structure. To run, edit Utils project to generate an executable, and run
#include "Pegasus/UnitTests/UnitTests.h"
#include "Pegasus/UnitTests/UtilsTests.h"
#include "Pegasus/UnitTests/RenderTests.h"
#include <vector>
#include <string>
#include <set>

using namespace Pegasus::Tests;

//! Utility function, presents and runs unit tests to tty
bool RunTest(TestFunc func, TestHarness* harness, TestContext& context)
{
    printf("***********************\n");
    printf("RUNNING TEST: %s\n", func.name);
    printf("***********************\n");
    bool result = func.fn(harness);
    context.successCount += result ? 1 : 0;
    ++context.totals;
    printf("Result: %s", result ? "SUCCESS" : "FAIL");
    printf("\n-------------------------\n\n");
    return result;
}

typedef TestHarness* (*TestSuiteFn)();

struct TestSuite
{
	const char* name;
	TestSuiteFn fn;
};

const TestSuite gSuites[] = {
	{"Utilities", createUtilsHarness},
	{"Rendering", createRenderHarness},
};

struct CmdLineSettings
{
    bool printTests = false;
	bool printHelp = false;
    std::vector<int> suites;
};

bool parseCmdLine(int argc, char* argv[], CmdLineSettings& outSettings)
{
    std::set<std::string> suiteNames;

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "-t")
        {
            ++i;
            if (i > argc)
                return false;
            suiteNames.insert(argv[i]);
        }
        else if (arg == "-i")
        {
            outSettings.printTests = true;
            return true;
        }
		else if (arg == "-h")
		{
			outSettings.printHelp = true;
			return true;
		}
    }

    //Add all
    if (suiteNames.empty())
    {
        for (int i = 0; i < sizeof(gSuites)/sizeof(gSuites[0]); ++i)
            outSettings.suites.push_back(i);
    }
    else
    {
        for (std::string name : suiteNames)
        {
            int testIndex = -1;
            for (int sIdx = 0; sIdx < sizeof(gSuites)/sizeof(gSuites[0]); ++sIdx)
            { 
                if (gSuites[sIdx].name == name)
                {
                    testIndex = sIdx;
                    break;
                }
            }

            if (testIndex == -1)
                return false;

            outSettings.suites.push_back(testIndex);
        }
    }

	return true;
}


void printInfo()
{
    printf("*****************************\n");
    printf("Pegasus Unit Test Program V0.1\n");
    printf("*****************************\n\n");
    printf("Args:\n");
    printf("-t <testName>\n");
	printf("    For unique tests names just put the desired test name there.\n");
	printf("    If we want multiple, use another - t arg.\n");
    printf("-i\n");
    printf("    prints lists of available tests.\n");
	printf("-h\n");
	printf("    prints help panel.\n");
}

int main(int argc, char* argv[])
{
    int successes = 0;
    int total = 0;
    CmdLineSettings settings;
    bool result = parseCmdLine(argc, argv, settings);
    if (!result || settings.printHelp)
    {
		if (!result)
			printf("Invalid argument passed!\n");
        printInfo();
        return 0;
    }

    if (settings.printTests)
    {
        printf("Available tests: \n");
        for (const auto& suite : gSuites)
        {
            printf("%s\n", suite.name);
        }
        return 0;
    }

    TestContext context;
    for (int suiteIndex : settings.suites)
    {
        TestHarness* harness = gSuites[suiteIndex].fn();
        harness->Initialize();
        int testCounts = 0; 
        const TestFunc* funcs = harness->GetTests(testCounts);
        for (int i = 0; i < testCounts; ++i)
            RunTest(funcs[i], harness, context);
        harness->Destroy();
        delete harness;
    }
    
    printf("Final Results: %d out of %d succeeded\n", context.successCount, context.totals);
    return 0;
}
