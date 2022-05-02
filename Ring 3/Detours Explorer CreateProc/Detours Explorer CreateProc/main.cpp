#include "Detours_x64.h"
#include <Windows.h>
#include <atlbase.h>

BYTE *orgBytes;
DWORD len;

BOOL WINAPI CreateProcessWHooked(
	_In_opt_ LPCWSTR lpApplicationName,
	_Inout_opt_ LPWSTR lpCommandLine,
	_In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
	_In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
	_In_ BOOL bInheritHandles,
	_In_ DWORD dwCreationFlags,
	_In_opt_ LPVOID lpEnvironment,
	_In_opt_ LPCWSTR lpCurrentDirectory,
	_In_ LPSTARTUPINFOW lpStartupInfo,
	_Out_ LPPROCESS_INFORMATION lpProcessInformation
	) {

	int msgResult = MessageBoxA(0, CW2A(lpApplicationName), "This function has been hooked, do you want really to open this program?", MB_YESNO);

	if (msgResult == IDYES) {
		unhookDetoursX64("Kernel32", "CreateProcessW", orgBytes, len);

		BOOL retValue = CreateProcessW(
			  lpApplicationName,
			  lpCommandLine,
			  lpProcessAttributes,
			  lpThreadAttributes,
			  bInheritHandles,
			  dwCreationFlags,
			  lpEnvironment,
			  lpCurrentDirectory,
			  lpStartupInfo,
			  lpProcessInformation
		);

		hookDetoursX64("Kernel32", "CreateProcessW", CreateProcessWHooked, &orgBytes, &len);

		return retValue;
	}

	return TRUE;
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD Call_Reason, LPVOID lpReserved) {

	switch (Call_Reason) {
	case DLL_PROCESS_ATTACH:
		hookDetoursX64("Kernel32", "CreateProcessW", CreateProcessWHooked, &orgBytes, &len);
	}

	return 1;
}