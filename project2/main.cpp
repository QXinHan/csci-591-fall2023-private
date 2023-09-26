/*
 * The next function contains is defined in function.h.
 *  the coding environment is x64 so I use DWORD64.
 *  because there is no bound import table in the two applications you provided,
 *  I have not test the 'parseBoundImportTable' function.
 *
 *
 *
*/

#include<stdio.h>
#include<stdlib.h>
#include <string>
#include<vector>
#include<iostream>
#include<Windows.h>
#include"function.h"
using namespace std;
/*test1 to test3 is used for debug*/
const char test1[] = "C:\\Users\\QQmian\\Desktop\\AntivirusPlatinum.exe";//32-bit
const char test2[] = "C:\\Users\\QQmian\\Desktop\\WindowsProject1.exe";//32-bit
const char test3[] = "C:\\Users\\QQmian\\Desktop\\Stardust.EXE";//64-bit
int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        printf("The exe is null");
        return 1;
    }
    FILE* fileptr = NULL;
    char* disfileptr = NULL;
    char* imagebuffer = NULL;
    DWORD Exfilebuffer;
    size_t sizeOfFile;
    fopen_s(&fileptr, argv[1], "rb");
    fseek(fileptr, 0, SEEK_END);
    sizeOfFile = ftell(fileptr);
    fseek(fileptr, 0, SEEK_SET);
    disfileptr = (char*)malloc(sizeOfFile);
    if (disfileptr == NULL)
    {
        printf("fail to allocate memory for the file\n");
        return 1;
    }
    fread_s(disfileptr, sizeOfFile, sizeOfFile, 1, fileptr);/*load in the memory*/
    printf("Usage: %s\n", argv[1]);
    parseBoundImportTable((FILE*)disfileptr);
    // inforPrint((FILE*)disfileptr);
    system("pause");
    fclose(fileptr);
    free(disfileptr);
    free(imagebuffer);
    return 0;
}


void parseDataDirectory(char* file)
{
    printf("---------------------------------------------\n");
    PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)file;
    PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)((DWORD64)file + pDos->e_lfanew);
    for (int i = 0; i < IMAGE_NUMBEROF_DIRECTORY_ENTRIES; i++)
    {
        printf("vietualAddress:%x\n", pNt->OptionalHeader.DataDirectory[i].VirtualAddress);
        printf("size:%x\n", pNt->OptionalHeader.DataDirectory[i].Size);
        printf("--------\n");
    }
}
//parse the export
void parseExportDirectory(char* file)
{
    if (Jude32or64((FILE*)file)) {
        //parse pe
        PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)file;
        PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)((DWORD64)file + pDos->e_lfanew);
        PIMAGE_FILE_HEADER pFileHeader = (PIMAGE_FILE_HEADER)((DWORD64)pNt + 4);
        PIMAGE_OPTIONAL_HEADER pOptionalHeader = (PIMAGE_OPTIONAL_HEADER)((char*)pFileHeader + sizeof(IMAGE_OPTIONAL_HEADER));

        //find exportdirectory
        if (!pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress && !pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size)
        {
            printf("There is no Export Directory\n");
            return;
        }
        DWORD64 Foa = RVATOFOA(file, pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
        PIMAGE_EXPORT_DIRECTORY pExportDir = (PIMAGE_EXPORT_DIRECTORY)(Foa + (DWORD64)file);
        //print the information
        printf("---------------------------------------------\n");
        printf("ExportDirectory\n");
        printf("Name:%x\n", pExportDir->Name);
        printf("Base:%x\n", pExportDir->Base);
        printf("NumberOfFunctions:%x\n", pExportDir->NumberOfFunctions);
        printf("NumberOfNames:%x\n", pExportDir->NumberOfNames);
        printf("AddressOfFunctions:%x\n", pExportDir->AddressOfFunctions);
        printf("AddressOfNames:%x\n", pExportDir->AddressOfNames);
        printf("AddressOfNameOrdinals:%x\n", pExportDir->AddressOfNameOrdinals);
    }
    else { // 32-bit exe
        //parse pe
        PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)file;
        PIMAGE_NT_HEADERS32 pNt = (PIMAGE_NT_HEADERS32)((DWORD64)file + pDos->e_lfanew);
        PIMAGE_FILE_HEADER pFileHeader = (PIMAGE_FILE_HEADER)((DWORD64)pNt + 4);
        PIMAGE_OPTIONAL_HEADER pOptionalHeader = (PIMAGE_OPTIONAL_HEADER)((DWORD64)pFileHeader + sizeof(IMAGE_OPTIONAL_HEADER));

        //find exportdirectory
        if (!pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress && !pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size)
        {
            printf("There is no Export Directory\n");
            return;
        }
        DWORD64 Foa = RVATOFOA32(file, pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
        PIMAGE_EXPORT_DIRECTORY pExportDir = (PIMAGE_EXPORT_DIRECTORY)(Foa + (DWORD64)file);
        //print the information
        printf("---------------------------------------------\n");
        printf("ExportDirectory\n");
        printf("Name:%x\n", pExportDir->Name);
        printf("Base:%x\n", pExportDir->Base);
        printf("NumberOfFunctions:%x\n", pExportDir->NumberOfFunctions);
        printf("NumberOfNames:%x\n", pExportDir->NumberOfNames);
        printf("AddressOfFunctions:%x\n", pExportDir->AddressOfFunctions);
        printf("AddressOfNames:%x\n", pExportDir->AddressOfNames);
        printf("AddressOfNameOrdinals:%x\n", pExportDir->AddressOfNameOrdinals);
    }

}

