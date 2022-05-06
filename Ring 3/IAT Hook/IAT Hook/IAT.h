#include <Windows.h>

DWORD_PTR hookIAT(char libName[], char API_Name[], LPVOID newFun);

BOOL unhookIAT(char libName[], char API_Name[]);