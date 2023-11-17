/*
__author__ = "Zhuoyun Qian"
__copyright__ = "Copyright 2023, Zhuoyun Qian"
__license__ = "Apache"
__version__ = "1.0.0"
__maintainer__ = "Zhuoyun Qian"
__email__ = "qianzhuoyun@nenu.edu.cn"
__status__ = "Prototype"
*/

#include <iostream>
#include<stdio.h>
#include<stdlib.h>
#include<Windows.h>

// Jugde the file whether it is a PE file or not.
// Because it is x86,the code of it doesn't contain the function of judging if the file is 64-bits or 32-bits.
bool judgePeFile(char* image) {
    if (image == NULL) {
        printf("Error:parameter is NULL!");
        return FALSE;
    }
    PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)image;
    PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)(image + pDos->e_lfanew);
    if (pNtHeader->Signature == IMAGE_NT_SIGNATURE) {
        return TRUE;
    }
    else {
        printf("Error: This is not a PE file!");
        return FALSE;
    }
}

DWORD Align(DWORD src, DWORD des)
//src sizeofrawdata ,   des sectionalign
{
    return des * (src / des) + ((src % des == 0) ? 0 : des);
}

int main()
{
    // Parse the program1.exe and encrypt it.
    FILE* peFile_1;
    FILE* peFile_2;
    fopen_s(&peFile_1, ".\\program1.exe", "rb");
    fseek(peFile_1, 0, SEEK_END);
    size_t sizeOfFile_1 = ftell(peFile_1);
    char* image1 = new char[sizeOfFile_1];
    fseek(peFile_1, 0, SEEK_SET);
    fread(image1, sizeOfFile_1, 1, peFile_1);//read the program1.exe
    fclose(peFile_1);

    PIMAGE_DOS_HEADER pdos = (PIMAGE_DOS_HEADER)image1;
    PIMAGE_NT_HEADERS32 pnt = (PIMAGE_NT_HEADERS32)(pdos->e_lfanew + image1);
    PIMAGE_SECTION_HEADER psec = (PIMAGE_SECTION_HEADER)(pnt + 1);
    size_t seccnt = pnt->FileHeader.NumberOfSections;

    if (!judgePeFile(image1)) {
        return 1;
    }

    //encrypt the program1.exe 
    for (BYTE* i = (BYTE*)image1; i < (BYTE*)image1 + sizeOfFile_1; i++) {
        *i ^= 0x40;
    }

    // Open and parse the program2.exe.
    fopen_s(&peFile_2, ".\\program2.exe", "rb");
    fseek(peFile_2, 0, SEEK_END);
    size_t sizeOfFile_2 = ftell(peFile_2);
    char* image2 = new char[sizeOfFile_2 + sizeOfFile_1];
    fseek(peFile_2, 0, SEEK_SET);
    fread(image2, sizeOfFile_2, 1, peFile_2);
    fclose(peFile_2);

    PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)image2;
    PIMAGE_NT_HEADERS32 pNt = (PIMAGE_NT_HEADERS32)(pDos->e_lfanew + image2);
    PIMAGE_SECTION_HEADER pSec = (PIMAGE_SECTION_HEADER)(pNt + 1);
    size_t secCnt = pNt->FileHeader.NumberOfSections;

    // Add a new section at the last of program2.exe , the section is the encrypted program1.exe.
    // The section of program2.exe plus.
    pNt->FileHeader.NumberOfSections++;
    pNt->OptionalHeader.SizeOfImage += sizeOfFile_1;

    // Loate the new section.
    pSec = (PIMAGE_SECTION_HEADER)(pNt + 1);
    PIMAGE_SECTION_HEADER pNewSec = pSec + secCnt;

    // Caculate the VA of new section.
    size_t lastSecLocation = pSec[secCnt - 1].VirtualAddress;
    size_t lastSecSize = pSec[secCnt - 1].Misc.VirtualSize;
    size_t newSecRva = lastSecLocation + Align(lastSecSize, pNt->OptionalHeader.SectionAlignment);

    // Copy the encrypted program1.exe into the new section.
    memcpy(pNewSec->Name, ".shell", 7);
    pNewSec->Misc.VirtualSize = Align(sizeOfFile_1, pNt->OptionalHeader.SectionAlignment);
    pNewSec->SizeOfRawData = sizeOfFile_1;
    pNewSec->VirtualAddress = newSecRva;
    pNewSec->PointerToRawData = sizeOfFile_2;
    pNewSec->Characteristics = IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE | IMAGE_SCN_CNT_CODE;
    char* cur = image2 + sizeOfFile_2;
    memcpy(cur, image1, sizeOfFile_1);
    fopen_s(&peFile_2, ".\\program2.exe", "wb");
    fwrite(image2, sizeOfFile_2 + sizeOfFile_1, 1, peFile_2);
    fclose(peFile_2);
    return 0;
}