void parseRelcDirectory(char* file)
{
    if(Jude32or64((FILE*)file))
    {
        printf("---------------------------------------------\n");
        printf("BaseRelocationDirectory\n");
        PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)file;
        PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)((DWORD64)file + pDos->e_lfanew);
        DWORD64 BaseRelFoa = RVATOFOA(file, pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);
        PIMAGE_BASE_RELOCATION pBaseReloc = (PIMAGE_BASE_RELOCATION)((DWORD64)file + BaseRelFoa);
        for (int i = 0;; i++)
        {
            if (pBaseReloc->VirtualAddress == 0)
            {
                break;
            }
            DWORD numOfEntries = (pBaseReloc->SizeOfBlock - 8) / 2;
            WORD* pEntry = (WORD*)((DWORD64)pBaseReloc + 8);
            printf("Item:%d PageRva:%x  BlockSize:%x  EntriesCount:%x \n", i + 1, pBaseReloc->VirtualAddress, pBaseReloc->SizeOfBlock, numOfEntries);
            for (int j = 0; j < numOfEntries; j++)
            {
                if (!pEntry[j]) break;
                printf("Entry:%d ", j + 1);
                printf("Characteristic:%x ", (pEntry[j]) & 0xf000);
                printf("OffsetFromPage:%x\n", (pEntry[j]) & 0x0fff);
            }
            pBaseReloc = (PIMAGE_BASE_RELOCATION)((DWORD64)pBaseReloc + pBaseReloc->SizeOfBlock);
        }
    }
    else
    {
        printf("---------------------------------------------\n");
        printf("BaseRelocationDirectory\n");
        PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)file;
        PIMAGE_NT_HEADERS32 pNt = (PIMAGE_NT_HEADERS32)((DWORD64)file + pDos->e_lfanew);
        DWORD64 BaseRelFoa = RVATOFOA32(file, pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);
        PIMAGE_BASE_RELOCATION pBaseReloc = (PIMAGE_BASE_RELOCATION)((DWORD64)file + BaseRelFoa);
        for (int i = 0;; i++)
        {
            if (pBaseReloc->VirtualAddress == 0)
            {
                break;
            }
            DWORD numOfEntries = (pBaseReloc->SizeOfBlock - 8) / 2;
            WORD* pEntry = (WORD*)((DWORD64)pBaseReloc + 8);
            printf("Item:%d PageRva:%x  BlockSize:%x  EntriesCount:%x \n", i + 1, pBaseReloc->VirtualAddress, pBaseReloc->SizeOfBlock, numOfEntries);
            for (int j = 0; j < numOfEntries; j++)
            {
                if (!pEntry[j]) break;
                printf("Entry:%d ", j + 1);
                printf("Characteristic:%x ", (pEntry[j]) & 0xf000);
                printf("OffsetFromPage:%x\n", (pEntry[j]) & 0x0fff);

            }
            pBaseReloc = (PIMAGE_BASE_RELOCATION)((DWORD64)pBaseReloc + pBaseReloc->SizeOfBlock);
        }
    }

}

