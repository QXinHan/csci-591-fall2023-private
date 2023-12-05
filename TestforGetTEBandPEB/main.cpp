#include <windows.h>
#include<iostream>
#include<winternl.h>
using namespace std;

void* GetTebAddress()
{
#ifdef _WIN64
	return (void*)__readgsqword(0x30);
#else
	return (void*)__readfsdword(0x18);
#endif
}

void* GetPebAddress()
{
#ifdef _WIN64
	return (void*)__readgsqword(0x60);
#else
	return (void*)__readfsdword(0x30);
#endif

}
void testfor_1()
{
    //to get the image base
	HANDLE imagebase = GetModuleHandle(NULL);
	if (imagebase != NULL) std::cout <<"imagebase:0x" << hex << imagebase << endl;
#ifdef _WIN64  

    //
    //
            /*x64 Architecture*/
    //
    //
    
    //get the address of teb and peb
    void* teb = GetTebAddress();
    void* peb = GetPebAddress();
    std::cout << "teb address:0x" << hex << (size_t)teb << endl;
    std::cout << "peb address:0x" << hex << (size_t)peb << endl;

    //get peb by teb to test if the return value of `GetTebAddress()` is the address of teb
    size_t pebByteb = *(size_t*)((size_t)teb + 0x60);
    if (pebByteb == (size_t)peb)
    {
        std::cout << "Yes the return value of GetTebAddress is corresct" << endl;
        std::cout << "pebByTeb:0x" << hex << pebByteb << endl;
    }
    else
    {
        std::cout << "No the return value of GetTebAddress is incorrect" << endl;
        std::cout << "pebByTeb:0x" << hex << pebByteb << endl;
    }

    //get imagebase by peb test if the `GetPebAddress()` works correctly
    size_t imagebaseBypeb = *(size_t*)((size_t)peb + 0x10);
    if (imagebaseBypeb == (size_t)imagebase)
    {
        std::cout << "Yes the GetPebAddress works correctly" << endl;
        std::cout << "imagebaseBypeb:0x" << hex << imagebaseBypeb << endl;
    }
    else
    {
        std::cout << "No the GetPebAddress works incorrectly" << endl;
        std::cout << "imagebaseBypeb:0x" << hex << imagebaseBypeb << endl;
    }
#else
    //
    //
            /*x64 Architecture*/
    //
    //

    //get the address of teb and peb
    void* teb = GetTebAddress();
    void* peb = GetPebAddress();
    std::cout << "teb address:0x" << hex << teb << endl;
    std::cout << "peb address:0x" << hex << peb << endl;

    //get peb by teb to test if the return value of `GetTebAddress()` is the address of teb
    DWORD32 pebByteb = *(DWORD32*)((DWORD32)teb + 0x30);
    if (pebByteb == (size_t)peb)
    {
        std::cout << "Yes the GetTebAddress works corresctly" << endl;
        std::cout << "pebByTeb:0x" << hex << pebByteb << endl;
    }
    else
    {
        std::cout << "No the GetTebAddress works incorrectly" << endl;
        std::cout << "pebByTeb:0x" << hex << pebByteb << endl;
    }

    //get imagebase by peb test if the `GetPebAddress()` works correctly
    DWORD32 imagebaseBypeb = *(DWORD32*)((DWORD32)peb + 0x8);
    if (imagebaseBypeb == (DWORD32)imagebase)
    {
        std::cout << "Yes the GetPebAddress works correctly" << endl;
        std::cout << "imagebaseBypeb:0x" << hex << imagebaseBypeb << endl;
    }
    else
    {
        std::cout << "No the GetPebAddress works incorrectly" << endl;
        std::cout << "imagebaseBypeb:0x" << hex << imagebaseBypeb << endl;
    }
#endif
}

