#include <Windows.h>

#pragma comment(lib, "LDE64")
extern "C" DWORD __stdcall LDE(const DWORD lpData, unsigned int ProcType);

DWORD instructionsLength(DWORD address, unsigned int minimumLength);

DWORD hookTrampolineX86(char libName[], char API_Name[], LPVOID newFun);