void parseImportTbale(FILE* file)
{
    //parse Pe format
    if (Jude32or64(file)) {
        printf("---------------------------------------------\n");
        printf("ImportTbale\n");
        PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)file;
        PIMAGE_NT_HEADERS64 pNt = (PIMAGE_NT_HEADERS64)((DWORD64)file + pDos->e_lfanew);
        DWORD64 ImportDirFoa = RVATOFOA(file, pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
        PIMAGE_IMPORT_DESCRIPTOR pImportDir = (PIMAGE_IMPORT_DESCRIPTOR)((DWORD64)file + ImportDirFoa);
        if (pImportDir == NULL)
        {
            printf("The import table is NULL\n");
            return;
        }
        //loop for the import table
        for (int i = 0;; i++)
        {
            if (pImportDir->OriginalFirstThunk == 0)
            {
                break;
            }
            //dll name
            char* dllName = (char*)((DWORD64)file + RVATOFOA(file, pImportDir->Name));
            //get the INT
            PIMAGE_THUNK_DATA pThunkData = (PIMAGE_THUNK_DATA)((DWORD64)file + RVATOFOA(file, pImportDir->OriginalFirstThunk));
            //a loop for the INT
            printf("DLL NAME: %s\n", dllName);
            for (int i = 0;; i++)
            {
                if (pThunkData->u1.Function == 0) break;
                if (pThunkData->u1.Ordinal & IMAGE_ORDINAL_FLAG)
                {
                    printf("Ordinal:%d\n", IMAGE_ORDINAL(pThunkData->u1.Ordinal));
                }
                else
                {
                    PIMAGE_IMPORT_BY_NAME ImportByNameAddr = (PIMAGE_IMPORT_BY_NAME)(RVATOFOA(file, pThunkData->u1.AddressOfData) + (DWORD64)file);
                    printf("ImportFunName:%s\n", ImportByNameAddr->Name);
                }
                pThunkData += 1;
            }
            pImportDir++;
        }
    }
    else
    {
        printf("---------------------------------------------\n");
        printf("ImportTbale\n");
        PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)file;
        PIMAGE_NT_HEADERS32 pNt = (PIMAGE_NT_HEADERS32)((DWORD64)file + pDos->e_lfanew);
        DWORD64 ImportDirFoa = RVATOFOA32(file, pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
        PIMAGE_IMPORT_DESCRIPTOR pImportDir = (PIMAGE_IMPORT_DESCRIPTOR)((DWORD64)file + ImportDirFoa);
        if (pImportDir == NULL)
        {
            printf("The import table is NULL\n");
            return;
        }
        //loop for the import table
        for (int i = 0;; i++)
        {
            if (pImportDir->OriginalFirstThunk == 0)
            {
                break;
            }
            //dll name
            char* dllName = (char*)((DWORD64)file + RVATOFOA32(file, pImportDir->Name));
            //get the INT
            PIMAGE_THUNK_DATA32 pThunkData = (PIMAGE_THUNK_DATA32)((DWORD64)file + RVATOFOA32(file, pImportDir->OriginalFirstThunk));
            //a loop for the INT
            printf("DLL NAME: %s\n", dllName);
            for (int i = 0;; i++)
            {
                if (pThunkData->u1.Function == 0) break;
                if (pThunkData->u1.Ordinal & IMAGE_ORDINAL_FLAG)
                {
                    printf("Ordinal:%d\n", IMAGE_ORDINAL(pThunkData->u1.Ordinal));
                }
                else
                {
                    PIMAGE_IMPORT_BY_NAME ImportByNameAddr = (PIMAGE_IMPORT_BY_NAME)(RVATOFOA32(file, pThunkData->u1.AddressOfData) + (DWORD64)file);
                    printf("ImportFunName:%s\n", ImportByNameAddr->Name);
                }
                pThunkData += 1;
            }
            pImportDir++;
        }
    }
}