void testfor_2()
{
    //to get the image base
    HANDLE imageBase = GetModuleHandle(NULL);
    if (imageBase != NULL) std::cout << "imageBase:0x" << hex << imageBase << endl;
#ifdef _WIN64
    //use API NtCurrentTeb() to get the TEB of current thread
    size_t TebAddr = (size_t)NtCurrentTeb();
    //to get the PEB
    size_t PebAddr = *(size_t*)(TebAddr + 0x60);
    //to get the image base
    size_t imageBase_byAPI = *(size_t*)(PebAddr + 0x10);

    //test the imagebase I get
    if (imageBase_byAPI == (size_t)imageBase)
    {
        std::cout << "Yes the NtCurrentTeb works correctly\n";
        std::cout << "imageBase_byAPI:0x" << hex << imageBase_byAPI << endl;
    }
    else
    {
        std::cout << "No the NtCurrentTeb works incorrectly\n";
        std::cout << "imageBase_byAPI:0x" << hex << imageBase_byAPI << endl;
    }
#else
    //use API NtCurrentTeb() to get the TEB of current thread
    DWORD32 TebAddr = (DWORD32)NtCurrentTeb();
    //to get the PEB
    DWORD32 PebAddr = *(DWORD32*)(TebAddr + 0x30);
    //to get the image base
    DWORD32 imageBase_byAPI = *(DWORD32*)(PebAddr + 0x8);

    //test the imagebase I get
    if (imageBase_byAPI == (DWORD32)imageBase)
    {
        std::cout << "Yes the NtCurrentTeb works correctly\n";
        std::cout << "imageBase_byAPI:0x" << hex << imageBase_byAPI << endl;
    }
    else
    {
        std::cout << "No the NtCurrentTeb works incorrectly\n";
        std::cout << "imageBase_byAPI:0x" << hex << imageBase_byAPI << endl;
    }
#endif 

}

typedef NTSTATUS(WINAPI *fnNtQueryInformationProcess)(HANDLE, PROCESSINFOCLASS, PVOID, ULONG, PULONG);
void testfor_3()
{
#ifdef _WIN64
    size_t susProImageBase;
#else
    DWORD32 susProImageBase;
#endif

    //create a suspended process by program2.exe
    //the program2.exe is just for test don't care what it is
    TCHAR appName[] = TEXT("program2.exe");
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    ZeroMemory(&pi, sizeof(pi));
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(STARTUPINFO);
    bool sucess = CreateProcess(
        appName,
        NULL,
        NULL,
        NULL,
        FALSE,
        CREATE_SUSPENDED,
        NULL,
        NULL,
        &si,
        &pi
    );
    //if false return
    if (sucess == FALSE) {
        printf("failed to create new process\n");
        return;
    }

    //load the ntdll.dll into the current process.
    HMODULE hNtdll = LoadLibrary(L"ntdll.dll");
    if (hNtdll == NULL) {
        printf("Error:fail to connect ntdll.dll\n");
        return;
    }

    //get the target function NtQueryInformationProcess
    fnNtQueryInformationProcess fNtQueryInformationProcess = (fnNtQueryInformationProcess)GetProcAddress(hNtdll, "NtQueryInformationProcess");
    //if fail, return
    if (fNtQueryInformationProcess == NULL) {
        printf("Error:we can not get the function named ZwUnmapViewOfSection or fNtQueryInformationProcess\n");
        return;
    }

    //get the PEB of the suspended process

    PROCESS_BASIC_INFORMATION info{ 0 };
    NTSTATUS stat = fNtQueryInformationProcess(pi.hProcess, ProcessBasicInformation, &info, sizeof(info), NULL);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void testfor_4()
{
#ifdef _WIN64
#define PEBpointer Rdx
#else
#define PEBpointer Ebx
#endif

    //create a suspended process by program2.exe
    //the program2.exe is just for test don't care what it is
    TCHAR appName[] = TEXT("program2.exe");
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    ZeroMemory(&pi, sizeof(pi));
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(STARTUPINFO);
    bool sucess = CreateProcess(
        appName,
        NULL,
        NULL,
        NULL,
        FALSE,
        CREATE_SUSPENDED,
        NULL,
        NULL,
        &si,
        &pi
    );
    //if false return
    if (sucess == FALSE) {
        printf("failed to create new process\n");
        return;
    }


    // Get the context of the suspended process
    CONTEXT context;
    context.ContextFlags = CONTEXT_FULL;
    GetThreadContext(pi.hThread, &context);

    //close the handle
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

}


int main()
{
	testfor_4();
	return 0;
}