#pragma once
#include<stdio.h>
#include<winnt.h>
void parseDataDirectory(char* file);
void parseExportDirectory(char* file);
void parseRelcDirectory(char* file);
void parseImportTbale(FILE* file);
void parseImportAddrTable(FILE* file);
void parseBoundImportTable(FILE* file);
void inforPrint(FILE* file);
DWORD Align(DWORD src, DWORD des);
bool Jude32or64(FILE* file);
void parseRcTable(FILE* file);
DWORD64 RVATOFOA(PVOID file_buffer, DWORD64 Rva);
DWORD64 RVATOFOA32(PVOID file_buffer, DWORD Rva);