void parseImportAddrTable(FILE* file)
{
    if (Jude32or64(file))
    {
        printf("---------------------------------------------\n");
        printf("IAT\n");

        PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)file;
        PIMAGE_NT_HEADERS64 pNt = (PIMAGE_NT_HEADERS64)((DWORD64)file + pDos->e_lfanew);
        if (!pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT].VirtualAddress && !pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT].Size)
        {
            printf("There is no IAT\n");
            return;
        }
        DWORD64 Foa = RVATOFOA(file, pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT].VirtualAddress);
        PIMAGE_THUNK_DATA64 pIatTable = (PIMAGE_THUNK_DATA64)((DWORD64)file + Foa);
        DWORD numOfTables = pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT].Size / sizeof(IMAGE_THUNK_DATA64);

        for (DWORD i = 0; i < numOfTables; i++)
        {
            if (!pIatTable->u1.AddressOfData) continue;
            if (pIatTable->u1.AddressOfData & IMAGE_ORDINAL_FLAG64)
            {
                printf("Ordinal:%x\n", (pIatTable->u1.Ordinal) & 0x7fffffffffffffff);
            }
            else
            {
                Foa = RVATOFOA(file, pIatTable->u1.AddressOfData);
                PIMAGE_IMPORT_BY_NAME pImpourByName = (PIMAGE_IMPORT_BY_NAME)((DWORD64)file + Foa);
                printf("%s\n", pImpourByName->Name);
            }
            pIatTable++;
        }

    }
    else
    {
        printf("---------------------------------------------\n");
        printf("IAT\n");

        PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)file;
        PIMAGE_NT_HEADERS32 pNt = (PIMAGE_NT_HEADERS32)((DWORD64)file + pDos->e_lfanew);
        if (!pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT].VirtualAddress && !pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT].Size)
        {
            printf("There is no IAT\n");
            return;
        }
        DWORD64 Foa = RVATOFOA32(file, pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT].VirtualAddress);
        PIMAGE_THUNK_DATA32 pIatTable = (PIMAGE_THUNK_DATA32)((DWORD64)file + Foa);
        DWORD numOfTables = pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT].Size / sizeof(IMAGE_THUNK_DATA32);
        for (DWORD i = 0; i < numOfTables; i++)
        {
            if (!pIatTable->u1.AddressOfData) continue;
            if (pIatTable->u1.AddressOfData & IMAGE_ORDINAL_FLAG32)
            {
                printf("Ordinal:%x\n", IMAGE_ORDINAL32((pIatTable + i)->u1.Ordinal));
            }
            else
            {
                Foa = RVATOFOA32(file, pIatTable->u1.AddressOfData);
                PIMAGE_IMPORT_BY_NAME pImpourByName = (PIMAGE_IMPORT_BY_NAME)((DWORD64)file + Foa);
                printf("%s\n", pImpourByName->Name);
            }
            pIatTable++;
        }
    }
}

