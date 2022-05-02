#include <Windows.h>
#include <iostream>
#include <string>
using namespace std;

/*
 * IMPORTANT: Switch bit version before compiling and testing ... 
 */

int main() {

	char title[100] = {0};
	strcpy(title, "Hook Challenge ... ");
	strcpy(title + strlen(title), to_string(GetCurrentProcessId()).c_str());

	while (1) {
		MessageBoxA(0, "Can you change this message?!", title, 0);
	}

	return 0;
}