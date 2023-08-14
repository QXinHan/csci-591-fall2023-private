#include<stdio.h>
#include<stdlib.h>
#include<Windows.h>
#include <ntstatus.h>
typedef NTSTATUS(WINAPI* fnNtUnmapViewOfSection)(HANDLE ProcessHandle, PVOID BaseAddress);
// Rva to Foa
DWORD RVATOFOA(DWORD file_buffer, DWORD Rva) {
	PIMAGE_DOS_HEADER pdos = (PIMAGE_DOS_HEADER)file_buffer;
	PIMAGE_NT_HEADERS pnt = (PIMAGE_NT_HEADERS)(pdos->e_lfanew + file_buffer);
	PIMAGE_SECTION_HEADER psec = (PIMAGE_SECTION_HEADER)(pnt + 1);
	if (Rva <= pnt->OptionalHeader.SizeOfHeaders) 
	{
		return Rva;
	}
	for (WORD i = 0; i < pnt->FileHeader.NumberOfSections; i++)
	{
		if (Rva >= psec[i].VirtualAddress && Rva < psec[i].VirtualAddress + psec[i].Misc.VirtualSize) 
		{
			
			DWORD offset = Rva - psec[i].VirtualAddress;
			DWORD FOA = offset + psec[i].PointerToRawData;
			return FOA;
		}
	}
	printf("failed RVA to FOA\n");
	return -1;
}
int main() {
	printf("Hello program2\n");
	LPVOID image1;
	char* image2;
	LPVOID peFile1;
	// To get the program2.exe file
	image2 = (char*)GetModuleHandle(NULL);
	if (image2 == NULL) {
		printf("image2 == NULL");
		return 1;
	}
	// Parse the program2.exe PE format and decrypt the section ".shell" to get program1.exe 
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)image2;
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + image2);
	SIZE_T OEP_exe2 = pNt->OptionalHeader.AddressOfEntryPoint;
	SIZE_T imageBase_exe2 = pNt->OptionalHeader.ImageBase;
	PIMAGE_SECTION_HEADER pSec = (PIMAGE_SECTION_HEADER)(pNt + 1);
	SIZE_T secCnt = pNt->FileHeader.NumberOfSections;
	SIZE_T textStart;
	SIZE_T textSize;
	while (1) {
		if (!memcmp(pSec->Name, ".shell", 7)) {
			textStart = pSec->VirtualAddress + (SIZE_T)image2;
			textSize = pSec->SizeOfRawData;
			break;
		}
		pSec++;
	}
	for (BYTE* i = (BYTE*)textStart; i < (BYTE*)textStart + textSize; i++) {
		*i ^= 0x40;
	}
	// Parse the program1.exe 
	pDos = (PIMAGE_DOS_HEADER)textStart;
	pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + textStart);
	pSec = (PIMAGE_SECTION_HEADER)(pNt + 1);
	SIZE_T imageSize_exe1 = pNt->OptionalHeader.SizeOfImage;
	SIZE_T imageHeaderSize_exe1 = pNt->OptionalHeader.SizeOfHeaders;
	SIZE_T imageBase_exe1 = pNt->OptionalHeader.ImageBase;
	SIZE_T imageOEP_exe1 = pNt->OptionalHeader.AddressOfEntryPoint;
	SIZE_T cntSec = pNt->FileHeader.NumberOfSections;
	//Stretch the program1.exe to the memory
	peFile1 = (LPVOID)malloc(textSize);
	memset(peFile1, 0, textSize);
	memcpy(peFile1, (void*)textStart, textSize);
	image1 = (LPVOID)malloc(imageSize_exe1);
	memset(image1, 0, imageSize_exe1);
	memcpy(image1, (void*)peFile1, imageHeaderSize_exe1);
	for (SIZE_T i = 0; i < cntSec; i++) {
		memcpy((LPVOID)((SIZE_T)image1 + pSec[i].VirtualAddress), (LPVOID)((SIZE_T)peFile1 + pSec[i].PointerToRawData), pSec[i].SizeOfRawData);
	}
	// Create a suspended process "program2.exe"
	TCHAR appName[] = TEXT("program2.exe");
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	ZeroMemory(&pi, sizeof(pi));
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(STARTUPINFO);
	bool sucess = CreateProcess(
		appName,
		NULL,
		NULL,                     // 进程安全属性
		NULL,                     // 线程安全属性
		FALSE,                    // 是否继承句柄
		CREATE_SUSPENDED,         // 标志位，暂停创建的进程
		NULL,                     // 使用父进程的环境变量
		NULL,                     // 使用父进程的当前目录
		&si,                      // 启动信息
		&pi                       // 进程信息
	);
	if (sucess == FALSE) {
		printf("NONONONO\n");
		system("pause");
		return 1;
	}
	// Get the context of the suspended process
	HMODULE hNtdll = LoadLibrary(L"ntdll.dll");
	CONTEXT context;
	context.ContextFlags = CONTEXT_FULL;
	GetThreadContext(pi.hThread, &context);
	//To get API "NtUnmapViewOfSection" from ntdll.dll
	if (hNtdll == NULL) {
		printf("Error:fail to connect ntdll.dll\n");
		return 1;
	}
	fnNtUnmapViewOfSection fNtUnmapViewOfSection = (fnNtUnmapViewOfSection)GetProcAddress(hNtdll, "NtUnmapViewOfSection");
	if (fNtUnmapViewOfSection == NULL) {
		printf("Error:we can not get the function named ZwUnmapViewOfSection\n");
		return 1;
	}
	// Use API NtUnmapViewOfSection to uninstall the memory of main thread
	SIZE_T susProOEP = context.Eip;
	SIZE_T susProImageBase = context.Ebx + 8;
	NTSTATUS status = fNtUnmapViewOfSection(pi.hProcess, (PVOID)susProImageBase);
	// Use API "VirtualAllocEx" to allocate memory for the stretched program1.exe
	LPVOID realImage_exe1 = VirtualAllocEx(pi.hProcess, (LPVOID)imageBase_exe1, imageSize_exe1, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	// Get the expected memory
	if (realImage_exe1 == (LPVOID)imageBase_exe1) {
		bool copySuc = WriteProcessMemory(pi.hProcess, realImage_exe1, (LPCVOID)image1, imageSize_exe1, NULL);
		if (copySuc == FALSE) {
			printf("Error:it can not be write in there\nI do not know how to solve it\n");
			FreeLibrary(hNtdll);
			return 1;
		}
	}
	// Not get the expected memory
	else {
		printf("Now I will repair this relocation!\n");//for text
		if (realImage_exe1 == NULL) 
		{
			realImage_exe1 = VirtualAllocEx(pi.hProcess, NULL, imageSize_exe1, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		}
		// Repair the relocation table
		pDos = (PIMAGE_DOS_HEADER)peFile1;
		pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + (SIZE_T)peFile1);
		BYTE* relAddr = (BYTE*)(RVATOFOA((SIZE_T)peFile1,pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress) + (SIZE_T)peFile1);
		DWORD relSize = pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size;
		while (relSize) 
		{
			PIMAGE_BASE_RELOCATION pBaserel = (PIMAGE_BASE_RELOCATION)relAddr;
			DWORD sizeOfBlock = pBaserel->SizeOfBlock;
			DWORD numOfblock = (sizeOfBlock - 8) / 2;
			PUSHORT data = (PUSHORT)pBaserel;
			for (SIZE_T i = 4; i < numOfblock + 4; i++) 
			{
				SIZE_T d = RVATOFOA((DWORD)peFile1, pBaserel ->VirtualAddress+(data[i] & 0x0fff))+ (SIZE_T)peFile1;
				SIZE_T Va = *(SIZE_T*)d + (SIZE_T)realImage_exe1 - imageBase_exe1;
				if ((data[i] & 0xf000)==0x3000) {
					*(SIZE_T*)d = Va;
				}
			}
			relSize -= sizeOfBlock;
			relAddr += sizeOfBlock;
		}
		// Recopy the repaired program1.exe to the buffer "image1"
		memset(image1, 0, imageSize_exe1);
		memcpy(image1, (void*)peFile1, imageHeaderSize_exe1);
		for (SIZE_T i = 0; i < cntSec; i++) {
			memcpy((LPVOID)((SIZE_T)image1 + pSec[i].VirtualAddress), (LPVOID)((SIZE_T)peFile1 + pSec[i].PointerToRawData), pSec[i].SizeOfRawData);
		}
		bool copySuc = WriteProcessMemory(pi.hProcess, realImage_exe1, (LPCVOID)image1, imageSize_exe1, NULL);
		if (!copySuc) {
			printf("Error:i do not know how to solve it!");
			return 1;
		}
	}
	// Set the context and back to main thread
	context.Eip = imageOEP_exe1 + (SIZE_T)realImage_exe1;
	WriteProcessMemory(pi.hProcess, (LPVOID)(context.Ebx + 8), &realImage_exe1, 4, NULL);
	context.ContextFlags = CONTEXT_FULL;
	SetThreadContext(pi.hThread, &context);
	ResumeThread(pi.hThread);
	free(image1);
	free(peFile1);
	FreeLibrary(hNtdll);
	system("pause");
	return 0;
}