void parseBoundImportTable(FILE* file)
{
    if (Jude32or64(file)) {
        PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)file;
        PIMAGE_NT_HEADERS64 pNt = (PIMAGE_NT_HEADERS64)((DWORD64)file + pDos->e_lfanew);
        if (!pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT].Size && !pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT].VirtualAddress)
        {
            printf("There is no bound import table\n");
            return;
        }
        DWORD64 Foa = RVATOFOA(file, pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT].VirtualAddress);
        PIMAGE_BOUND_IMPORT_DESCRIPTOR pboundImportDes = (PIMAGE_BOUND_IMPORT_DESCRIPTOR)((DWORD64)file + Foa);
        while (true)
        {
            if (pboundImportDes == 0) break;
            char* name = (char*)((DWORD64)file + RVATOFOA(file,pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT].VirtualAddress + pboundImportDes->OffsetModuleName));
            printf("BoundDLLName:%s\n", name);
            DWORD numOfDll = pboundImportDes->NumberOfModuleForwarderRefs;
            PIMAGE_BOUND_FORWARDER_REF pBoundRef = (PIMAGE_BOUND_FORWARDER_REF)((DWORD64)pboundImportDes + sizeof(PIMAGE_BOUND_IMPORT_DESCRIPTOR));
            for (DWORD i = 0; i < numOfDll; i++)
            {
                //The dll is used by the bounded dll.
                name = (char*)((DWORD64)file + RVATOFOA(file, pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT].VirtualAddress + pBoundRef->OffsetModuleName));
                printf("boundDllName:%s\n", name);
            }
            pboundImportDes++;
        }
    }
    else {
        PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)file;
        PIMAGE_NT_HEADERS32 pNt = (PIMAGE_NT_HEADERS32)((DWORD64)file + pDos->e_lfanew);
        if (!pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT].Size && !pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT].VirtualAddress)
        {
            printf("There is no bound import table\n");
            return;
        }
        DWORD64 Foa = RVATOFOA32(file, pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT].VirtualAddress);
        PIMAGE_BOUND_IMPORT_DESCRIPTOR pboundImportDes = (PIMAGE_BOUND_IMPORT_DESCRIPTOR)((DWORD64)file + Foa);
        while (true)
        {
            if (pboundImportDes == 0) break;
            char* name = (char*)((DWORD64)file + RVATOFOA32(file, pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT].VirtualAddress + pboundImportDes->OffsetModuleName));
            printf("BoundDLLName:%s\n", name);
            DWORD numOfDll = pboundImportDes->NumberOfModuleForwarderRefs;
            PIMAGE_BOUND_FORWARDER_REF pBoundRef = (PIMAGE_BOUND_FORWARDER_REF)((DWORD64)pboundImportDes + sizeof(PIMAGE_BOUND_IMPORT_DESCRIPTOR));
            for (DWORD i = 0; i < numOfDll; i++)
            {
                //The dll is used by the bounded dll.
                name = (char*)((DWORD64)file + RVATOFOA32(file, pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT].VirtualAddress + pBoundRef->OffsetModuleName));
                printf("boundDllName:%s\n", name);
            }
            pboundImportDes++;
        }
    }
}

void inforPrint(FILE* file)
{

    //parseDataDirectory((char*)file);

    //parseDataDirectory((char*)file);

    //parseExportDirectory((char*)file);

    parseImportTbale(file);
}

DWORD Align(DWORD src, DWORD des)
{
    return des * (src / des) + ((src % des == 0) ? 0 : des);
}


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

