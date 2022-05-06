#include "EAT.h"


BYTE MOV[10] = { 0x48, 0xB8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };	// __asm { MOV rax, Address_8Bytes }
BYTE JMP_RAX[2] = { 0xFF, 0xE0 };                                       // __asm { JMP rax }

DWORD_PTR findEmptyLocation(LPVOID memoryAddress) {
    char *memAddr = (char*)memoryAddress;
    const int jmpInstructions = sizeof(MOV) + sizeof(JMP_RAX);
    const int maxInstructionSize = 15;
    const unsigned int maxMemRange = 2 ^ (8 * sizeof(DWORD)) - 2 * maxInstructionSize - jmpInstructions - 1;

    char lastValue = 0x00;
    int sum = 0;
    for (size_t i = 0; i < maxMemRange; i++)
    {
        if (memAddr[i] != 0xCC && memAddr[i] != 0x00) {
            sum = 0;
            lastValue = 0;
            continue;
        }

        if (lastValue != memAddr[i]) {
            lastValue = memAddr[i];
            sum = 0;
            continue;
        }

        sum++;

        if (sum >= 2 * maxInstructionSize + jmpInstructions) {
            DWORD_PTR newAddr = (DWORD_PTR)memoryAddress + i - sum + maxInstructionSize + 1;
            return newAddr;
        }
    }

    return 0;
}


DWORD_PTR hookEAT(char libName[], char API_Name[], LPVOID newFun) {
    DWORD_PTR  imageBase = (DWORD_PTR)(LoadLibraryA(libName));
    PIMAGE_DOS_HEADER dosHeaders = (PIMAGE_DOS_HEADER)imageBase;
    PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)(imageBase + dosHeaders->e_lfanew);
    IMAGE_OPTIONAL_HEADER optionalHeader = ntHeaders->OptionalHeader;
    PIMAGE_EXPORT_DIRECTORY exportDir = (PIMAGE_EXPORT_DIRECTORY)(imageBase + optionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
    // Recall: Export Directory but Import Descriptor (1 Export, and More Than One Import)


    DWORD *funNamesRVAs = (DWORD*)(imageBase + exportDir->AddressOfNames);              // 32bit
    DWORD *funAddressesRVAs = (DWORD*)(imageBase + exportDir->AddressOfFunctions);      // 32bit
    WORD *funNamesOrdinalRVAs = (WORD*)(imageBase + exportDir->AddressOfNameOrdinals);  // 16bit 
    /*
    * AddressOfNames is RVA of RVAs (list of RVA) for functions names
    *      because the function name length is not static, so it can't be (char**)
    */


    char* funcName;
    DWORD_PTR *realFunctionAddress;
    DWORD oldProtection;
    DWORD *pointerToFuncAddressRVA;
    DWORD newRVAFunValue = (DWORD_PTR)newFun - (DWORD_PTR)imageBase;

#if defined(_WIN64)
    /*
    * If (DWORD_PTR)newFun - (DWORD_PTR)imageBase > DWORD (4Bytes),
    *      then the function can't be hooked!
    *      Because funAddressesRVAs is a list of DWORD for both, x64 and x32 systems.
    *
    * As a solution, we will search for empty location in the module to write a jump instruction into the hook function
    */
    DWORD_PTR jmpLocation;
    jmpLocation = findEmptyLocation((LPVOID)imageBase);
    VirtualProtect((LPVOID)jmpLocation, sizeof(MOV) + sizeof(JMP_RAX), PAGE_EXECUTE_READWRITE, &oldProtection);

    memcpy(&MOV[2], &newFun, sizeof(newFun));
    memcpy((LPVOID)jmpLocation, MOV, sizeof(MOV));
    memcpy((LPVOID)(jmpLocation + sizeof(MOV)), JMP_RAX, sizeof(JMP_RAX));

    VirtualProtect((LPVOID)jmpLocation, sizeof(MOV) + sizeof(JMP_RAX), PAGE_EXECUTE_READ, &oldProtection);
    newRVAFunValue = jmpLocation - imageBase;
#endif


    for (size_t i = 0; i < exportDir->NumberOfNames; i++) {
        funcName = (char*)(imageBase + funNamesRVAs[i]);

        if (_stricmp(funcName, API_Name) == 0) {
            realFunctionAddress = (DWORD_PTR*)(imageBase + funAddressesRVAs[funNamesOrdinalRVAs[i]]);
            pointerToFuncAddressRVA = &funAddressesRVAs[funNamesOrdinalRVAs[i]];

            VirtualProtect((LPVOID)pointerToFuncAddressRVA, sizeof(DWORD), PAGE_READWRITE, &oldProtection);
            memcpy(pointerToFuncAddressRVA, &newRVAFunValue, sizeof(DWORD));
            VirtualProtect((LPVOID)pointerToFuncAddressRVA, sizeof(DWORD), oldProtection, &oldProtection);

            return (DWORD_PTR)realFunctionAddress;
        }
    }


    return 0;
}



BOOL unhookEAT(char libName[], char API_Name[], LPVOID originalFuncAddress) {
    return (BOOL)hookEAT(libName, API_Name, originalFuncAddress);
}