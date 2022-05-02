#include <Windows.h>

BOOL hookDetoursX86(char libName[], char API_Name[], LPVOID newFun, BYTE **orgBytes, DWORD *len);

BOOL unhookDetoursX86(char libName[], char API_Name[], BYTE *orgBytes, DWORD len);