#pragma once
#pragma once
#include<stdio.h>
#include<Windows.h>
#include<iostream>
#include<stdlib.h>
/*the only differece is optional header*/
class parsePE
{
private:
    PIMAGE_DOS_HEADER pDos = nullptr;
    PIMAGE_NT_HEADERS64 pNt64 = nullptr;
    PIMAGE_NT_HEADERS32 pNt32 = nullptr;
    PIMAGE_FILE_HEADER pFileHeader = nullptr;
    PIMAGE_OPTIONAL_HEADER64 pOptionalHeader64 = nullptr;
    PIMAGE_OPTIONAL_HEADER32 pOptionalHeader32 = nullptr;
    PIMAGE_SECTION_HEADER pSec = nullptr;

public:
    void parseDos(FILE* file);
    void parseFileHeader(FILE* file);
    void parseSignature(FILE* file);
    void parseOptionHeader32(FILE* file);
    void parseOptionHeader64(FILE* file);
    /*the members of section header are the same but we need to use optional header to get the section header, so it is important to devide into 32-bits and 64-bits*/
    void parseSecHeader32(FILE* file);
    void parseSecHeader64(FILE* file);
    void inforPrint(FILE* file);
    bool Jude32or64(FILE* file);
};