#include "Trampoline_X86.h"

BYTE JMP[5] = { 0xE9, 0x0, 0x0, 0x0, 0x0 }; // __asm { JMP Address_4Bytes }
#define BuffSize sizeof(JMP)

DWORD hookTrampolineX86(char libName[], char API_Name[], LPVOID newFun) {
	DWORD orgFun;
	DWORD JMP_GAP;
	DWORD oldProtect;
	DWORD instLen;
	BYTE* overWritten;
	DWORD newBuff;

	orgFun = (DWORD)GetProcAddress(GetModuleHandleA(libName), API_Name);
	if (orgFun == NULL)
		return 0;

	JMP_GAP = (DWORD)newFun - orgFun - BuffSize;
	memcpy(&JMP[1], &JMP_GAP, 4);

	VirtualProtect((LPVOID)orgFun, BuffSize, PAGE_EXECUTE_READWRITE, &oldProtect);
	instLen = instructionsLength(orgFun, BuffSize);
	overWritten = new BYTE[instLen];
	memcpy(overWritten, (LPVOID)orgFun, instLen);
	memcpy((LPVOID)orgFun, JMP, BuffSize);
	VirtualProtect((LPVOID)orgFun, BuffSize, oldProtect, &oldProtect);

	newBuff = (DWORD)VirtualAlloc(NULL, instLen + BuffSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	JMP_GAP = (orgFun + instLen) - (newBuff + instLen) - BuffSize;
	memcpy(&JMP[1], &JMP_GAP, 4);
	memcpy((LPVOID)newBuff, overWritten, instLen);
	memcpy((LPVOID)(newBuff + instLen), JMP, BuffSize);

	return newBuff;
}


/*
 * This functions will return the length of instructions in bytes such that the length is bigger than minimumLength variable
 */
DWORD instructionsLength(DWORD address, unsigned int minimumLength) {
	DWORD num = 0;

	while (num < minimumLength) {
		num += LDE(address + num, 0);
	}

	return num;
}
