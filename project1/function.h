#pragma once
#include<stdio.h>
#include<winnt.h>
void parseDos(FILE* file);
void parseNt(FILE* file);
void parseFileHeader(FILE* file);
void parseOptionHeader(FILE* file);
void parseSecHeader32(FILE* file);
void parseSecHeader64(FILE* file);
void inforPrint(FILE* file);
bool Jude32or64(FILE* file);
