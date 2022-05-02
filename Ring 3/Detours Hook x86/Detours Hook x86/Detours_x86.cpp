#include "Detours_x86.h"

BYTE JMP[5] = { 0xE9, 0x0, 0x0, 0x0, 0x0 }; // __asm { JMP Address_4Bytes }

#define JMP_BUFF_SIZE sizeof(JMP)


BOOL hookDetoursX86(char libName[], char API_Name[], LPVOID newFun, BYTE **orgBytes, DWORD *len) {
	DWORD orgAddress;
	DWORD oldProtect;
	DWORD JMP_Dist;


	orgAddress = (DWORD)GetProcAddress(LoadLibraryA(libName), API_Name);
	if (orgAddress == NULL)
		return 0;

	JMP_Dist = (DWORD)newFun - orgAddress - JMP_BUFF_SIZE;
	
	memcpy(&JMP[1], &JMP_Dist, 4);
	
	VirtualProtect((LPVOID)orgAddress, JMP_BUFF_SIZE, PAGE_EXECUTE_READWRITE, &oldProtect);
	
	*orgBytes = new BYTE[JMP_BUFF_SIZE];
	*len = JMP_BUFF_SIZE;
	memcpy((LPVOID)*orgBytes, (LPVOID)orgAddress, JMP_BUFF_SIZE);

	memcpy((LPVOID)orgAddress, JMP, JMP_BUFF_SIZE);
	
	VirtualProtect((LPVOID)orgAddress, JMP_BUFF_SIZE, oldProtect, &oldProtect);


	return 1;
}


BOOL unhookDetoursX86(char libName[], char API_Name[], BYTE *orgBytes, DWORD len) {
	DWORD orgAddress;
	DWORD oldProtect;

	orgAddress = (DWORD)GetProcAddress(LoadLibraryA(libName), API_Name);
	if (orgAddress == NULL)
		return 0;

	VirtualProtect((LPVOID)orgAddress, len, PAGE_EXECUTE_READWRITE, &oldProtect);

	memcpy((LPVOID)orgAddress, orgBytes, len);

	VirtualProtect((LPVOID)orgAddress, len, oldProtect, &oldProtect);

	return 1;
}