#include <Windows.h>
#include <iostream>
#include <string>
using namespace std;

typedef int (WINAPI* NewMessageBoxA)(
    _In_opt_ HWND hWnd,
    _In_opt_ LPCSTR lpText,
    _In_opt_ LPCSTR lpCaption,
    _In_ UINT uType);
NewMessageBoxA ReCall;
    
/*
 * IMPORTANT: Switch bit version before compiling and testing ... 
 */

int main() {

	char title[100] = {0};
	strcpy(title, "Hook Challenge ... ");
	strcpy(title + strlen(title), to_string(GetCurrentProcessId()).c_str());

	while (1) {
        ReCall = (NewMessageBoxA)GetProcAddress(LoadLibraryA("User32"), "MessageBoxA");
        ReCall(0, "Can you change this message?!", title, 0);
	}

	return 0;
}