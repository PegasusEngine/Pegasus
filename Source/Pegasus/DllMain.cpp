#include <windows.h>

#include "Pegasus/Pegasus.h"

// Check for C or C++ interface
#ifdef __cplusplus
extern "C" {
#endif

// Dll entry point, for init/cleanup
BOOL WINAPI DllMain(_In_ HINSTANCE hinstDLL, _In_ DWORD fdwReason, _In_ LPVOID lpvReserved)
{
    return true;
}

// Test exporting a function
int __cdecl testFunc(int val)
{
    return 2 * val;
}

#ifdef __cplusplus
}
#endif
