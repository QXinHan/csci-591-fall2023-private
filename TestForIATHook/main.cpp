#include<iostream>
#include<stdio.h>
#include<Windows.h>
using namespace std;
/*In this code I will test IAT hook and hook the current process*/
LPVOID oldFunAddr = nullptr;/*This stores the address of the `MessageBoxW` in `user32.dll`*/
LPVOID newFunAddr = nullptr;/*This stores the address of the `MyMessageBox` defined by myself*/
typedef int(WINAPI* PMessageBox)(
	HWND hWnd,          // handle to owner window
	LPCTSTR lpText,     // text in message box
	LPCTSTR lpCaption,  // message box title
	UINT uType          // message box style
	);
/*
* this function is used to change the address written in IAT to the address of the function I write.
* the parameter from 1 to 2 (left to right)
* 1. the address of `MessageBoxW` in user32.dll load by `LoadLibrary`.
* 2. the pointer of the function defined by myself.
*/
bool SetIATHook(LPVOID oldAddr, LPVOID newAddr)
{
	bool flag = FALSE;
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)GetModuleHandle(NULL);
	PIMAGE_NT_HEADERS32 pNt = (PIMAGE_NT_HEADERS32)((DWORD)pDos + pDos->e_lfanew);
	PIMAGE_IMPORT_DESCRIPTOR pIATDes = (PIMAGE_IMPORT_DESCRIPTOR)((DWORD)pDos + pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
	while (pIATDes->FirstThunk != 0 && flag == FALSE)
	{
		PDWORD findFunAddr = (PDWORD)((DWORD)pDos + pIATDes->FirstThunk);
		while (*findFunAddr) 
		{
			if ((DWORD)oldAddr == *findFunAddr)
			{
				DWORD oldProtect = 0;
				VirtualProtect(findFunAddr, sizeof(DWORD), PAGE_READWRITE, &oldProtect);
				*findFunAddr = (DWORD)newAddr;
				VirtualProtect(findFunAddr, sizeof(DWORD), oldProtect, 0);
				flag = TRUE;
			}
			findFunAddr++;
		}
		pIATDes = (PIMAGE_IMPORT_DESCRIPTOR)((DWORD)pIATDes + sizeof(IMAGE_IMPORT_DESCRIPTOR));
	}
	return flag;
}
/*
* This function is used to replace the `MessageBoxW`.
*/
int WINAPI MyMessageBox(
	HWND hWnd,          // handle to owner window
	LPCTSTR lpText,     // text in message box
	LPCTSTR lpCaption,  // message box title
	UINT uType          // message box style
) 
{
	PMessageBox pMessageBox = (PMessageBox)oldFunAddr;
	int x = pMessageBox(hWnd, lpText, lpCaption, uType);
	printf("%d\n", x);
	return x;

}
/*
* this function is used to repair the address changed by me.
* When use the function, one of the address in IAT has been changed to the address of `MyMessgeBox`. So I need to repair it to the previous address.
* the parameter from 1 to 2 (left to right)
* 1. the pointer of the function defined by myself.
* 2. the address of `MessageBoxW` in user32.dll load by `LoadLibrary`.
*/
bool UnitIATHook(LPVOID oldAddr, LPVOID newAddr)
{
	bool flag = FALSE;
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)GetModuleHandle(NULL);
	/*GetModuleHandle(NULL)*/
	/*get the starting address of the exe running in current process, So then I can parse it.*/
	PIMAGE_NT_HEADERS32 pNt = (PIMAGE_NT_HEADERS32)((DWORD)pDos + pDos->e_lfanew);
	PIMAGE_IMPORT_DESCRIPTOR pIATDes = (PIMAGE_IMPORT_DESCRIPTOR)((DWORD)pDos + pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
	/*To find where to change*/
	while (pIATDes->FirstThunk != 0 && flag == FALSE)
	{
		PDWORD findFunAddr = (PDWORD)((DWORD)pDos + pIATDes->FirstThunk);
		while (*findFunAddr)
		{
			if ((DWORD)oldAddr == *findFunAddr)
			{
				DWORD oldProtect = 0;
				VirtualProtect(findFunAddr, sizeof(DWORD), PAGE_READWRITE, &oldProtect);
				*findFunAddr = (DWORD)newAddr;
				VirtualProtect(findFunAddr, sizeof(DWORD), oldProtect, 0);
				flag = TRUE;
			}
			findFunAddr++;
		}
		pIATDes = (PIMAGE_IMPORT_DESCRIPTOR)((DWORD)pIATDes + sizeof(IMAGE_IMPORT_DESCRIPTOR));
	}
	return flag;
}
int main()
{
	oldFunAddr = GetProcAddress(LoadLibrary(TEXT("User32.dll")), "MessageBoxW");
	newFunAddr = MyMessageBox;
	SetIATHook(oldFunAddr, newFunAddr);
	MessageBoxW(NULL, TEXT("test start"), TEXT("test"), MB_OK);
	/*after hooking it, call the `MessageBoxW`,actually call the `MyMessageBox`*/
	UnitIATHook(newFunAddr, oldFunAddr);
	MessageBoxW(NULL, TEXT("test start"), TEXT("test"), MB_OK);
	/*after repairing it, call the `MessageBoxW`,actually call the `MessageBoxW`*/
	return 0;
}