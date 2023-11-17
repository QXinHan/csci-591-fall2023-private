/*
__author__ = "Zhuoyun Qian"
__copyright__ = "Copyright 2023, Zhuoyun Qian"
__license__ = "Apache"
__version__ = "1.0.0"
__maintainer__ = "Zhuoyun Qian"
__email__ = "qianzhuoyun@nenu.edu.cn"
__status__ = "Prototype"
*/

#include<stdio.h>
#include<stdlib.h>
#include<Windows.h>
#include <ntstatus.h>
#include<winternl.h>

typedef NTSTATUS(WINAPI* fnNtUnmapViewOfSection)(HANDLE ProcessHandle, PVOID BaseAddress);
typedef NTSTATUS(*fnNtQueryInformationProcess)(HANDLE, PROCESSINFOCLASS, PVOID, ULONG, PULONG);

bool Jude32or64(FILE* file)
{
    PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)file;
    PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)((DWORD64)file + pDos->e_lfanew);
    PIMAGE_FILE_HEADER pFileHeader = (PIMAGE_FILE_HEADER)((DWORD64)pNt + 4);
    PIMAGE_OPTIONAL_HEADER pOptionalHeader = PIMAGE_OPTIONAL_HEADER((DWORD64)pFileHeader + sizeof(IMAGE_FILE_HEADER));
    if (pOptionalHeader->Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
    {
        return 1;
    }
    return 0;
}
// Rva to Foa
DWORD64 RVATOFOA32(PVOID file_buffer, DWORD64 Rva)
{
    PIMAGE_DOS_HEADER pdos = (PIMAGE_DOS_HEADER)file_buffer;
    PIMAGE_NT_HEADERS64 pnt64 = nullptr;
    PIMAGE_NT_HEADERS32 pnt32 = nullptr;
    PIMAGE_SECTION_HEADER psec = nullptr;
    DWORD sizeofHeaders = 0;
    DWORD numofSecs = 0;

    if (Jude32or64((FILE*)file_buffer))
    {
        pnt64 = (PIMAGE_NT_HEADERS64)(pdos->e_lfanew + (char*)pdos);
        psec = (PIMAGE_SECTION_HEADER)(pnt64 + 1);
        sizeofHeaders = pnt64->OptionalHeader.SizeOfHeaders;
        numofSecs = pnt64->FileHeader.NumberOfSections;
    }
    else
    {
        pnt32 = (PIMAGE_NT_HEADERS32)(pdos->e_lfanew + (char*)pdos);
        psec = (PIMAGE_SECTION_HEADER)(pnt32 + 1);
        sizeofHeaders = pnt32->OptionalHeader.SizeOfHeaders;
        numofSecs = pnt32->FileHeader.NumberOfSections;
    }

    if (Rva <= sizeofHeaders)
    {
        return (DWORD64)Rva;
    }
    for (WORD i = 0; i < numofSecs; i++)
    {
        if (i + 1 != numofSecs)
        {
            if (Rva >= psec[i].VirtualAddress && Rva < psec[i + 1].VirtualAddress)
            {

                DWORD64 offset = Rva - psec[i].VirtualAddress;
                DWORD64 FOA = offset + psec[i].PointerToRawData;
                return FOA;
            }
        }
        else
        {
            DWORD64 offset = Rva - psec[i].VirtualAddress;
            DWORD64 FOA = offset + psec[i].PointerToRawData;
            return FOA;
        }
    }
    return -1;
}
DWORD repairRelocationTable(PVOID peFile_exe1, DWORD64 ImageOffset) {
    PIMAGE_DOS_HEADER pDos = NULL;
    PIMAGE_NT_HEADERS64 pNt = NULL;
    PIMAGE_BASE_RELOCATION pBaserel = NULL;

    pDos = (PIMAGE_DOS_HEADER)peFile_exe1;
    pNt = (PIMAGE_NT_HEADERS64)(((DWORD64)peFile_exe1 + pDos->e_lfanew));
    pNt->OptionalHeader.ImageBase = pNt->OptionalHeader.ImageBase + ImageOffset;//This code is important , without it the program1.exe will not excute.
    pBaserel = (PIMAGE_BASE_RELOCATION)((DWORD64)peFile_exe1 + RVATOFOA32(peFile_exe1, pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress));
    while (pBaserel->VirtualAddress && pBaserel->SizeOfBlock) {
        DWORD NumberOfItems = (pBaserel->SizeOfBlock - 8) / 2;
        PWORD data = (PWORD)(((DWORD64)pBaserel) + 8);
        for (size_t num = 0; num < NumberOfItems; num++)
        {
            if (((*data) & 0x2000)) {
                DWORD64 changeAddr = (*data) & 0x0fff;
                changeAddr = pBaserel->VirtualAddress + changeAddr;
                changeAddr = (DWORD64)peFile_exe1 + RVATOFOA32(peFile_exe1, changeAddr);
                *((PDWORD64)changeAddr) += ImageOffset;
            }
            data++;
        }
        pBaserel = (PIMAGE_BASE_RELOCATION)(pBaserel->SizeOfBlock + (DWORD64)pBaserel);
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
    PIMAGE_NT_HEADERS64 pNt = (PIMAGE_NT_HEADERS64)(pDos->e_lfanew + image2);
    DWORD64 OEP_exe2 = pNt->OptionalHeader.AddressOfEntryPoint;
    DWORD64 imageBase_exe2 = pNt->OptionalHeader.ImageBase;
    PIMAGE_SECTION_HEADER pSec = (PIMAGE_SECTION_HEADER)(pNt + 1);
    DWORD secCnt = pNt->FileHeader.NumberOfSections;
    DWORD64 textStart;
    DWORD64 textSize;
    while (1) {
        if (!memcmp(pSec->Name, ".shell", 7)) {
            textStart = pSec->VirtualAddress + (DWORD64)image2;
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
    pNt = (PIMAGE_NT_HEADERS64)(pDos->e_lfanew + textStart);
    pSec = (PIMAGE_SECTION_HEADER)(pNt + 1);
    DWORD64 imageSize_exe1 = pNt->OptionalHeader.SizeOfImage;
    DWORD64 imageHeaderSize_exe1 = pNt->OptionalHeader.SizeOfHeaders;
    DWORD64 imageBase_exe1 = pNt->OptionalHeader.ImageBase;
    DWORD64 imageOEP_exe1 = pNt->OptionalHeader.AddressOfEntryPoint;
    DWORD cntSec = pNt->FileHeader.NumberOfSections;

    //Stretch the program1.exe to the memory
    peFile_exe1 = (LPVOID)malloc(textSize);
    memset(peFile_exe1, 0, textSize);
    memcpy(peFile_exe1, (void*)textStart, textSize);
    image1 = (LPVOID)malloc(imageSize_exe1);
    memset(image1, 0, imageSize_exe1);
    memcpy(image1, (void*)peFile_exe1, imageHeaderSize_exe1);
    for (DWORD i = 0; i < cntSec; i++) {
        memcpy((LPVOID)((DWORD64)image1 + pSec[i].VirtualAddress), (LPVOID)((DWORD64)peFile_exe1 + pSec[i].PointerToRawData), pSec[i].SizeOfRawData);
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
        NULL,                     
        NULL,                     
        FALSE,                    
        CREATE_SUSPENDED,         
        NULL,                     
        NULL,                     
        &si,                      
        &pi                       
    );
    if (sucess == FALSE) {
        printf("failed to create new process\n");
        system("pause");
        return 1;
    }

    // Get the context of the suspended process
    CONTEXT context;
    context.ContextFlags = CONTEXT_FULL;
    GetThreadContext(pi.hThread, &context);

    //To get API "NtUnmapViewOfSection" from ntdll.dll
    HMODULE hNtdll = LoadLibrary(L"ntdll.dll");
    if (hNtdll == NULL) {
        printf("Error:fail to connect ntdll.dll\n");
        return 1;
    }
    fnNtUnmapViewOfSection fNtUnmapViewOfSection = (fnNtUnmapViewOfSection)GetProcAddress(hNtdll, "NtUnmapViewOfSection");
    fnNtQueryInformationProcess fNtQueryInformationProcess = (fnNtQueryInformationProcess)GetProcAddress(hNtdll, "NtQueryInformationProcess");
    if (fNtUnmapViewOfSection == NULL || fNtQueryInformationProcess == NULL) {
        printf("Error:we can not get the function named ZwUnmapViewOfSection or fNtQueryInformationProcess\n");
        return 1;
    }

    //get the PEB of the suspended process
    DWORD64 len;
    DWORD64 susProImageBase;
    PROCESS_BASIC_INFORMATION info{ 0 };
    NTSTATUS stat = fNtQueryInformationProcess(pi.hProcess, ProcessBasicInformation, &info, sizeof(info), NULL);
    ReadProcessMemory(pi.hProcess, LPCVOID((DWORD64)info.PebBaseAddress + 0X10), &susProImageBase, 8,NULL);

    // Use API NtUnmapViewOfSection to uninstall the memory of main thread
    NTSTATUS status = fNtUnmapViewOfSection(pi.hProcess, (PVOID)susProImageBase);

    // Use API "VirtualAllocEx" to allocate memory for the stretched program1.exe
    DWORD64 realImage_exe1 = (DWORD64)VirtualAllocEx(pi.hProcess, (LPVOID)imageBase_exe1, imageSize_exe1, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

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
            realImage_exe1 = (DWORD64)VirtualAllocEx(pi.hProcess, NULL, imageSize_exe1, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
        }

        // Repair the relocation table
        repairRelocationTable(peFile_exe1, (SIZE_T)realImage_exe1 - imageBase_exe1);

        // Recopy the repaired program1.exe to the buffer "image1"
        memset(image1, 0, imageSize_exe1);
        memcpy(image1, (void*)peFile_exe1, imageHeaderSize_exe1);
        for (DWORD i = 0; i < cntSec; i++) {
            memcpy((LPVOID)((DWORD64)image1 + pSec[i].VirtualAddress), (LPVOID)((DWORD64)peFile_exe1 + pSec[i].PointerToRawData), pSec[i].SizeOfRawData);
        }

        bool copySuc = WriteProcessMemory(pi.hProcess, (LPVOID)realImage_exe1, (LPCVOID)image1, imageSize_exe1, NULL);
        if (!copySuc) {
            printf("Error:i do not know how to solve it!");
            return 1;
        }
    }

    // Set the context and back to main thread
    context.Rip = imageOEP_exe1 + (DWORD64)realImage_exe1;
    WriteProcessMemory(pi.hProcess, (LPVOID)((DWORD64)info.PebBaseAddress + 0X10), &realImage_exe1, 8, NULL);
    context.ContextFlags = CONTEXT_FULL;

    SetThreadContext(pi.hThread, &context);

    ResumeThread(pi.hThread);

    free(image1);
    free(peFile_exe1);
    FreeLibrary(hNtdll);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;
}