void parseRcTable(FILE* file)
{
    /*Get the resource directory*/
    PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)file;
    PIMAGE_NT_HEADERS pNt = NULL;
    PIMAGE_RESOURCE_DIRECTORY pRescDir1 = NULL;
    WORD numOfNameEntries1 = 0;
    WORD numOfIdEntries1 = 0;
    if (Jude32or64(file)) {//succeed to parse 64-bit
        pNt = (PIMAGE_NT_HEADERS)((DWORD64)file + pDos->e_lfanew);
        DWORD64 a = RVATOFOA(file, pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress);
        pRescDir1 = (PIMAGE_RESOURCE_DIRECTORY)((DWORD64)file + RVATOFOA(file, pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress));
        numOfNameEntries1 = pRescDir1->NumberOfNamedEntries;
        numOfIdEntries1 = pRescDir1->NumberOfIdEntries;
        for (WORD i = 0; i < numOfIdEntries1 + numOfNameEntries1; i++)
        {
            PIMAGE_RESOURCE_DIRECTORY_ENTRY pRescDirEntry1 = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD64)pRescDir1 + sizeof(IMAGE_RESOURCE_DIRECTORY) + i * 8);
            if (pRescDirEntry1->NameIsString)
            {
                PIMAGE_RESOURCE_DIR_STRING_U pNameStr = (PIMAGE_RESOURCE_DIR_STRING_U)(pRescDirEntry1->NameOffset);
                wprintf(L"%s\n", pNameStr->NameString);
            }
            else
            {
                printf("Res1 ID: %x\n", pRescDirEntry1->Id);
            }
            if (pRescDirEntry1->DataIsDirectory)
            {
                PIMAGE_RESOURCE_DIRECTORY pRescDir2 = (PIMAGE_RESOURCE_DIRECTORY)((DWORD64)pRescDir1 + (pRescDirEntry1->OffsetToData & 0x7fffffff));

                WORD numOfNameEntries2 = pRescDir2->NumberOfNamedEntries;
                WORD numOfIdEntries2 = pRescDir2->NumberOfIdEntries;

                for (WORD j = 0; j < numOfIdEntries2 + numOfNameEntries2; j++)
                {
                    PIMAGE_RESOURCE_DIRECTORY_ENTRY pRescDirEntry2 = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD64)pRescDir2 + sizeof(IMAGE_RESOURCE_DIRECTORY) + j * 8);
                    if (pRescDirEntry2->NameIsString)
                    {
                        PIMAGE_RESOURCE_DIR_STRING_U pNameStr = (PIMAGE_RESOURCE_DIR_STRING_U)(pRescDirEntry2->NameOffset);
                        wprintf(L"%s\n", pNameStr->NameString);
                    }
                    else
                    {
                        printf("Res2 ID: %x\n", pRescDirEntry2->Id);
                    }
                    if (pRescDirEntry2->DataIsDirectory)
                    {
                        PIMAGE_RESOURCE_DIRECTORY pRescDir3 = (PIMAGE_RESOURCE_DIRECTORY)((DWORD64)pRescDir1 + (pRescDirEntry2->OffsetToData & 0x7fffffff));

                        WORD numOfNameEntries3 = pRescDir3->NumberOfNamedEntries;
                        WORD numOfIdEntries3 = pRescDir3->NumberOfIdEntries;

                        for (WORD k = 0; k < numOfIdEntries3 + numOfNameEntries3; k++)
                        {
                            PIMAGE_RESOURCE_DIRECTORY_ENTRY pRescDirEntry3 = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD64)pRescDir3 + sizeof(IMAGE_RESOURCE_DIRECTORY) + k * 8);
                            if (pRescDirEntry2->NameIsString)
                            {
                                PIMAGE_RESOURCE_DIR_STRING_U pNameStr = (PIMAGE_RESOURCE_DIR_STRING_U)(pRescDirEntry3->NameOffset);
                                wprintf(L"%s\n", pNameStr->NameString);
                            }
                            else
                            {
                                printf("Res3 ID: %x\n", pRescDirEntry3->Id);
                            }
                            PIMAGE_DATA_DIRECTORY pDataDir = (PIMAGE_DATA_DIRECTORY)((DWORD64)pRescDir1 + (pRescDirEntry3->OffsetToData & 0x7fffffff));
                            printf("RVA:%x\n", pDataDir->VirtualAddress);
                            printf("SIZE:%x\n", pDataDir->Size);
                        }
                    }
                }
            }
        }
    }
    else
    {
        PIMAGE_NT_HEADERS32 pNt32 = (PIMAGE_NT_HEADERS32)((DWORD64)file + pDos->e_lfanew);
        DWORD64 a = RVATOFOA32(file, pNt32->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress);
        pRescDir1 = (PIMAGE_RESOURCE_DIRECTORY)((DWORD64)file + RVATOFOA32(file, pNt32->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress));

        numOfNameEntries1 = pRescDir1->NumberOfNamedEntries;
        numOfIdEntries1 = pRescDir1->NumberOfIdEntries;
        for (WORD i = 0; i < numOfIdEntries1 + numOfNameEntries1; i++)
        {
            PIMAGE_RESOURCE_DIRECTORY_ENTRY pRescDirEntry1 = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD64)pRescDir1 + sizeof(IMAGE_RESOURCE_DIRECTORY) + i * 8);
            if (pRescDirEntry1->NameIsString)
            {
                PIMAGE_RESOURCE_DIR_STRING_U pNameStr = (PIMAGE_RESOURCE_DIR_STRING_U)(pRescDirEntry1->NameOffset);
                wprintf(L"%s\n", pNameStr->NameString);
            }
            else
            {
                printf("Res1 ID: %x\n", pRescDirEntry1->Id);
            }
            if (pRescDirEntry1->DataIsDirectory)
            {
                PIMAGE_RESOURCE_DIRECTORY pRescDir2 = (PIMAGE_RESOURCE_DIRECTORY)((DWORD64)pRescDir1 + (pRescDirEntry1->OffsetToData & 0x7fffffff));

                WORD numOfNameEntries2 = pRescDir2->NumberOfNamedEntries;
                WORD numOfIdEntries2 = pRescDir2->NumberOfIdEntries;

                for (WORD j = 0; j < numOfIdEntries2 + numOfNameEntries2; j++)
                {
                    PIMAGE_RESOURCE_DIRECTORY_ENTRY pRescDirEntry2 = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD64)pRescDir2 + sizeof(IMAGE_RESOURCE_DIRECTORY) + j * 8);
                    if (pRescDirEntry2->NameIsString)
                    {
                        PIMAGE_RESOURCE_DIR_STRING_U pNameStr = (PIMAGE_RESOURCE_DIR_STRING_U)(pRescDirEntry2->NameOffset);
                        wprintf(L"%s\n", pNameStr->NameString);
                    }
                    else
                    {
                        printf("Res2 ID: %x\n", pRescDirEntry2->Id);
                    }
                    if (pRescDirEntry2->DataIsDirectory)
                    {
                        PIMAGE_RESOURCE_DIRECTORY pRescDir3 = (PIMAGE_RESOURCE_DIRECTORY)((DWORD64)pRescDir1 + (pRescDirEntry2->OffsetToData & 0x7fffffff));

                        WORD numOfNameEntries3 = pRescDir3->NumberOfNamedEntries;
                        WORD numOfIdEntries3 = pRescDir3->NumberOfIdEntries;

                        for (WORD k = 0; k < numOfIdEntries3 + numOfNameEntries3; k++)
                        {
                            PIMAGE_RESOURCE_DIRECTORY_ENTRY pRescDirEntry3 = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD64)pRescDir3 + sizeof(IMAGE_RESOURCE_DIRECTORY) + k * 8);
                            if (pRescDirEntry2->NameIsString)
                            {
                                PIMAGE_RESOURCE_DIR_STRING_U pNameStr = (PIMAGE_RESOURCE_DIR_STRING_U)(pRescDirEntry3->NameOffset);
                                wprintf(L"%s\n", pNameStr->NameString);
                            }
                            else
                            {
                                printf("Res3 ID: %x\n", pRescDirEntry3->Id);
                            }
                            PIMAGE_DATA_DIRECTORY pDataDir = (PIMAGE_DATA_DIRECTORY)((DWORD64)pRescDir1 + (pRescDirEntry3->OffsetToData & 0x7fffffff));
                            printf("RVA:%x\n", pDataDir->VirtualAddress);
                            printf("SIZE:%x\n", pDataDir->Size);
                        }
                    }
                }
            }
        }
    }
}
DWORD64 RVATOFOA(PVOID file_buffer, DWORD64 Rva)
{
    PIMAGE_DOS_HEADER pdos = (PIMAGE_DOS_HEADER)file_buffer;
    PIMAGE_NT_HEADERS pnt = (PIMAGE_NT_HEADERS)(pdos->e_lfanew + (char*)pdos);
    PIMAGE_SECTION_HEADER psec = (PIMAGE_SECTION_HEADER)(pnt + 1);
    if (Rva <= pnt->OptionalHeader.SizeOfHeaders)
    {
        return (DWORD64)Rva;
    }
    for (WORD i = 0; i < pnt->FileHeader.NumberOfSections; i++)
    {
        if (i + 1 != pnt->FileHeader.NumberOfSections)
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
DWORD64 RVATOFOA32(PVOID file_buffer, DWORD Rva)
{
    PIMAGE_DOS_HEADER pdos = (PIMAGE_DOS_HEADER)file_buffer;
    PIMAGE_NT_HEADERS32 pnt = (PIMAGE_NT_HEADERS32)(pdos->e_lfanew + (char*)pdos);
    PIMAGE_SECTION_HEADER psec = (PIMAGE_SECTION_HEADER)(pnt + 1);
    if (Rva <= pnt->OptionalHeader.SizeOfHeaders)
    {
        return (DWORD64)Rva;
    }
    for (WORD i = 0; i < pnt->FileHeader.NumberOfSections; i++)
    {
        if (i + 1 != pnt->FileHeader.NumberOfSections)
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