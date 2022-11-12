#include <windows.h>
#include <iostream>
//#include "wow64ext.h"

#pragma comment(lib,"wow64ext.lib")

 DWORD64 __cdecl GetModuleHandle64(wchar_t* lpModuleName);


int main() {

	system("pause");

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, GetCurrentProcessId());
	
	char aaa = 0x1;

	char test1;

	if (ReadProcessMemory(hProcess, (LPVOID)&aaa, &test1, sizeof(char), 0)) {

		printf("[+]Read by handle successly!\r\n");
		system("pause");

	}
	else {

		printf("[+]ouch!failed to read,may be hooked\r\n");

		system("pause");
	}


}