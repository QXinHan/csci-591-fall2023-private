#pragma once
#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<Windows.h>
#include<queue>
void parseDataDirectory64(char* file);
void parseDataDirectory32(char* file);
void parseExportDirectory(char* file);
void parseRelcDirectory(char* file);
void parseImportTbale(FILE* file);
void parseImportAddrTable(FILE* file);
void parseBoundImportTable(FILE* file);
void inforPrint(FILE* file);
DWORD Align(DWORD src, DWORD des);
bool Jude32or64(FILE* file);
void parseRcTable(FILE* file);
void dfsparseRcTable(FILE* file, PIMAGE_RESOURCE_DIRECTORY curDir, DWORD floor);
void bfsparseRcTable(FILE* file, DWORD floor);
DWORD64 RVATOFOA(PVOID file_buffer, DWORD64 Rva);