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

#include "Pegasus/UnitTests/UtilsTests.h"
#include <vector>
#include <string>
#include <set>

#define RUN_TEST(name) RunTests(UNIT_TEST_##name, #name, context)

struct TestContext
{
    int successCount = 0;
    int totals = 0;
};


typedef bool(*TestFunc)(void);

bool RunTests(TestFunc func, const char * testTitle, TestContext& context);

void runUtilitySuite(TestContext& context)
{
    ///////////////////////////////////////////////////////////////////
    // UNIT TESTS - add here your UTILS package unit tests executions//
    ///////////////////////////////////////////////////////////////////

    //memcpy
    RUN_TEST(Memcpy1);
    RUN_TEST(Memcpy2);
    RUN_TEST(Memcpy3);

    //memset
    RUN_TEST(Memset1);
    RUN_TEST(Memset2);
    RUN_TEST(Memset3);
    RUN_TEST(Memset4);

    //strcmp
    RUN_TEST(Strcmp1);
    RUN_TEST(Strcmp2);
    RUN_TEST(Strcmp3);
    RUN_TEST(Strcmp4);

    //stricmp
    RUN_TEST(Stricmp1);
    RUN_TEST(Stricmp2);
    RUN_TEST(Stricmp3);

    //strlen
    RUN_TEST(Strlen1);
    RUN_TEST(Strlen2);
    RUN_TEST(Strlen3);

    //strchr
    RUN_TEST(Strchr1);
    RUN_TEST(Strchr2);
    RUN_TEST(Strchr3);

    //strrchr
    RUN_TEST(Strrchr1);
    RUN_TEST(Strrchr2);
    RUN_TEST(Strrchr3);

    //strcat
    RUN_TEST(Strcat1);
    RUN_TEST(Strcat2);
    RUN_TEST(Strcat3);
    RUN_TEST(Strcat4);
    RUN_TEST(Strcat5);
    RUN_TEST(Strcat6);

    //StringHash
    RUN_TEST(HashStr);

    //Atoi
    RUN_TEST(Atoi1);
    RUN_TEST(Atoi2);
    RUN_TEST(Atoi3);

    //Atoi
    RUN_TEST(Atof1);
    RUN_TEST(Atof2);
    RUN_TEST(Atof3);

    //TesselationTable
    RUN_TEST(TesselationTable1);
    RUN_TEST(TesselationTable2);
    RUN_TEST(TesselationTable3);
    RUN_TEST(TesselationTable4);

    //Vector
    RUN_TEST(Vector1);
    RUN_TEST(Vector2);

    //ByteStream
    RUN_TEST(ByteStream1);
    RUN_TEST(ByteStream2);
    RUN_TEST(ByteStream3);    

    ///////////////////////////////////////////////////////////
}

void runRenderSuite(TestContext& context)
{
    ///////////////////////////////////////////////////////////////////
    // UNIT TESTS - add here your RENDER package unit tests executions//
    ///////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////
}
//! Utility function, presents and runs unit tests to tty
bool RunTests(TestFunc func, const char * testTitle, TestContext& context)
{
    printf("***********************\n");
    printf("RUNNING TEST: %s\n", testTitle);
    printf("***********************\n");
    bool result = func();
    context.successCount += result ? 1 : 0;
    ++context.totals;
    printf("Result: %s", result ? "SUCCESS" : "FAIL");
    printf("\n-------------------------\n\n");
    return result;
}


typedef void (*TestSuiteFn)(TestContext& context);
struct TestSuite
{
	const char* name;
	TestSuiteFn fn;
};

const TestSuite gSuites[] = {
	{"Utilities", runUtilitySuite},
	{"Rendering", runRenderSuite},
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
        gSuites[suiteIndex].fn(context);
    }
    
    printf("Final Results: %d out of %d succeeded\n", context.successCount, context.totals);
    return 0;
}
