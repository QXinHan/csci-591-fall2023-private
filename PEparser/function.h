#pragma once
#include<stdio.h>
#include<winnt.h>
int rwFile(char* filename);
DWORD writeFile(char* fileBuffer);
void parseDos(FILE* file);
void parseNt(FILE* file);
void parseFileHeader(FILE* file);
void parseOptionHeader(FILE* file);
void parseSecHeader(FILE* file);
void parseDataDirectory(char* file);
void inforPrint(FILE* file);
DWORD FileBufferToImageBuffer(char* fileBuffer, char**imageBuffer);
DWORD ImageBufferToFileBuffer(char** fileBuffer, char* ImageBuffer);
DWORD RVATOFOA(PVOID file_buffer, DWORD Rva);
DWORD ExTheLastSection(char* fileBuffer,DWORD szieOfEx);
DWORD Align(DWORD src, DWORD des);
