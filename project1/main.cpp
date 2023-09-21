#include<stdio.h>
#include<stdlib.h>
#include<Windows.h>
#include"function.h"
using namespace std;
const char test[] = "C:\\Users\\QQmian\\Desktop\\AntivirusPlatinum.exe";
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
    inforPrint((FILE*)disfileptr);
    system("pause");
    fclose(fileptr);
    free(disfileptr);
    free(imagebuffer);
    return 0;
}

void parseDos(FILE* file)/*The e_magic and e_lfanew are the most important in DOS HEADER so I just print two of them*/
{
    PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)file;
    printf("---------------------------------------------\n");
    printf("DOS_HEADER:\n");
    printf("e_magic:%x\n", pDos->e_magic);
    printf("e_lfanew:%x\n", pDos->e_lfanew);
}

void parseNt(FILE* file)
{
    PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)file;
    PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)((char*)file + pDos->e_lfanew);
    printf("---------------------------------------------\n");
    printf("NT_HEADER\n");
    printf("Signature:%x\n", pNt->Signature);
}

void parseFileHeader(FILE* file)
{
    PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)file;
    PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)((char*)file + pDos->e_lfanew);
    PIMAGE_FILE_HEADER pFileHeader = (PIMAGE_FILE_HEADER)((char*)pNt + 4);
    printf("---------------------------------------------\n");
    printf("FILE_HEADER\n");
    printf("Machine:%x\n", pFileHeader->Machine);
    printf("NumberOfSections:%x\n", pFileHeader->NumberOfSections);
    printf("TimeDateStamp:%x\n", pFileHeader->TimeDateStamp);
    printf("PointerToSymbolTable:%x\n", pFileHeader->PointerToSymbolTable);
    printf("numberOfSymbols:%x\n", pFileHeader->NumberOfSymbols);
    printf("SizeOfOptionalHeader:%x\n", pFileHeader->SizeOfOptionalHeader);
    printf("Characteristics:%x\n", pFileHeader->Characteristics);
}

void parseOptionHeader(FILE* file)
{
    PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)file;
    PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)((char*)file + pDos->e_lfanew);
    PIMAGE_FILE_HEADER pFileHeader = (PIMAGE_FILE_HEADER)((char*)pNt + 4);
    PIMAGE_OPTIONAL_HEADER pOptionalHeader = PIMAGE_OPTIONAL_HEADER((char*)pFileHeader + sizeof(IMAGE_FILE_HEADER));
    printf("---------------------------------------------\n");
    printf("OPTIONAL_HEADER\n");
    printf("Magic:%x\n", pOptionalHeader->Magic);/*The value of the magic is used to judge if it is x64 or x86*/
    printf("SizeOfCode:%x\n", pOptionalHeader->SizeOfCode);
    printf("SizeOfInitializedData:%x\n", pOptionalHeader->SizeOfInitializedData);
    printf("SizeOfUninitializedData:%x\n", pOptionalHeader->SizeOfUninitializedData);
    printf("AddressOfEntryPoint:%x\n", pOptionalHeader->AddressOfEntryPoint);
    printf("BaseOfCode:%x\n", pOptionalHeader->BaseOfCode);
    printf("ImageBase:%x\n", pOptionalHeader->ImageBase);
    printf("SectionAlignment:%x\n", pOptionalHeader->SectionAlignment);
    printf("FileAlignment:%x\n", pOptionalHeader->FileAlignment);
    printf("SizeOfImage:%x\n", pOptionalHeader->SizeOfImage);
    printf("SizeOfHeaders:%x\n", pOptionalHeader->SizeOfHeaders);
}

