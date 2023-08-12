#include<stdio.h>
#include<stdlib.h>
#include<Windows.h>
#include <ntstatus.h>
typedef HMODULE(WINAPI* fGetModuleHandleW)(LPCWSTR);
typedef NTSTATUS(WINAPI* fnNtUnmapViewOfSection)(HANDLE ProcessHandle, PVOID BaseAddress);
//看别人blog写的，用来提升Debug权限。有没有也没关系，不影响。
BOOL EnableDebugPrivilege()
{
	HANDLE hToken;
	BOOL fOk = FALSE;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
	{
		TOKEN_PRIVILEGES tp;
		tp.PrivilegeCount = 1;
		LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid);

		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);

		fOk = (GetLastError() == ERROR_SUCCESS);
		CloseHandle(hToken);
	}
	return fOk;
}
int main() {
	EnableDebugPrivilege();
	//	HMODULE hKernel32 = LoadLibrary("kernel32.dll");
	printf("Hello program2\n");
	LPVOID image1;
	char* image2;
	LPVOID peFile1;
	//打开program2.exe
	image2 = (char*)GetModuleHandle(NULL);
	if (image2 == NULL) {
		printf("image2 == NULL");
		return 1;
	}
	//找到并且解密最后的.shell section for program1.exe
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
	pDos = (PIMAGE_DOS_HEADER)textStart;
	pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + textStart);
	pSec = (PIMAGE_SECTION_HEADER)(pNt + 1);
	SIZE_T imageSize_exe1 = pNt->OptionalHeader.SizeOfImage;
	SIZE_T imageHeaderSize_exe1 = pNt->OptionalHeader.SizeOfHeaders;
	SIZE_T exe1_imageBase = pNt->OptionalHeader.ImageBase;
	SIZE_T exe1_imageOEP = pNt->OptionalHeader.AddressOfEntryPoint;
	SIZE_T cntSec = pNt->FileHeader.NumberOfSections;
	//拉伸program1.exe到内存中
	peFile1 = (LPVOID)malloc(textSize);
	memset(peFile1, 0, textSize);
	memcpy(peFile1, (void*)textStart, textSize);
	image1 = (LPVOID)malloc(imageSize_exe1);
	memset(image1, 0, imageSize_exe1);
	memcpy(image1, (void*)peFile1, imageHeaderSize_exe1);
	for (SIZE_T i = 0; i < cntSec; i++) {
		memcpy((LPVOID)((SIZE_T)image1 + pSec[i].VirtualAddress), (LPVOID)((SIZE_T)peFile1 + pSec[i].PointerToRawData), pSec[i].SizeOfRawData);
	}
	//用createprocess创建一个program2.exe的进程
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
	// 获取新进程主线程上下文
	HMODULE hNtdll = LoadLibrary(L"ntdll.dll");
	CONTEXT context;
	context.ContextFlags = CONTEXT_FULL;
	GetThreadContext(pi.hThread, &context);
	//从ntdll.dll中获取NtUnmapViewOfSection
	if (hNtdll == NULL) {
		printf("Error:fail to connect ntdll.dll\n");
		return 1;
	}
	fnNtUnmapViewOfSection fNtUnmapViewOfSection = (fnNtUnmapViewOfSection)GetProcAddress(hNtdll, "NtUnmapViewOfSection");
	if (fNtUnmapViewOfSection == NULL) {
		printf("Error:we can not get the function named ZwUnmapViewOfSection\n");
		return 1;
	}
	//用NtUnmapViewOfSection清空这个悬挂进程的内存空间
	//首先要获取悬挂地址的基地址和OEP
	SIZE_T susProOEP = context.Eip;
	SIZE_T susProImageBase = context.Ebx + 8;
	NTSTATUS status = fNtUnmapViewOfSection(pi.hProcess, (PVOID)susProImageBase);
	pDos = (PIMAGE_DOS_HEADER)textStart;
	pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + (char*)textStart);
	SIZE_T imageBase_exe1 = pNt->OptionalHeader.ImageBase;
	SIZE_T sizeOfimage_exe1 = pNt->OptionalHeader.SizeOfImage;
	LPVOID realImage_exe1 = VirtualAllocEx(pi.hProcess, (LPVOID)imageBase_exe1, sizeOfimage_exe1, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	SIZE_T numWriten = 0;
	//分配到了预期空间
	if (realImage_exe1 == (LPVOID)imageBase_exe1) {
		bool copySuc = WriteProcessMemory(pi.hProcess, realImage_exe1, (LPCVOID)image1, imageSize_exe1, &numWriten);
		if (copySuc == FALSE) {
			printf("Error:it can not be write in there\nI do not know how to solve it\n");
			FreeLibrary(hNtdll);
			return 1;
		}
	}
	//未测试重定位，因为还不知道怎么发布所以还没在别的电脑上测试重定位是否正确
	else {
		//随机给exe1在当前进程中分配地址，返回值为exe1被分配空间的基地址
		LPVOID imbase = VirtualAllocEx(pi.hProcess, NULL, sizeOfimage_exe1, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		realImage_exe1 = imbase;
		if (imbase == NULL) {
			printf("Error: imBase is NULL\n");
			return 1;
		}
		bool copySuc = WriteProcessMemory(pi.hProcess, imbase, (LPCVOID)image1, imageSize_exe1, &numWriten);
		
		if (!copySuc) {
			printf("Error:i do not know how to solve it!");
			return 1;
		}
		//已经给exe1分配了新的内存地址，现在该修复重定位表
		pDos = (PIMAGE_DOS_HEADER)imbase;
		pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + (char*)imbase);
		BYTE* relAddr = (BYTE*)pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress + (SIZE_T)imbase;
		SIZE_T relSize = pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size;
		while (relSize) {
			PIMAGE_BASE_RELOCATION pBaserel = (PIMAGE_BASE_RELOCATION)relAddr;
			SIZE_T sizeOfBlock = pBaserel->SizeOfBlock;
			SIZE_T numOfblock = (sizeOfBlock - 8) / 2;
			PUSHORT data = (PUSHORT)pBaserel;
			for (SIZE_T i = 4; i < numOfblock + 4; i++) {
				SIZE_T d = (data[i] & 0xfff) + pBaserel->VirtualAddress + (SIZE_T)imbase;
				//重定位表中存的是需要重定位的数据的位置，和修复方式
				SIZE_T da = *(SIZE_T*)d - (SIZE_T)imbase + exe1_imageBase;//在内存中的绝对地址
				if (data[i] & 0x1000) {
					((PUSHORT)d)[1] = (da & 0xffff0000) >> 16;
				}
				if (data[i] & 0x2000) {
					((PUSHORT)d)[0] = da & 0xffff;
				}
			}
			relSize -= sizeOfBlock;
			relAddr += sizeOfBlock;
		}
	}
	//设置上下文
	context.Eip = exe1_imageOEP + (SIZE_T)realImage_exe1;
	SIZE_T write = 0;
	WriteProcessMemory(pi.hProcess, (LPVOID)(context.Ebx + 8), &realImage_exe1, 4, &write);
	context.ContextFlags = CONTEXT_FULL;
	SetThreadContext(pi.hThread, &context);
	//唤醒挂起的进程回归主线程。
	ResumeThread(pi.hThread);
	free(image1);
	free(peFile1);
	FreeLibrary(hNtdll);
	system("pause");
	return 0;
}
