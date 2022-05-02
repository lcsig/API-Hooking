#include "Trampoline_X64.h"


BYTE PUSH_RAX[1] = { 0x50 };											// __asm { PUSH rax }
BYTE POP_RAX[1] = { 0x58 };												// __asm { POP rax }
BYTE MOV[10] = { 0x48, 0xB8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };	// __asm { MOV rax, Address_8Bytes }
BYTE JMP_RAX[2] = { 0xFF, 0xE0 };										// __asm { JMP rax }


DWORD64 hookTrampolineX64(char libName[], char API_Name[], LPVOID newFun) {
	DWORD64 orgFun;
	DWORD oldProtect;
	DWORD bytesLen;
	BYTE* overWritten;
	DWORD64 newBuff;

	orgFun = (DWORD64)GetProcAddress(GetModuleHandleA(libName), API_Name);
	if (orgFun == NULL)
		return 0;

	// Prepare JMP instruction 
	memcpy(&MOV[2], &newFun, 8);

	// Store the original instructions 
	bytesLen = instructionsLength(orgFun, sizeof(MOV) + sizeof(JMP_RAX) + sizeof(POP_RAX));
	overWritten = new BYTE[bytesLen];
	memcpy(overWritten, (LPVOID)orgFun, bytesLen);

	// Hook the original function (MOV RAX, JMP RAX, POP RAX) after changing memory protection
	VirtualProtect((LPVOID)orgFun, bytesLen, PAGE_EXECUTE_READWRITE, &oldProtect);
	memcpy((LPVOID)orgFun, MOV, sizeof(MOV));
	memcpy((LPVOID)(orgFun + sizeof(MOV)), JMP_RAX, sizeof(JMP_RAX));
	memcpy((LPVOID)(orgFun + bytesLen - sizeof(POP_RAX)), POP_RAX, sizeof(POP_RAX));
	VirtualProtect((LPVOID)orgFun, bytesLen, oldProtect, &oldProtect);
	
	// Allocate new buffer 
	newBuff = (DWORD64)VirtualAlloc(NULL, bytesLen + sizeof(MOV) + sizeof(JMP_RAX) + sizeof(PUSH_RAX), MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	
	// Prepare JMP instruction 
	orgFun += bytesLen - sizeof(POP_RAX);
	memcpy(&MOV[2], &orgFun, 8);

	// Prepare the new function and write (originalInstructions, PUSH RAX, MOV RAX, JMP RAX)
	memcpy((LPVOID)newBuff, overWritten, bytesLen);
	memcpy((LPVOID)(newBuff + bytesLen), PUSH_RAX, sizeof(PUSH_RAX));
	memcpy((LPVOID)(newBuff + bytesLen + sizeof(PUSH_RAX)), MOV, sizeof(MOV));
	memcpy((LPVOID)(newBuff + bytesLen + sizeof(PUSH_RAX) + sizeof(MOV)), JMP_RAX, sizeof(JMP_RAX));

	return newBuff;
}


/*
 * This functions will return the length of instructions in bytes such that the length is bigger than minimumLength variable
*/
DWORD instructionsLength(DWORD64 address, unsigned int minimumLength) {
	DWORD num = 0;

	while (num < minimumLength) {
		num += LDE(address + num, 0);
	}

	return num;
}