void parseSecHeader64(FILE* file)
{
    PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)file;
    PIMAGE_NT_HEADERS64 pNt = (PIMAGE_NT_HEADERS64)((char*)file + pDos->e_lfanew);
    PIMAGE_FILE_HEADER pFileHeader = (PIMAGE_FILE_HEADER)((char*)pNt + 4);
    PIMAGE_SECTION_HEADER pSec = (PIMAGE_SECTION_HEADER)(pNt + 1);
    size_t numberOfSec = pFileHeader->NumberOfSections;
    printf("---------------------------------------------\n");
    printf("SECTION_HEADER\n");
    for (size_t i = 0; i < numberOfSec; i++)
    {
        PIMAGE_SECTION_HEADER pSecHeader = pSec + i;
        char name[9] = {};
        memcpy(name, pSecHeader->Name, 8);
        printf("Name:%s\n", name);
        printf("VirtualSize:%x\n", pSecHeader->Misc.VirtualSize);
        printf("VirtualAddress:%x\n", pSecHeader->VirtualAddress);
        printf("SizeOfRawData:%x\n", pSecHeader->SizeOfRawData);
        printf("PointerToRawData:%x\n", pSecHeader->PointerToRawData);
        printf("PointerToRelocations:%x\n", pSecHeader->PointerToRelocations);
        printf("PointerToLinenumbers:%x\n", pSecHeader->PointerToLinenumbers);
        printf("NumberOfRelocations:%x\n", pSecHeader->NumberOfRelocations);
        printf("NumberOfLinenumbers:%x\n", pSecHeader->NumberOfLinenumbers);
        printf("Characteristics:%x\n", pSecHeader->Characteristics);
    }
}


void parseSecHeader32(FILE* file)
{
    PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)file;
    PIMAGE_NT_HEADERS32 pNt = (PIMAGE_NT_HEADERS32)((char*)file + pDos->e_lfanew);
    PIMAGE_FILE_HEADER pFileHeader = (PIMAGE_FILE_HEADER)((char*)pNt + 4);
    PIMAGE_SECTION_HEADER pSec = (PIMAGE_SECTION_HEADER)(pNt + 1);
    size_t numberOfSec = pFileHeader->NumberOfSections;
    printf("---------------------------------------------\n");
    printf("SECTION_HEADER\n");
    for (size_t i = 0; i < numberOfSec; i++)
    {
        PIMAGE_SECTION_HEADER pSecHeader = pSec + i;
        char name[9] = {};
        memcpy(name, pSecHeader->Name, 8);
        printf("Name:%s\n", name);
        printf("VirtualSize:%x\n", pSecHeader->Misc.VirtualSize);
        printf("VirtualAddress:%x\n", pSecHeader->VirtualAddress);
        printf("SizeOfRawData:%x\n", pSecHeader->SizeOfRawData);
        printf("PointerToRawData:%x\n", pSecHeader->PointerToRawData);
        printf("PointerToRelocations:%x\n", pSecHeader->PointerToRelocations);
        printf("PointerToLinenumbers:%x\n", pSecHeader->PointerToLinenumbers);
        printf("NumberOfRelocations:%x\n", pSecHeader->NumberOfRelocations);
        printf("NumberOfLinenumbers:%x\n", pSecHeader->NumberOfLinenumbers);
        printf("Characteristics:%x\n", pSecHeader->Characteristics);
    }
}



void inforPrint(FILE* file)
{
    parseDos(file);

    parseNt(file);

    parseFileHeader(file);

    parseOptionHeader(file);
    if (Jude32or64(file))
    {
        parseSecHeader64(file);
    }
    else
    {
        parseSecHeader32(file);
    }

}

bool Jude32or64(FILE* file)
{
    PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)file;
    PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)((char*)file + pDos->e_lfanew);
    PIMAGE_FILE_HEADER pFileHeader = (PIMAGE_FILE_HEADER)((char*)pNt + 4);
    PIMAGE_OPTIONAL_HEADER pOptionalHeader = PIMAGE_OPTIONAL_HEADER((char*)pFileHeader + sizeof(IMAGE_FILE_HEADER));
    if (pOptionalHeader->Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
    {
        return 1;
    }
    return 0;
}

