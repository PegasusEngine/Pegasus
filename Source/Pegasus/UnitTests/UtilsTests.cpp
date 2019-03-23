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
#include "Pegasus/UnitTests/UnitTests.h"
#include "Pegasus/Utils/Memset.h"
#include "Pegasus/Utils/Memcpy.h"
#include "Pegasus/Utils/String.h"
#include "Pegasus/Utils/TesselationTable.h"
#include "Pegasus/Utils/Vector.h"
#include "Pegasus/Utils/ByteStream.h"

namespace Pegasus
{
namespace Tests
{
class UtilsHarness : public TestHarness
{
    virtual void Initialize() {}
    virtual void Destroy() {}
    virtual const TestFunc* GetTests(int& funcSz) const;
};

TestHarness* createUtilsHarness()
{
    return new UtilsHarness();
}

static Pegasus::Memory::MallocFreeAllocator sGlobalAllocator(0);

bool runMemcpy1(TestHarness* harness)
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


bool runMemcpy2(TestHarness* harness)
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

bool runMemcpy3(TestHarness* harness)
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

bool runMemset1(TestHarness* harness)
{
    char p = 100; 
    Pegasus::Utils::Memset8(&p, '9', 1);
    return p == '9';
}

bool runMemset2(TestHarness* harness)
{
    int values[100];
    for (int i = 0; i < 100; ++i) values[i] = i + 2;
    Pegasus::Utils::Memset8(values, 1, sizeof(values));
    for (int i = 0; i < 100; ++i) if (values[i] != ((1 << 24) | (1 << 16) | (1 << 8) | 1)) return false;
    return true;
}

bool runMemset3(TestHarness* harness)
{
    unsigned long p = 100; 
    Pegasus::Utils::Memset32(&p, 0x12345678, 4);
    return p == 0x12345678;
}

bool runMemset4(TestHarness* harness)
{
    int values[100];
    for (int i = 0; i < 100; ++i) values[i] = i + 2;
    Pegasus::Utils::Memset32(values, 1, sizeof(values));
    for (int i = 0; i < 100; ++i) if (values[i] != 1) return false;
    return true;
}

bool runStrcmp1(TestHarness* harness)
{
    const char * c1 = "ThisIsAString";
    const char * c2 = "ThisIsAString";
    return !Pegasus::Utils::Strcmp(c1, c2);
}

bool runStrcmp2(TestHarness* harness)
{
    const char * c1 = "ThisIsAString";
    const char * c2 = "ThisIsA";
    return Pegasus::Utils::Strcmp(c1, c2) == -static_cast<int>('S');
}

bool runStrcmp3(TestHarness* harness)
{
    const char * c1 = "ThisIsAString";
    const char * c2 = "ThisIsAstring";
    return Pegasus::Utils::Strcmp(c1, c2) == static_cast<int>('S' - 's');
}

bool runStrcmp4(TestHarness* harness)
{
    const char * c1 = "";
    const char * c2 = "ThisIsAstring";
    return Pegasus::Utils::Strcmp(c1, c2) == static_cast<int>('T');
}

bool runStricmp1(TestHarness* harness)
{
    const char * c1 = " \t ThisIsAString+*-&\\";
    const char * c2 = " \t ThIsISastrIng+*-&\\";
    return !Pegasus::Utils::Stricmp(c1, c2);
}

bool runStricmp2(TestHarness* harness)
{
    const char * c1 = " \t ThisIsAString+*-& ";
    const char * c2 = " \t ThIsISastrIng+*-&\\";
    return Pegasus::Utils::Stricmp(c1, c2) == static_cast<int>(' ' - '\\');
}

bool runStricmp3(TestHarness* harness)
{
    const char * c1 = " ThisIsAString+*-& ";
    const char * c2 = "";
    return Pegasus::Utils::Stricmp(c1, c2) == -static_cast<int>(' ');
}

bool runStrlen1(TestHarness* harness)
{
    const char * c1 = "1234567890";
    return Pegasus::Utils::Strlen(c1) == 10;
}

bool runStrlen2(TestHarness* harness)
{
    const char * c1 = "";
    return Pegasus::Utils::Strlen(c1) == 0;
}

bool runStrlen3(TestHarness* harness)
{
    const char * c1 = "1";
    return Pegasus::Utils::Strlen(c1) == 1;
}


bool runStrchr1(TestHarness* harness)
{
    const char * c1 = "01234567890";
    return Pegasus::Utils::Strchr(c1, '0') == c1;
}

bool runStrchr2(TestHarness* harness)
{
    const char * c1 = "01234567890";
    return Pegasus::Utils::Strchr(c1, '9') == (c1 + 9);
}

bool runStrchr3(TestHarness* harness)
{
    const char * c1 = "01234567890";
    return Pegasus::Utils::Strchr(c1, '6') == (c1 + 6);
}

bool runStrrchr1(TestHarness* harness)
{
    const char * c1 = "01234567890";
    return Pegasus::Utils::Strrchr(c1, '0') == c1 + 10;
}

bool runStrrchr2(TestHarness* harness)
{
    const char * c1 = "01234567890";
    return Pegasus::Utils::Strrchr(c1, '9') == (c1 + 9);
}

bool runStrrchr3(TestHarness* harness)
{
    const char * c1 = "x1234567890";
    return Pegasus::Utils::Strrchr(c1, 'x') == (c1);
}

bool runStrcat1(TestHarness* harness)
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


bool runStrcat2(TestHarness* harness)
{
    const char * src1 = "B";
    char dest[64];
    dest[0] = '\0';
    Pegasus::Utils::Strcat(dest, src1);
    return !Pegasus::Utils::Strcmp(dest, src1);
}

bool runStrcat3(TestHarness* harness)
{
    const char * src1 = "";
    const char * src2 = "";
    char dest[64];
    dest[0] = '\0';
    Pegasus::Utils::Strcat(dest, src1);
    Pegasus::Utils::Strcat(dest, src2);
    return !Pegasus::Utils::Strcmp(dest, "");
}


bool runStrcat4(TestHarness* harness)
{
    char p[3] = "";
    p[2] = 99;//sentinel
    Pegasus::Utils::Strcat(p,0);
    return !Pegasus::Utils::Strcmp(p,"0") && p[2] == 99;
}

bool runStrcat5(TestHarness* harness)
{
    char p[3] = "";
    p[2] = 99;//sentinel
    Pegasus::Utils::Strcat(p,9);
    return !Pegasus::Utils::Strcmp(p,"9") && p[2] == 99;
}

bool runStrcat6(TestHarness* harness)
{
    char p[5] = "";
    p[4] = 99;//sentinel
    Pegasus::Utils::Strcat(p,128);
    return !Pegasus::Utils::Strcmp(p,"128") && p[4] == 99;
}

bool runAtoi1(TestHarness* harness)
{
    const char * n = "-1234567890";
    return Pegasus::Utils::Atoi(n) == -1234567890;
}

bool runAtoi2(TestHarness* harness)
{
    const char * n = "-0000000";
    return Pegasus::Utils::Atoi(n) == 0;
}

bool runAtoi3(TestHarness* harness)
{
    const char * n = "1";
    return Pegasus::Utils::Atoi(n) == 1;
}

bool runAtof1(TestHarness* harness)
{
    const char * n = "3.0";
    float res = Pegasus::Utils::Atof(n) - 3.0f;
    res = res < 0.0 ? -res : res;
    return  res < 0.001;
}

bool runAtof2(TestHarness* harness)
{
    const char * n = "643.0283";
    float res = Pegasus::Utils::Atof(n) - 643.0283f;
    res = res < 0.0 ? -res : res;
    return  res < 0.001;
}

bool runAtof3(TestHarness* harness)
{
    const char * n = "-7.0283";
    float res = Pegasus::Utils::Atof(n) - (-7.0283f);
    res = res < 0.0 ? -res : res;
    return  res < 0.001;
}

bool runTesselationTable1(TestHarness* harness)
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

bool runTesselationTable2(TestHarness* harness)
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

bool runTesselationTable3(TestHarness* harness)
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

bool runTesselationTable4(TestHarness* harness)
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

bool runVector1(TestHarness* harness)
{
    Pegasus::Utils::Vector<int> v(&sGlobalAllocator);
    for (int i = 0; i < 400; ++i) v.PushEmpty() = i;

    bool rightOrdering = true;

    int last = -1;

    for (unsigned int i = 0; i < v.GetSize(); ++i)
    {
        if ((v[i] - last) != 1) return false;
        last = v[i];
    }

    return true;
}

bool runVector2(TestHarness* harness)
{
    struct Ss
    {
        char c;
        int i;
    };
    Pegasus::Utils::Vector<Ss> v(&sGlobalAllocator);
    for (int i = 0; i < 300; ++i)
    {
        Ss& st = v.PushEmpty();
        st.i = i;
        st.c = i % 56;
    }

    for (int i = 0; i < 150; ++i)
    {
        v.Delete(i);
    }

    if (v.GetSize() != 150) return false;

    for (unsigned int i = 0; i < v.GetSize(); ++i)
    {
        Ss& st = v[i];
        if (st.i != (2*i + 1) || st.c != (st.i % 56)) return false;
    }
    return true;
}

bool runByteStream1(TestHarness* harness)
{
    Pegasus::Utils::ByteStream bs(&sGlobalAllocator);
    int k = 992;
    bs.Append(&k, sizeof(k));
    bool test1 = *static_cast<int*>(bs.GetBuffer()) == 992 && bs.GetSize() == sizeof(k);
    
    bs.Reset();

    return test1 && bs.GetSize() == 0 && bs.GetBuffer() == nullptr;
}

bool runByteStream2(TestHarness* harness)
{
    Pegasus::Utils::ByteStream bs(&sGlobalAllocator);
    const char* str1 = "hello world";
    const char* str2 = "hello world2";
    const char* str3 = "hello world3";

    int sz1 = Pegasus::Utils::Strlen(str1);
    int sz2 = Pegasus::Utils::Strlen(str2);
    int sz3 = Pegasus::Utils::Strlen(str3);

    bs.Append(str1, sz1);
    bs.Append(str2, sz2);
    bs.Append(str3, sz3 + 1);

    return !Pegasus::Utils::Strcmp(static_cast<char*>(bs.GetBuffer()), "hello worldhello world2hello world3");
}

bool runByteStream3(TestHarness* harness)
{
    Pegasus::Utils::ByteStream bs(&sGlobalAllocator);
    Pegasus::Utils::ByteStream bs2(&sGlobalAllocator);
    const char* str1 = "this is a long string";
    int sz1 = Pegasus::Utils::Strlen(str1);
    bs.Append(str1, sz1);
    bs2.Append(&bs);
    bs2.Append(bs.GetBuffer(), sz1 + 1);
    bs.Reset();
    bs.Append(&bs2);

    return !Pegasus::Utils::Strcmp(static_cast<char*>(bs2.GetBuffer()), static_cast<char*>(bs.GetBuffer()));
}

bool runHashStr(TestHarness* harness)
{
    int hashes[10];
    hashes[0] = Pegasus::Utils::HashStr("Test string 1");
    hashes[1] = Pegasus::Utils::HashStr("Test string 2");
    hashes[2] = Pegasus::Utils::HashStr("Test string 11");
    hashes[3] = Pegasus::Utils::HashStr("Test strina 1");
    hashes[4] = Pegasus::Utils::HashStr("Tect strinG 1");
    hashes[5] = Pegasus::Utils::HashStr("Test ssring 1");
    hashes[6] = Pegasus::Utils::HashStr("Test  tring 1");
    hashes[7] = Pegasus::Utils::HashStr("Teststring 1 ");
    hashes[8] = Pegasus::Utils::HashStr("Test stiing 1");
    hashes[9] = Pegasus::Utils::HashStr("test string 1");

    bool pass = true;

    for (int i = 0; i < 10; ++i)
    {
        for (int j = 0; j < 10; ++j)
        {
            if (i != j)
            {
                pass = pass && hashes[i] != hashes[j];
            }
        }
    }

    return pass;
}

static Pegasus::Tests::TestFunc gUtilsTests[] = {
    //memcpy
    DECLARE_TEST(Memcpy1),
    DECLARE_TEST(Memcpy2),
    DECLARE_TEST(Memcpy3),

    //memset
    DECLARE_TEST(Memset1),
    DECLARE_TEST(Memset2),
    DECLARE_TEST(Memset3),
    DECLARE_TEST(Memset4),

    //strcmp
    DECLARE_TEST(Strcmp1),
    DECLARE_TEST(Strcmp2),
    DECLARE_TEST(Strcmp3),
    DECLARE_TEST(Strcmp4),

    //stricmp
    DECLARE_TEST(Stricmp1),
    DECLARE_TEST(Stricmp2),
    DECLARE_TEST(Stricmp3),

    //strlen
    DECLARE_TEST(Strlen1),
    DECLARE_TEST(Strlen2),
    DECLARE_TEST(Strlen3),

    //strchr
    DECLARE_TEST(Strchr1),
    DECLARE_TEST(Strchr2),
    DECLARE_TEST(Strchr3),

    //strrchr
    DECLARE_TEST(Strrchr1),
    DECLARE_TEST(Strrchr2),
    DECLARE_TEST(Strrchr3),

    //strcat
    DECLARE_TEST(Strcat1),
    DECLARE_TEST(Strcat2),
    DECLARE_TEST(Strcat3),
    DECLARE_TEST(Strcat4),
    DECLARE_TEST(Strcat5),
    DECLARE_TEST(Strcat6),

    //StringHash
    DECLARE_TEST(HashStr),

    //Atoi
    DECLARE_TEST(Atoi1),
    DECLARE_TEST(Atoi2),
    DECLARE_TEST(Atoi3),

    //Atoi
    DECLARE_TEST(Atof1),
    DECLARE_TEST(Atof2),
    DECLARE_TEST(Atof3),

    //TesselationTable
    DECLARE_TEST(TesselationTable1),
    DECLARE_TEST(TesselationTable2),
    DECLARE_TEST(TesselationTable3),
    DECLARE_TEST(TesselationTable4),

    //Vector
    DECLARE_TEST(Vector1),
    DECLARE_TEST(Vector2),

    //ByteStream
    DECLARE_TEST(ByteStream1),
    DECLARE_TEST(ByteStream2),
    DECLARE_TEST(ByteStream3)
};

const Pegasus::Tests::TestFunc* Pegasus::Tests::UtilsHarness::GetTests(int& funcSz) const
{
    funcSz = sizeof(gUtilsTests)/sizeof(gUtilsTests[0]);
    return gUtilsTests;
}

}
}

