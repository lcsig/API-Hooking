#include "IAT.h"


DWORD_PTR hookIAT(char libName[], char API_Name[], LPVOID newFun) {
	DWORD_PTR  imageBase = (DWORD_PTR)GetModuleHandleA(0);
	PIMAGE_DOS_HEADER dosHeaders = (PIMAGE_DOS_HEADER)imageBase;
	PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)(imageBase + dosHeaders->e_lfanew);
	IMAGE_OPTIONAL_HEADER optionalHeader = ntHeaders->OptionalHeader;
	PIMAGE_IMPORT_DESCRIPTOR importDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)(imageBase + optionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);


	DWORD oldProtect;
	PIMAGE_IMPORT_BY_NAME functionName;
	PIMAGE_THUNK_DATA originalFirstThunk = NULL, firstThunk = NULL;
	/*
	Initially FirstThunk is the same as OriginalFirstThunk
	The OriginalFirstThunk is array of names ---> Uses the AddressOfData element of the IMAGE_THUNK_DATA structure to point to IMAGE_IMPORT_BY_NAME structure that contains the Name element, function name.
	The FirstThunk is array of addresses -------> Uses the Function element of the IMAGE_THUNK_DATA structure, which points to the address of the imported function.

	When the executable is loaded, the loader goes through the OriginalFirstThunk array and finds all imported function names the executable is using.
	Then it calculates the addresses of the functions and populates the FirstThunk array so that real functions can be accessed.
	As a result, we need to change the real loaded addresses which are found in FirstThunk not in OriginalFirstThunk
	*/


	LoadLibraryA(libName);
	while (importDescriptor->Name) {
		
		if (strnicmp(libName, (LPCSTR)(imageBase + importDescriptor->Name), strlen(libName)) != 0) {
			importDescriptor++;
			continue;
		}

		originalFirstThunk = (PIMAGE_THUNK_DATA)(imageBase + importDescriptor->OriginalFirstThunk);
		firstThunk = (PIMAGE_THUNK_DATA)(imageBase + importDescriptor->FirstThunk);

		while (originalFirstThunk->u1.AddressOfData) {
			functionName = (PIMAGE_IMPORT_BY_NAME)(imageBase + originalFirstThunk->u1.AddressOfData);

			if (strcmp(functionName->Name, API_Name) == 0) {
				VirtualProtect((LPVOID)(&firstThunk->u1.Function), sizeof(DWORD_PTR), PAGE_READWRITE, &oldProtect);
				firstThunk->u1.Function = (DWORD_PTR)newFun;
				VirtualProtect((LPVOID)(&firstThunk->u1.Function), sizeof(DWORD_PTR), oldProtect, &oldProtect);

				return (DWORD_PTR)GetProcAddress(LoadLibraryA(libName), API_Name);
			}

			originalFirstThunk++;
			firstThunk++;
		}
	}


	return NULL;
}


BOOL unhookIAT(char libName[], char API_Name[]) {
	return (BOOL)hookIAT(libName, API_Name, GetProcAddress(LoadLibraryA(libName), API_Name));
}