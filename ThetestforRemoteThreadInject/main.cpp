#include<iostream>
#include<stdio.h>
#include<Windows.h>
//#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")
/*
7.The dll address "D:\\Cprogram\\DllForTest\\x64\\Release\\Dll.dll".
8.The test exe address "D:\\Cprogram\\CheckMe\\x64\\Release\\CheckMe.exe"
*/
using namespace std;
int main()
{
	/*Use task manager to find a process ID by myself*/
	TCHAR dllName[] = TEXT("D:\\Cprogram\\DllForTest\\x64\\Release\\Dll.dll");
	DWORD proId;
	cin >> proId;

	/*open the process which we will inject a dll to it*/
	HANDLE hprocess = NULL;
	hprocess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, proId);
	/*
	* 1.the access right to the process opened by this function, this parameter specifies all possible rights fot the process object
	* 2.if the handle of the process is inheritable for the current process creates
	* 3.the process ID that will be opened
	*/
	if (hprocess == NULL)
	{
		cout << "can not open the process" << endl;
		return 1;
	}
	SIZE_T sizeOfDllName = sizeof(dllName) + 1;
	LPVOID pDllAddr = VirtualAllocEx(hprocess, NULL, sizeOfDllName, MEM_COMMIT, PAGE_READWRITE);
	/*
	* allocate a memory in the opened process , this space is used to store the name of injected dll
	* from parameter 1 to five
	* 1.Handle to a process. The function allocates memory within the virtual address space of this process.
	* 2.The starting of the address of the allocated memory you want. if NULL, the function determines where to allocate the memory.
	* 3.The size of the allocated memory.
	* 4.specifies the type of memory allocation,This `MEM_COMMIT` means the allocated memory is initialized to zero.
	* 5.the access rights of the allocated memory, This `PAGE_READWRITE` means the allocated memory can be read and written.
	* The return value is the starting address of the allocated memory.
	*/
	if (pDllAddr == NULL)
	{
		cout << "can not allocate memory" << endl;
		return 1;
	}
	/*write the dll name to the allocated memory*/
	SIZE_T writtenBytes = 0;
	WriteProcessMemory(hprocess, pDllAddr, dllName, sizeOfDllName, &writtenBytes);
	if (writtenBytes == 0)
	{
		cout << "can not write into the memory" << endl;
		return 1;
	}

	LPVOID pFun = GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "LoadLibraryA");
	if (pFun == NULL)
	{
		cout << "can not find the function loadlibraryA\n";
		return 1;
	}

	HANDLE hRemoteThread = CreateRemoteThread(hprocess, NULL, 0, (LPTHREAD_START_ROUTINE)pFun, dllName, 0, NULL);
	/*
	* this function is used to create a thread in the opened process.
	* from parameter 1 to 6.
	* 1. create thread in which process.
	* 2. if this process's child process can inherit the handle of the thread.
	* 3. the size of the thread(stack size). zero means the defaut size.
	* 4. the function that will be executed in the new thread.
	* 5. the parameter to the pFun.
	* 6.Specifies additional flags that control the creation of the thread
	* 7. the ID of the new thread returned (OUT). if NULL, specifies the thread ID does not return.  
	* 
	*/
	if (hRemoteThread == NULL)
	{
		cout << "fail to create remote thread\n";
		return 1;
	}
	WaitForSingleObject(hRemoteThread, 0);
	CloseHandle(hprocess);
	return 0;
}