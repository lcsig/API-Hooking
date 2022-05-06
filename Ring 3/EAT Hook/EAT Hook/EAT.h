#include <Windows.h>

DWORD_PTR hookEAT(char libName[], char API_Name[], LPVOID newFun);

BOOL unhookEAT(char libName[], char API_Name[], LPVOID originalFuncAddress);