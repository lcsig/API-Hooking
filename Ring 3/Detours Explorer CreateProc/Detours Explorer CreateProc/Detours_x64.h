#include <Windows.h>

BOOL hookDetoursX64(char libName[], char API_Name[], LPVOID newFun, BYTE **orgBytes, DWORD *len);

BOOL unhookDetoursX64(char libName[], char API_Name[], BYTE *orgBytes, DWORD len);