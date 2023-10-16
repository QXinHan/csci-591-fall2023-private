#include"function.h"
using namespace std;
const char test[] = "C:\\Users\\QQmian\\Desktop\\AntivirusPlatinum.exe";

void parsePE::parseDos(FILE* file)/*The e_magic and e_lfanew are the most important in DOS HEADER so I just print two of them*/
{
    /*Just print some members of the DOS structure because they are the same whether in x64 or x86*/
    pDos = (PIMAGE_DOS_HEADER)file;
    printf("---------------------------------------------\n");
    printf("DOS_HEADER:\n");
    printf("e_magic:%x\n", pDos->e_magic);
    printf("e_lfanew:%x\n", pDos->e_lfanew);
}

void parsePE::parseSignature(FILE* file)
{
    pDos = (PIMAGE_DOS_HEADER)file;
    pNt64 = (PIMAGE_NT_HEADERS)((char*)file + pDos->e_lfanew);
    printf("---------------------------------------------\n");
    printf("NT_HEADER\n");
    printf("Signature:%x\n", pNt64->Signature);
}

void parsePE::parseFileHeader(FILE* file)
{
    pDos = (PIMAGE_DOS_HEADER)file;
    //  PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)((char*)file + pDos->e_lfanew);
    /*I decided not to write this, instead ,get the file header by using offset*/
    pFileHeader = (PIMAGE_FILE_HEADER)((char*)file + 4 + pDos->e_lfanew);
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

void parsePE::parseOptionHeader64(FILE* file)
{
    pDos = (PIMAGE_DOS_HEADER)file;
    pFileHeader = (PIMAGE_FILE_HEADER)((char*)file + 4 + pDos->e_lfanew);
    pOptionalHeader64 = PIMAGE_OPTIONAL_HEADER((char*)pFileHeader + sizeof(IMAGE_FILE_HEADER));
    PIMAGE_OPTIONAL_HEADER64 pOptionalHeader = pOptionalHeader64;
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

void parsePE::parseOptionHeader32(FILE* file)
{
    pDos = (PIMAGE_DOS_HEADER)file;
    pFileHeader = (PIMAGE_FILE_HEADER)((char*)file + 4 + pDos->e_lfanew);
    pOptionalHeader32 = PIMAGE_OPTIONAL_HEADER32((char*)pFileHeader + sizeof(IMAGE_FILE_HEADER));
    PIMAGE_OPTIONAL_HEADER32 pOptionalHeader = pOptionalHeader32;
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


void parsePE::parseSecHeader64(FILE* file)
{
    pDos = (PIMAGE_DOS_HEADER)file;
    pNt64 = (PIMAGE_NT_HEADERS64)((char*)file + pDos->e_lfanew);
    pSec = (PIMAGE_SECTION_HEADER)(pNt64 + 1);
    size_t numberOfSec = pNt64->FileHeader.NumberOfSections;
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


void parsePE::parseSecHeader32(FILE* file)
{
    pDos = (PIMAGE_DOS_HEADER)file;
    pNt32 = (PIMAGE_NT_HEADERS32)((char*)file + pDos->e_lfanew);
    pSec = (PIMAGE_SECTION_HEADER)(pNt32 + 1);
    size_t numberOfSec = pNt32->FileHeader.NumberOfSections;
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


void parsePE::inforPrint(FILE* file)
{
    parseDos(file);

    parseSignature(file);

    parseFileHeader(file);


    if (Jude32or64(file))
    {
        parseOptionHeader64(file);
        parseSecHeader64(file);
    }
    else
    {
        parseOptionHeader32(file);
        parseSecHeader32(file);
    }

}

bool parsePE::Jude32or64(FILE* file)
{
    PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)file;
    PIMAGE_FILE_HEADER pFileHeader = (PIMAGE_FILE_HEADER)((char*)file + 4 + pDos->e_lfanew);
    PIMAGE_OPTIONAL_HEADER pOptionalHeader = PIMAGE_OPTIONAL_HEADER((char*)pFileHeader + sizeof(IMAGE_FILE_HEADER));
    /*there are some members in the structure optional header between x64 and x86*/
    if (pOptionalHeader->Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
    {
        return 1;
    }
    return 0;
}

int main(int argc, char* argv[])
{
    /*If the command line is NULL then print "The exe is null"*/
    if (argc == 1)
    {
        printf("The exe is null");
        return 1;
    }

    parsePE parsePe;
    FILE* fileptr = NULL;
    char* disfileptr = NULL;
    char* imagebuffer = NULL;
    DWORD Exfilebuffer;
    size_t sizeOfFile;
    fopen_s(&fileptr, argv[1], "rb");/*open a file on disk*/
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
    parsePe.inforPrint((FILE*)disfileptr);
    system("pause");
    fclose(fileptr);
    free(disfileptr);
    free(imagebuffer);
    return 0;
}
