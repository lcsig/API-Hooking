#include <Windows.h>

#pragma comment(lib, "LDE64x64")
extern "C" DWORD __stdcall LDE(const DWORD64 lpData, unsigned int ProcType);

DWORD instructionsLength(DWORD64 address, unsigned int minimumLength);

DWORD64 hookTrampolineX64(char libName[], char API_Name[], LPVOID newFun);