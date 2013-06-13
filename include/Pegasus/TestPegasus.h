#ifndef PEGASUS_TESTPEGASUS_H
#define PEGASUS_TESTPEGASUS_H

// Test exporting a function
#ifdef __cplusplus
extern "C" {
#endif

#ifdef PEGASUS_DLL
__declspec(dllexport) int __cdecl testFunc(int val);
#else
__declspec(dllimport) int __cdecl testFunc(int val);
#endif

#ifdef __cplusplus
}
#endif

namespace Pegasus {

class TestPegasus
{
public:
    TestPegasus(int value);
    ~TestPegasus();

    int GetValue() const;

private:

    int mValue;
};


}

#endif  // PEGASUS_TESTPEGASUS_H