#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <stdio.h>
#include "Wow64Ext/wow64ext.h"

#include "x64_x86.h"




//win10 21h offset,this offset just for my os version,it may not be compatible with other os version!

UINT64 g_ServiceTableOffset = 0x3A640;

UINT64 g_pServiceTable=0;

UINT64 g_oWow64OpenProcess=0;




UINT32* g_oWow64OpenProcess_ = 0;



void __declspec(naked) HkOpenProcess() {


    //_rcx = 0;


    


    __asm {
        

        __emit 0x51 //push rcx
        __emit 0x56 //push rsi
        __emit 0x57 //push rdi
        __emit 0x53 //push rbx
        __emit 0x52 //push rdx
        __emit 0x54 //push rsp
        __emit 0x55 //push rbp

    }

    HellDoor();


    //修改参数 降权
    __asm {
        xor eax,eax
        mov [ecx+4], eax

    }

    HeavenDoor();
    
	__asm {
        __emit 0x5d //pop rbp
        __emit 0x5c //pop rsp
        __emit 0x5a //pop rdx
        __emit 0x5b
		__emit 0x5f
		__emit 0x5e
		__emit 0x59

    
    }




    __asm {

        __emit 0x48
        mov eax, [g_oWow64OpenProcess_] //mov rax,&target
        __emit 0x0
        __emit 0x0
        __emit 0x0
        __emit 0x0
        
        __emit 0x48       //mov rax,[rax]
        __emit 0x8b
        __emit 0x00

        __emit 0xff //jmp rax
        __emit 0xe0


    }
	
   
}



BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {


		//alloc a new console
		AllocConsole();

		freopen("CONOUT$", "w", stdout);


		wchar_t wszWow64[] = L"wow64.dll";

		DWORD64 hWow64 = GetModuleHandle64(wszWow64);

		printf("[+]hWow64==0x%llx\r\n", hWow64);

        g_pServiceTable = (UINT64)(g_ServiceTableOffset + hWow64);

        //number 23
        printf("[+]g_ServiceTable==0x%llx\r\n", g_pServiceTable);

        DWORD64 pServiceTable_OpenProcess = (DWORD64)(g_pServiceTable + 38 * 8);

        //get orginal Wow64_OpenProcess
        if (!ReadProcessMemory64((HANDLE)-1, pServiceTable_OpenProcess, &g_oWow64OpenProcess, sizeof(DWORD64), 0)) {

            

            printf("unable to read service table\r\n");

            return false;
        
        }

		g_oWow64OpenProcess_ = (UINT32*)malloc(8);
		g_oWow64OpenProcess_[0] = *(UINT32*)((ULONG_PTR)&g_oWow64OpenProcess);
		g_oWow64OpenProcess_[1] = *(UINT32*)((ULONG_PTR)&g_oWow64OpenProcess + 4);

        

        DWORD64 _HkOpenProcess = (DWORD64)HkOpenProcess;

        DWORD dwOldProtection = 0;

        

        printf("[+]g_oWow64OpenProcess==0x%llx\r\n", g_oWow64OpenProcess);

        //asm for jmp
        printf("[+]g_oWow64OpenProcess==0x%llx\r\n", *(UINT64*)(&g_oWow64OpenProcess_[0]));

 

        VirtualProtectEx64((HANDLE)-1, pServiceTable_OpenProcess, 0x1000, PAGE_READWRITE, &dwOldProtection);



        //Hook Table
        bool bOk=WriteProcessMemory64((HANDLE)-1, (DWORD64)pServiceTable_OpenProcess, &_HkOpenProcess, sizeof(DWORD64), 0);

        VirtualProtectEx64((HANDLE)-1, pServiceTable_OpenProcess, 0x1000, dwOldProtection, 0);

        if (!bOk) {

            printf("[+]unable to Hook openprocess\r\n");

            return false;
        }


		return true;
  
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


VOID NTAPI ApcPrintInfo(_In_ ULONG_PTR Parameter) {



    printf("111111111\r\n");

}