#include "Detours_x64.h"
#include <Windows.h>


BYTE *orgBytes;
DWORD len;

int WINAPI MessageBoxHooked(
	_In_opt_ HWND hWnd,
	_In_opt_ LPCSTR lpText,
	_In_opt_ LPCSTR lpCaption,
	_In_ UINT uType
	) {

	unhookDetoursX64("user32", "MessageBoxA", orgBytes, len);

	int retValue = MessageBoxA(0, "Hooked Successfully ...", "By Mahmoud M. Almazari", 0);

	hookDetoursX64("user32", "MessageBoxA", MessageBoxHooked, &orgBytes, &len);

	return retValue;
}


BOOL WINAPI DllMain(HMODULE hModule, DWORD Call_Reason, LPVOID lpReserved) {

	switch (Call_Reason) {
	case DLL_PROCESS_ATTACH:
		hookDetoursX64("user32", "MessageBoxA", MessageBoxHooked, &orgBytes, &len);
	}

	return 1;
}