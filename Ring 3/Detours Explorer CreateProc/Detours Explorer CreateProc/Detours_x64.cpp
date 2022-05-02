#include <Windows.h>

BYTE MOV[10] = { 0x48, 0xB8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };	// __asm { MOV rax, Address_8Bytes }
BYTE JMP_RAX[2] = { 0xFF, 0xE0 };										// __asm { JMP rax }

#define JMP_BUFF_SIZE (sizeof(MOV) + sizeof(JMP_RAX))


BOOL hookDetoursX64(char libName[], char API_Name[], LPVOID newFun, BYTE **orgBytes, DWORD *len) {
	DWORD oldProtect;
	DWORD64 orgAddress;
	
	orgAddress = (DWORD64)GetProcAddress(LoadLibraryA(libName), API_Name);
	if (orgAddress == NULL)
		return 0;

	memcpy(&MOV[2], &newFun, 8);

	VirtualProtect((LPVOID)orgAddress, JMP_BUFF_SIZE, PAGE_EXECUTE_READWRITE, &oldProtect);
	
	*orgBytes = new BYTE[JMP_BUFF_SIZE];
	*len = JMP_BUFF_SIZE; 
	memcpy(*orgBytes, (LPVOID)orgAddress, JMP_BUFF_SIZE);

	memcpy((LPVOID)orgAddress, MOV, sizeof(MOV));
	memcpy((LPVOID)(orgAddress + sizeof(MOV)), JMP_RAX, sizeof(JMP_RAX));
	
	VirtualProtect((LPVOID)orgAddress, JMP_BUFF_SIZE, oldProtect, &oldProtect);

	return 1;
}


BOOL unhookDetoursX64(char libName[], char API_Name[], BYTE *orgBytes, DWORD len) {
	DWORD oldProtect;
	DWORD64 orgAddress;

	orgAddress = (DWORD64)GetProcAddress(LoadLibraryA(libName), API_Name);
	if (orgAddress == NULL)
		return 0;

	VirtualProtect((LPVOID)orgAddress, len, PAGE_EXECUTE_READWRITE, &oldProtect);

	memcpy((LPVOID)orgAddress, orgBytes, len);

	VirtualProtect((LPVOID)orgAddress, len, oldProtect, &oldProtect);

	return 1;
}