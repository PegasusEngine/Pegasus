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
#include <stdio.h>

typedef bool (*TestFunc)(void);


//! Utility function, presents and runs unit tests to tty
bool RunTests(TestFunc func, const char * testTitle, int& outSucceses, int& outTotals)
{
    printf("***********************\n");
    printf("RUNNING TEST: %s\n", testTitle);
    printf("***********************\n");
    bool result = func();
    outSucceses += result ? 1 : 0;
    ++outTotals;
    printf("Result: %s", result ? "SUCCESS" : "FAIL");
    printf("\n-------------------------\n\n");
    return result;
}

int main()
{
    int successes = 0;
    int total = 0;
#define RUN_TEST(fn, name) RunTests(fn, name, successes, total)
    
    ///////////////////////////////////////////////////////////////////
    // UNIT TESTS - add here your UTILS package unit tests executions//
    ///////////////////////////////////////////////////////////////////
    
    //memcpy
    RUN_TEST(UNIT_TEST_MemcpyUnitTest1,"MemCpy1");
    RUN_TEST(UNIT_TEST_MemcpyUnitTest2,"MemCpy2");
    RUN_TEST(UNIT_TEST_MemcpyUnitTest3,"MemCpy3");

    //strcmp
    RUN_TEST(UNIT_TEST_Strcmp1,"strcmp1");
    RUN_TEST(UNIT_TEST_Strcmp2,"strcmp2");
    RUN_TEST(UNIT_TEST_Strcmp3,"strcmp3");
    RUN_TEST(UNIT_TEST_Strcmp4,"strcmp4");

    //stricmp
    RUN_TEST(UNIT_TEST_Stricmp1,"stricmp1");
    RUN_TEST(UNIT_TEST_Stricmp2,"stricmp2");
    RUN_TEST(UNIT_TEST_Stricmp3,"stricmp3");

    //strlen
    RUN_TEST(UNIT_TEST_Strlen1,"Strlen1");
    RUN_TEST(UNIT_TEST_Strlen2,"Strlen2");
    RUN_TEST(UNIT_TEST_Strlen3,"Strlen3");

    //strchr
    RUN_TEST(UNIT_TEST_Strchr1,"Strchr1");
    RUN_TEST(UNIT_TEST_Strchr2,"Strchr2");
    RUN_TEST(UNIT_TEST_Strchr3,"Strchr3");

    //strrchr
    RUN_TEST(UNIT_TEST_Strrchr1,"Strrchr1");
    RUN_TEST(UNIT_TEST_Strrchr2,"Strrchr2");
    RUN_TEST(UNIT_TEST_Strrchr3,"Strrchr3");

    ///////////////////////////////////////////////////////////

    printf("Final Results: %d out of %d succeeded", successes, total);
}
