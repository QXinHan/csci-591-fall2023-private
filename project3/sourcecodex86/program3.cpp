/*
这篇代码的目的：
1.给program1.exe 用0x40加密
2.将这一整个exe放入program2.exe中
*/
#include <iostream>
#include<stdio.h>
#include<stdlib.h>
#include<Windows.h>

bool judgePeFile(char* image) {
    if (image == NULL) {
        printf("Error:parameter is NULL!");
        return FALSE;
    }
    PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)image;
    PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)(image + pDos->e_lfanew);
    //   printf("0x%x\n",pNtHeader->Signature);
    if (pNtHeader->Signature == IMAGE_NT_SIGNATURE) {
        return TRUE;
    }
    else {
        printf("Error: This is not a PE file!");
        return FALSE;
    }
}


int main()
{
    FILE* peFile_1;
    FILE* peFile_2;
    fopen_s(&peFile_1, ".\\program1.exe", "rb");
    fseek(peFile_1, 0, SEEK_END);
    size_t sizeOfFile_1 = ftell(peFile_1);
    char* image1 = new char[sizeOfFile_1];
    fseek(peFile_1, 0, SEEK_SET);
    fread(image1, sizeOfFile_1, 1, peFile_1);
    fclose(peFile_1);
 //解析并且加密program1.exe
    PIMAGE_DOS_HEADER pdos = (PIMAGE_DOS_HEADER)image1;
    PIMAGE_NT_HEADERS pnt = (PIMAGE_NT_HEADERS)(pdos->e_lfanew + image1);
    PIMAGE_SECTION_HEADER psec = (PIMAGE_SECTION_HEADER)(pnt + 1);
    size_t seccnt = pnt->FileHeader.NumberOfSections;
   if (!judgePeFile(image1)) {
        return 1;
   }
   for (BYTE* i = (BYTE*)image1; i < (BYTE*)image1 + sizeOfFile_1; i++) {
        *i ^= 0x40;
    }
    //把一整个program1.exe加密成功后，我需要把这一块代码放到program2.exe中
    fopen_s(&peFile_2, ".\\program2.exe", "rb");
    fseek(peFile_2, 0, SEEK_END);
    size_t sizeOfFile_2 = ftell(peFile_2);
    char* image2 = new char[sizeOfFile_2 + sizeOfFile_1];
    fseek(peFile_2, 0, SEEK_SET);
    fread(image2, sizeOfFile_2, 1, peFile_2);
    fclose(peFile_2);
    //开始解析program2.exe的pe格式
    PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)image2;
    PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + image2);
    PIMAGE_SECTION_HEADER pSec = (PIMAGE_SECTION_HEADER)(pNt + 1);
    size_t secCnt = pNt->FileHeader.NumberOfSections;
    //节点加一，大小加上新节点大小
    pNt->FileHeader.NumberOfSections++;
   // std::cout << pNt->FileHeader.NumberOfSections << std::endl;
    pNt->OptionalHeader.SizeOfImage += sizeOfFile_1;
    //定位新节点
    pSec = (PIMAGE_SECTION_HEADER)(pNt + 1);
    PIMAGE_SECTION_HEADER pNewSec = pSec + secCnt;
    //算出新增节点的虚拟地址
    size_t lastSecLocation = pSec[secCnt - 1].VirtualAddress;
    size_t lastSecSize = pSec[secCnt - 1].Misc.VirtualSize;
    size_t newSecRva = lastSecLocation + lastSecSize + (0x1000 - (lastSecSize & 0xfff)& 0xfff);
    //给新增节点写入相关信息
    memcpy(pNewSec->Name, ".shell", 7);
    pNewSec->Misc.VirtualSize = sizeOfFile_1 + (0x1000 - (sizeOfFile_1 & 0xfff) & 0xfff);
    pNewSec->SizeOfRawData = sizeOfFile_1;
    pNewSec->VirtualAddress = newSecRva;
    pNewSec->PointerToRawData = sizeOfFile_2;
    pNewSec->Characteristics = IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE | IMAGE_SCN_CNT_CODE;
    //填充新增节点
    char* cur = image2 + sizeOfFile_2;
//    image2 = image2 + sizeOfFile_2;
//    if (image2 == cur) printf("好！");image2和cur是相同的
    //但是如果把cur换成image2那么所得到的program2.exe无法被PE-Bear打开
    memcpy(cur, image1, sizeOfFile_1);
    fopen_s(&peFile_2, ".\\program2.exe", "wb");
    fwrite(image2, sizeOfFile_2 + sizeOfFile_1, 1, peFile_2);
    fclose(peFile_2);
    return 0;
}


