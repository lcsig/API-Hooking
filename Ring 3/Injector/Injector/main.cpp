#include <Windows.h>
#include <tlhelp32.h>
#include <atlbase.h>
#include <iostream>
#include <string>
using namespace std;


DWORD getProcessIdByName(LPCSTR procName) {
	DWORD procID;
	HANDLE snapshot;
	PROCESSENTRY32 entry;

	entry.dwSize = sizeof(PROCESSENTRY32);
	snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	while (Process32Next(snapshot, &entry) == TRUE) {
		if (stricmp(CW2A(entry.szExeFile), procName) == 0)
		{
			procID = entry.th32ProcessID;
			CloseHandle(snapshot);
			break;
		}
	}

	return procID;
}


void main() {
	char libPath[MAX_PATH];
	string processName;
	string libName;
	HANDLE hProc;
	LPVOID hookLib;
	HANDLE threadHandle;

	while (1) {
		cout << "Enter Process Name : ";
		getline(cin, processName);

		cout << "Enter Dll File Name : ";
		getline(cin, libName);

		GetFullPathNameA(libName.c_str(), MAX_PATH, libPath, nullptr);
		hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, getProcessIdByName(processName.c_str()));
		hookLib = VirtualAllocEx(hProc, NULL, strlen(libPath) + 1, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
		WriteProcessMemory(hProc, hookLib, libPath, strlen(libPath), NULL);
		threadHandle = CreateRemoteThread(
			hProc,
			NULL,
			NULL,
			(LPTHREAD_START_ROUTINE)GetProcAddress(LoadLibraryA("Kernel32"), "LoadLibraryA"),
			hookLib,
			NULL,
			NULL
			);
		CloseHandle(hProc);

		if (threadHandle == 0 || hProc == 0)
			cout << "Injection Failed.\n";
		else
			cout << "Injected Successfully.\n";

		system("pause");
	}
}