/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Unit Tests                                */
/*                                                                                      */
/****************************************************************************************/

//! \file   UtilsTests.cpp
//! \author Kleber Garcia
//! \date   30th March 2014
//! \brief  Pegasus Unit tests for the Utils package, implementation

#include "Pegasus/UnitTests/UtilsTests.h"
#include "Pegasus/Utils/Memcpy.h"
#include "Pegasus/Utils/String.h"

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
    unsigned long long bigList[267];
    unsigned long long destList[267];
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
