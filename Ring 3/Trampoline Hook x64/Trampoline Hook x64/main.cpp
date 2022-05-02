#include "Trampoline_X64.h"
#include <Windows.h>

typedef int (WINAPI* NewMessageBoxA)(
	_In_opt_ HWND hWnd,
	_In_opt_ LPCSTR lpText,
	_In_opt_ LPCSTR lpCaption,
	_In_ UINT uType);

NewMessageBoxA ReCall;


int WINAPI MessageBoxHooked(
	HWND hWnd,
	LPCSTR lpText,
	LPCSTR lpCaption,
	UINT uType) {

	return ReCall(hWnd, lpText, "Hooked By Almazari ...", uType);
}


BOOL WINAPI DllMain(HMODULE hModule, DWORD Call_Reason, LPVOID lpReserved) {

	switch (Call_Reason) {
	case DLL_PROCESS_ATTACH:
		ReCall = (NewMessageBoxA)hookTrampolineX64("user32", "MessageBoxA", MessageBoxHooked);
	}

	return 1;
}