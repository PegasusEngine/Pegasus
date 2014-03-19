/****************************************************************************************/
/*                                                                                      */
/*                                       Pegasus                                        */
/*                                                                                      */
/****************************************************************************************/

//! \file   UtilsUnitTests.cpp
//! \author Kleber Garcia
//! \date   2/15/2014
//! \brief  Set of unit tests, used to prove soundness of 
//!         any data structure. To run, edit Utils project to generate an executable, and run

#if PEGASUS_DEV
#include "Pegasus\Utils\Memcpy.h"
#include "Pegasus\Utils\String.h"
#include <stdio.h>

typedef bool (*TestFunc)(void);

//********* UNIT TEST DECLARATIONS **********//

bool UNIT_TEST_MemcpyUnitTest1()
{
    //Test
    const char * testString1 = "This is a test string."; int count = 23;//size of memory is 
    char destBuffer[100];
    for (int i = 0; i < 100; ++i) destBuffer[i] = 99; //initialize all memory to 99
    Pegasus::Utils::Memcpy(destBuffer, testString1,count);

    //compare
    bool match = true;
    for (int i = 0; i < count; ++i) match = match && testString1[i] == destBuffer[i];
    for (int i = count; i < 100; ++i) match = match && destBuffer[i] == 99;

    return match;
}


bool UNIT_TEST_MemcpyUnitTest2()
{
    //Test 
    const char * testString1 = ""; int count = 0;
    char destBuffer[100];
    for (int i = 0; i < 100; ++i) destBuffer[i] = 21; //initialize all memory to 21
    Pegasus::Utils::Memcpy(destBuffer, testString1,count);

    //compare
    bool match = true;    
    for (int i = 0; i < 100; ++i) match = match && destBuffer[i] == 21;

    return match;
}

bool UNIT_TEST_MemcpyUnitTest3()
{
    //Test 
    unsigned long long bigList[256];
    unsigned long long destList[256];
    for (int i = 0; i < 256; ++i) bigList[i] =  i;
    Pegasus::Utils::Memcpy(destList, bigList, 256 * sizeof(unsigned long long));

    //compare
    bool match = true;    
    for (int i = 0; i < 256; ++i) match = match && destList[i] == i;

    return match;
}

bool UNIT_TEST_Strcmp1()
{
    const char * c1 = "ThisIsAString";
    const char * c2 = "ThisIsAString";
    return !Pegasus::Utils::Strcmp(c1, c2);
}

bool UNIT_TEST_Strcmp2()
{
    const char * c1 = "ThisIsAString";
    const char * c2 = "ThisIsA";
    return Pegasus::Utils::Strcmp(c1, c2) == -static_cast<int>('S');
}

bool UNIT_TEST_Strcmp3()
{
    const char * c1 = "ThisIsAString";
    const char * c2 = "ThisIsAstring";
    return Pegasus::Utils::Strcmp(c1, c2) == static_cast<int>('S' - 's');
}

bool UNIT_TEST_Strcmp4()
{
    const char * c1 = "";
    const char * c2 = "ThisIsAstring";
    return Pegasus::Utils::Strcmp(c1, c2) == static_cast<int>('T');
}

bool UNIT_TEST_Stricmp1()
{
    const char * c1 = " \t ThisIsAString+*-&\\";
    const char * c2 = " \t ThIsISastrIng+*-&\\";
    return !Pegasus::Utils::Stricmp(c1, c2);
}

bool UNIT_TEST_Stricmp2()
{
    const char * c1 = " \t ThisIsAString+*-& ";
    const char * c2 = " \t ThIsISastrIng+*-&\\";
    return Pegasus::Utils::Stricmp(c1, c2) == static_cast<int>(' ' - '\\');
}

bool UNIT_TEST_Stricmp3()
{
    const char * c1 = " ThisIsAString+*-& ";
    const char * c2 = "";
    return Pegasus::Utils::Stricmp(c1, c2) == -static_cast<int>(' ');
}

bool UNIT_TEST_Strlen1()
{
    const char * c1 = "1234567890";
    return Pegasus::Utils::Strlen(c1) == 10;
}

bool UNIT_TEST_Strlen2()
{
    const char * c1 = "";
    return Pegasus::Utils::Strlen(c1) == 0;
}

bool UNIT_TEST_Strlen3()
{
    const char * c1 = "1";
    return Pegasus::Utils::Strlen(c1) == 1;
}


bool UNIT_TEST_Strchr1()
{
    const char * c1 = "01234567890";
    return Pegasus::Utils::Strchr(c1, '0') == c1;
}

bool UNIT_TEST_Strchr2()
{
    const char * c1 = "01234567890";
    return Pegasus::Utils::Strchr(c1, '9') == (c1 + 9);
}

bool UNIT_TEST_Strchr3()
{
    const char * c1 = "01234567890";
    return Pegasus::Utils::Strchr(c1, '6') == (c1 + 6);
}

bool UNIT_TEST_Strrchr1()
{
    const char * c1 = "01234567890";
    return Pegasus::Utils::Strrchr(c1, '0') == c1 + 10;
}

bool UNIT_TEST_Strrchr2()
{
    const char * c1 = "01234567890";
    return Pegasus::Utils::Strrchr(c1, '9') == (c1 + 9);
}

bool UNIT_TEST_Strrchr3()
{
    const char * c1 = "x1234567890";
    return Pegasus::Utils::Strrchr(c1, 'x') == (c1);
}
///////////////////////////////////////////////

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

//! UNCOMMENT THIS TO COMPILE AS UNIT TEST PROJECT!
#if 0
int main()
{
    int successes = 0;
    int total = 0;
#define RUN_TEST(fn, name) RunTests(fn, name, successes, total)
    
    ///////////////////////////////////////////////////////////
    // UNIT TESTS - add here your utils unit tests executions//
    ///////////////////////////////////////////////////////////
    
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

#endif

#else
// The following line is added in order suppress the MS Visual C++ Linker warning 4221
// warning LNK4221: no public symbols found; archive member will be inaccessible
// This warning occurs on PC and XBOX when a file compiles out completely
// has no externally visible symbols which may be dependant on configuration
// #defines and options.

namespace { char NoEmptyFileDummy_vivalafrance; }

#endif
