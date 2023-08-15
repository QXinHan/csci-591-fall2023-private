#include<stdio.h>
#include<stdlib.h>
#include<Windows.h>
#include <ntstatus.h>
typedef NTSTATUS(WINAPI* fnNtUnmapViewOfSection)(HANDLE ProcessHandle, PVOID BaseAddress);
// Rva to Foa
DWORD RVATOFOA(PVOID file_buffer, DWORD Rva) {
	PIMAGE_DOS_HEADER pdos = (PIMAGE_DOS_HEADER)file_buffer;
	PIMAGE_NT_HEADERS pnt = (PIMAGE_NT_HEADERS)(pdos->e_lfanew + (DWORD)file_buffer);
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
DWORD repairRelocationTable(PVOID peFile_exe1,DWORD ImageOffset) {
	PIMAGE_DOS_HEADER pDos = NULL;
	PIMAGE_NT_HEADERS pNt = NULL;
	PIMAGE_BASE_RELOCATION pBaserel = NULL;
	pDos = (PIMAGE_DOS_HEADER)peFile_exe1;
	pNt = (PIMAGE_NT_HEADERS)(((DWORD)peFile_exe1 + pDos->e_lfanew));
	pNt->OptionalHeader.ImageBase = pNt->OptionalHeader.ImageBase + ImageOffset;//This code is important , without it the program1.exe will not excute.
	pBaserel = (PIMAGE_BASE_RELOCATION)((DWORD)peFile_exe1 + RVATOFOA(peFile_exe1, pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress));
	while (pBaserel->VirtualAddress && pBaserel->SizeOfBlock) {
		DWORD NumberOfItems = (pBaserel->SizeOfBlock - 8) / 2;
		PWORD data = (PWORD)(((DWORD)pBaserel) + 8);
		for (size_t num = 0; num < NumberOfItems; num++) 
		{
			if (((*data) & 0xf000) == 0x3000) {
				DWORD changeAddr = (*data) & 0x0fff;
				changeAddr = pBaserel->VirtualAddress + changeAddr; 
				changeAddr = (DWORD)peFile_exe1 + RVATOFOA(peFile_exe1, changeAddr);
				*((PDWORD)changeAddr) += ImageOffset;
			}
			data++;
		}
		pBaserel = (PIMAGE_BASE_RELOCATION)(pBaserel->SizeOfBlock + (DWORD)pBaserel);
	}
	return 0;
}
int main() {
	printf("Hello program2\n");
	PVOID image1;
	char* image2;
	PVOID peFile_exe1;
	// To get the program2.exe file
	image2 = (char*)GetModuleHandle(NULL);
	if (image2 == NULL) {
		printf("image2 == NULL");
		return 1;
	}
	// Parse the program2.exe PE format and decrypt the section ".shell" to get program1.exe 
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)image2;
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + image2);
	DWORD OEP_exe2 = pNt->OptionalHeader.AddressOfEntryPoint;
	DWORD imageBase_exe2 = pNt->OptionalHeader.ImageBase;
	PIMAGE_SECTION_HEADER pSec = (PIMAGE_SECTION_HEADER)(pNt + 1);
	DWORD secCnt = pNt->FileHeader.NumberOfSections;
	DWORD textStart;
	DWORD textSize;
	while (1) {
		if (!memcmp(pSec->Name, ".shell", 7)) {
			textStart = pSec->VirtualAddress + (DWORD)image2;
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
	DWORD imageSize_exe1 = pNt->OptionalHeader.SizeOfImage;
	DWORD imageHeaderSize_exe1 = pNt->OptionalHeader.SizeOfHeaders;
	DWORD imageBase_exe1 = pNt->OptionalHeader.ImageBase;
	DWORD imageOEP_exe1 = pNt->OptionalHeader.AddressOfEntryPoint;
	DWORD cntSec = pNt->FileHeader.NumberOfSections;
	//Stretch the program1.exe to the memory
	peFile_exe1 = (LPVOID)malloc(textSize);
	memset(peFile_exe1, 0, textSize);
	memcpy(peFile_exe1, (void*)textStart, textSize);
	image1 = (LPVOID)malloc(imageSize_exe1);
	memset(image1, 0, imageSize_exe1);
	memcpy(image1, (void*)peFile_exe1, imageHeaderSize_exe1);
	for (DWORD i = 0; i < cntSec; i++) {
		memcpy((LPVOID)((DWORD)image1 + pSec[i].VirtualAddress), (LPVOID)((DWORD)peFile_exe1 + pSec[i].PointerToRawData), pSec[i].SizeOfRawData);
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
		printf("failed to create new process\n");
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
	DWORD susProOEP = context.Eip;
	DWORD susProImageBase = context.Ebx + 8;
	NTSTATUS status = fNtUnmapViewOfSection(pi.hProcess, (PVOID)susProImageBase);
	// Use API "VirtualAllocEx" to allocate memory for the stretched program1.exe
	DWORD realImage_exe1 = (DWORD)VirtualAllocEx(pi.hProcess, (LPVOID)imageBase_exe1, imageSize_exe1, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	// Get the expected memory
	if (realImage_exe1 == imageBase_exe1) {
		bool copySuc = WriteProcessMemory(pi.hProcess, (LPVOID)realImage_exe1, (LPCVOID)image1, imageSize_exe1, NULL);
		if (copySuc == FALSE) {
			printf("Error:it can not be write in there\nI do not know how to solve it\n");
			FreeLibrary(hNtdll);
			return 1;
		}
	}
	// Not get the expected memory
	else {
		if (realImage_exe1 == NULL) 
		{
			realImage_exe1 = (DWORD)VirtualAllocEx(pi.hProcess, NULL, imageSize_exe1, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		}
		// Repair the relocation table
		repairRelocationTable(peFile_exe1, (SIZE_T)realImage_exe1 - imageBase_exe1);
		// Recopy the repaired program1.exe to the buffer "image1"
		memset(image1, 0, imageSize_exe1);
		memcpy(image1, (void*)peFile_exe1, imageHeaderSize_exe1);
		for (DWORD i = 0; i < cntSec; i++) {
			memcpy((LPVOID)((DWORD)image1 + pSec[i].VirtualAddress), (LPVOID)((DWORD)peFile_exe1 + pSec[i].PointerToRawData), pSec[i].SizeOfRawData);
		}
		bool copySuc = WriteProcessMemory(pi.hProcess, (LPVOID)realImage_exe1, (LPCVOID)image1, imageSize_exe1, NULL);
		if (!copySuc) {
			printf("Error:i do not know how to solve it!");
			return 1;
		}
	}
	// Set the context and back to main thread
	context.Eip = imageOEP_exe1 + (DWORD)realImage_exe1;
	WriteProcessMemory(pi.hProcess, (LPVOID)(context.Ebx + 8), &realImage_exe1, 4, NULL);
	context.ContextFlags = CONTEXT_FULL;
	SetThreadContext(pi.hThread, &context);
	ResumeThread(pi.hThread);
	free(image1);
	free(peFile_exe1);
	FreeLibrary(hNtdll);
	system("pause");
	return 0;
}
