/****************************************************************************************/
/*                                                                                      */
/*                                    Pegasus Unit Tests                                */
/*                                                                                      */
/****************************************************************************************/

//! \file   UtilsTests.cpp
//! \author Kleber Garcia
//! \date   30th March 2014
//! \brief  Pegasus Unit tests for the Utils package, implementation

#include "Pegasus/Memory/MallocFreeAllocator.h"
#include "Pegasus/UnitTests/UtilsTests.h"
#include "Pegasus/Utils/Memset.h"
#include "Pegasus/Utils/Memcpy.h"
#include "Pegasus/Utils/String.h"
#include "Pegasus/Utils/TesselationTable.h"

static Pegasus::Memory::MallocFreeAllocator sGlobalAllocator(0);

bool UNIT_TEST_Memcpy1()
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


bool UNIT_TEST_Memcpy2()
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

bool UNIT_TEST_Memcpy3()
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

bool UNIT_TEST_Memset1()
{
    char p = 100; 
    Pegasus::Utils::Memset8(&p, '9', 1);
    return p == '9';
}

bool UNIT_TEST_Memset2()
{
    int values[100];
    for (int i = 0; i < 100; ++i) values[i] = i;
    Pegasus::Utils::Memset8(values, 0, sizeof(values));
    for (int i = 0; i < 100; ++i) if (values[i] != 0) return false;
    return true;
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

bool UNIT_TEST_Strcat1()
{
    const char * src1 = "Hello";
    const char * src2 = "World";
    char dest[64];
    dest[0] = '\0';
    dest[11] = 99; //sentinel
    Pegasus::Utils::Strcat(dest, src1);
    Pegasus::Utils::Strcat(dest, src2);
    return !Pegasus::Utils::Strcmp(dest, "HelloWorld") && dest[10] == '\0' && dest[11] == 99;
}


bool UNIT_TEST_Strcat2()
{
    const char * src1 = "B";
    char dest[64];
    dest[0] = '\0';
    Pegasus::Utils::Strcat(dest, src1);
    return !Pegasus::Utils::Strcmp(dest, src1);
}

bool UNIT_TEST_Strcat3()
{
    const char * src1 = "";
    const char * src2 = "";
    char dest[64];
    dest[0] = '\0';
    Pegasus::Utils::Strcat(dest, src1);
    Pegasus::Utils::Strcat(dest, src2);
    return !Pegasus::Utils::Strcmp(dest, "");
}


bool UNIT_TEST_Strcat4()
{
    char p[3] = "";
    p[2] = 99;//sentinel
    Pegasus::Utils::Strcat(p,0);
    return !Pegasus::Utils::Strcmp(p,"0") && p[2] == 99;
}

bool UNIT_TEST_Strcat5()
{
    char p[3] = "";
    p[2] = 99;//sentinel
    Pegasus::Utils::Strcat(p,9);
    return !Pegasus::Utils::Strcmp(p,"9") && p[2] == 99;
}

bool UNIT_TEST_Strcat6()
{
    char p[5] = "";
    p[4] = 99;//sentinel
    Pegasus::Utils::Strcat(p,128);
    return !Pegasus::Utils::Strcmp(p,"128") && p[4] == 99;
}

bool UNIT_TEST_Atoi1()
{
    const char * n = "-1234567890";
    return Pegasus::Utils::Atoi(n) == -1234567890;
}

bool UNIT_TEST_Atoi2()
{
    const char * n = "-0000000";
    return Pegasus::Utils::Atoi(n) == 0;
}

bool UNIT_TEST_Atoi3()
{
    const char * n = "1";
    return Pegasus::Utils::Atoi(n) == 1;
}

bool UNIT_TEST_TesselationTable1()
{
    Pegasus::Utils::TesselationTable t (&sGlobalAllocator, sizeof(int));

    const int a = 992;
    const int b = 290;
    const int c = 2901375;
    t.Insert(0,0, a);
    t.Insert(29, 2, b);
    t.Insert(8, 45, c);

    int v1;
    int v2;
    int v3;

    bool r1 = t.Get(0,0,v1);
    bool r2 = t.Get(29, 2, v2);
    bool r3 = t.Get(8, 45, v3);
    
    return r1 && r2 && r3 && v1 == a && v2 == b && v3 == c;
}

bool UNIT_TEST_TesselationTable2()
{
    struct IntegrityCheck
    {
        char a;
        char b;
        char c;
        long long p;
    } integrityObject = { 0x1, 0x9, 0x5, 90099};

    Pegasus::Utils::TesselationTable t (&sGlobalAllocator, sizeof(integrityObject));

    for (int r = 0; r < 2024; ++r)
    {
        for (int c = 0; c <= r; ++c)
        {
            //do a checksum
            IntegrityCheck o = integrityObject;
            o.p = r * c + c;
            t.Insert(r,c,o); 
        }
    }

    bool check = true;
    
    for (int r = 0; r < 2024; ++r)
    {
        for (int c = 0; c <= r; ++c)
        {
            //do a checksum
            IntegrityCheck o = {0x0, 0x0, 0x0, 0x0};
            bool found = t.Get(r,c,o); 
            if (!found || o.a != integrityObject.a || o.b != integrityObject.b || o.c != integrityObject.c || o.p != r * c + c)
            {
                check = false;
            }
        }
    }

    return check;
}

bool UNIT_TEST_TesselationTable3()
{
    Pegasus::Utils::TesselationTable t (&sGlobalAllocator, sizeof(int));
    int val = 99;
    t.Insert(1,3, val);
    int res = 0;
    t.Get(3,1, res);
    t.Insert(511,3, val);
    int res2;
    t.Get(3,511,res2);

    return (res == val) && (res2 == val);
}

bool UNIT_TEST_TesselationTable4()
{
    
    Pegasus::Utils::TesselationTable t (&sGlobalAllocator, sizeof(int));
    int val = 99;
    t.Insert(789, 23, val);
    t.Clear();
    int res1 = 0;
    t.Get(789,23, res1);
    t.Insert(23, 789, val);
    int res2 = 0;
    t.Get(789,23, res2);

    return res1 == 0 && res2 == val;
}
