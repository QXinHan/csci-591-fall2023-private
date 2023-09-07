#include<stdio.h>
#include<stdlib.h>
#include <string>
#include<vector>
#include<iostream>
#include<Windows.h>
#include"function.h"
using namespace std;
int main(int argc,char* argv[])
{
	char desfile[] = "C:\\Users\\86182\\Desktop\\Project1.exe";
	FILE* fileptr = NULL;
	char* disfileptr = NULL;
	char* imagebuffer = NULL;
	DWORD Exfilebuffer;
	size_t sizeOfFile;
	fopen_s(&fileptr, desfile, "rb");
	fseek(fileptr, 0, SEEK_END);
	sizeOfFile = ftell(fileptr);
	fseek(fileptr, 0, SEEK_SET);
	disfileptr = (char*)malloc(sizeOfFile);
	if (disfileptr == NULL)
	{
		printf("fail to allocate memory for the file\n");
		return 1;
	}
	fread_s(disfileptr, sizeOfFile, sizeOfFile, 1, fileptr);//load in the memory
	ExTheLastSection(disfileptr, 0x1000);

	

	fclose(fileptr);
	free(disfileptr);
	free(imagebuffer);
	return 0;
}

int rwFile(char* filename)//allocate memory for file in disk 
{
	//read file and get size of file
	FILE* fileptr = NULL;
	char* ptr = NULL;
	size_t sizeOfFile = 0;
	fopen_s(&fileptr, filename, "rb");
	if (fileptr == NULL)
	{
		printf("The file is NULL!\n");
		return -1;
	}
	fseek(fileptr, 0, SEEK_END);
	sizeOfFile = ftell(fileptr);
	fseek(fileptr, 0, SEEK_SET);
	//allcate memory for file
	ptr = (char*)malloc(sizeOfFile);
	if (ptr == NULL)
	{
		printf("Fail to allocate memory\n");
		return -1;
	}
	memset(ptr, 0, sizeOfFile);
	fread_s(ptr, sizeOfFile, sizeOfFile, 1, fileptr);
	printf("The file address in memory: %x\n", ptr);
	fileptr = NULL;
	fopen_s(&fileptr, "D:\\Cprogram\\CheckMe\\text.exe", "wb");
	if (fileptr == NULL)
	{
		printf("Fail to open file\n");
		return -1;
	}
	fwrite(ptr, sizeOfFile, 1, fileptr);

	fclose(fileptr);
	free(ptr);
	return 1;
}

DWORD writeFile(char* fileBuffer)
{
	FILE* fileptr = NULL;
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)fileBuffer;
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(fileBuffer + pDos->e_lfanew);
	PIMAGE_SECTION_HEADER pSec = (PIMAGE_SECTION_HEADER)(pNt + 1);
	DWORD numOfSections = pNt->FileHeader.NumberOfSections;
	DWORD sizeOfFile = ((PIMAGE_SECTION_HEADER)(pSec + numOfSections - 1))->PointerToRawData + ((PIMAGE_SECTION_HEADER)(pSec + numOfSections - 1))->SizeOfRawData;
	if (fileBuffer == NULL)
	{
		return -1;
	}
	fopen_s(&fileptr, "D:\\Cprogram\\CheckMe\\text.exe", "wb");
	if (fileptr == NULL)
	{
		printf("Fail to open file\n");
		return -1;
	}
	fwrite(fileBuffer, sizeOfFile, 1, fileptr);
	fclose(fileptr);
	return sizeOfFile;
}

void parseDos(FILE* file)
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
	printf("Magic:%x\n", pOptionalHeader->Magic);
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

void parseSecHeader(FILE* file)
{
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)file;
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)((char*)file + pDos->e_lfanew);
	PIMAGE_FILE_HEADER pFileHeader = (PIMAGE_FILE_HEADER)((char*)pNt + 4);
	PIMAGE_SECTION_HEADER pSec = (PIMAGE_SECTION_HEADER)(pNt+1);
	size_t numberOfSec = pFileHeader->NumberOfSections;
	printf("---------------------------------------------\n");
	printf("SECTION_HEADER\n");
	for (size_t i = 0; i < numberOfSec; i++)
	{
		PIMAGE_SECTION_HEADER pSecHeader = pSec+i;
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
		printf("\n");
	}
}
void parseDataDirectory(char* file)
{
	printf("---------------------------------------------\n");
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)file;
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)((char*)file + pDos->e_lfanew);
	for (int i = 0; i < IMAGE_NUMBEROF_DIRECTORY_ENTRIES; i++)
	{
		printf("vietualAddress:%x\n", pNt->OptionalHeader.DataDirectory[i].VirtualAddress);
		printf("size:%x\n", pNt->OptionalHeader.DataDirectory[i].Size);
		printf("--------\n");
	}
}
void inforPrint(FILE* file)
{
	parseDos(file);
	parseNt(file);
	parseFileHeader(file);
	parseOptionHeader(file);
	parseSecHeader(file);
	parseDataDirectory((char*)file);
	parseDataDirectory((char*)file);
}


DWORD FileBufferToImageBuffer(char* fileBuffer, char** imageBuffer)
{
	char* ImageBuffer = NULL;
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)fileBuffer;
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)((char*)pDos + pDos->e_lfanew);
	PIMAGE_FILE_HEADER pFileHeader = (PIMAGE_FILE_HEADER)((char*)pNt + 4);
	PIMAGE_OPTIONAL_HEADER pOptionalHeader = PIMAGE_OPTIONAL_HEADER((char*)pFileHeader + sizeof(IMAGE_FILE_HEADER));
	PIMAGE_SECTION_HEADER pSec = PIMAGE_SECTION_HEADER(pNt + 1);
	DWORD numOfSections = pFileHeader->NumberOfSections;
	DWORD sizeOfImage = pOptionalHeader->SizeOfImage;
	ImageBuffer = (char*)malloc(sizeOfImage);
	if (ImageBuffer == NULL)
	{
		printf("fail to allocate memory\n");
		return 0;
	}
	memset(ImageBuffer, 0, sizeOfImage);
	memcpy(ImageBuffer, fileBuffer, pOptionalHeader->SizeOfHeaders);
	for (int i = 0; i < numOfSections; i++)
	{
		memcpy(ImageBuffer + pSec[i].VirtualAddress, fileBuffer + pSec[i].PointerToRawData, pSec[i].SizeOfRawData);
	}
	*imageBuffer = ImageBuffer;
	return sizeOfImage;
}

DWORD ImageBufferToFileBuffer(char** fileBuffer, char* ImageBuffer)
{
	char* FileBuffer = NULL;
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)ImageBuffer;
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)((char*)pDos + pDos->e_lfanew);
	PIMAGE_FILE_HEADER pFileHeader = (PIMAGE_FILE_HEADER)((char*)pNt + 4);
	PIMAGE_OPTIONAL_HEADER pOptionalHeader = PIMAGE_OPTIONAL_HEADER((char*)pFileHeader + sizeof(IMAGE_FILE_HEADER));
	PIMAGE_SECTION_HEADER pSec = PIMAGE_SECTION_HEADER(pNt + 1);
	DWORD numOfSections = pFileHeader->NumberOfSections;
	DWORD sizeOfFile = ((PIMAGE_SECTION_HEADER)(pSec + numOfSections - 1))->PointerToRawData + ((PIMAGE_SECTION_HEADER)(pSec + numOfSections - 1))->SizeOfRawData;
	FileBuffer = (char*)malloc(sizeOfFile);
	if (FileBuffer == NULL)
	{
		return 0;
	}
	memset(FileBuffer,0 , sizeOfFile);
	memcpy(FileBuffer, ImageBuffer, pOptionalHeader->SizeOfHeaders);
	for (int i = 0; i < numOfSections; i++)
	{
		memcpy(FileBuffer + pSec->PointerToRawData, ImageBuffer + pSec->VirtualAddress, pSec->SizeOfRawData);
		pSec++;
	}
	*fileBuffer = FileBuffer;
	return sizeOfFile;
}
DWORD RVATOFOA(PVOID file_buffer, DWORD Rva) 
{
	PIMAGE_DOS_HEADER pdos = (PIMAGE_DOS_HEADER)file_buffer;
	PIMAGE_NT_HEADERS pnt = (PIMAGE_NT_HEADERS)(pdos->e_lfanew + (DWORD)file_buffer);
	PIMAGE_SECTION_HEADER psec = (PIMAGE_SECTION_HEADER)(pnt + 1);
	if (Rva <= pnt->OptionalHeader.SizeOfHeaders)
	{
		return Rva;
	}
	for (WORD i = 0; i < pnt->FileHeader.NumberOfSections; i++)
	{
		if (Rva >= psec[i].VirtualAddress && Rva < psec[i].VirtualAddress + psec[i].Misc.VirtualSize)
		{

			DWORD offset = Rva - psec[i].VirtualAddress;
			DWORD FOA = offset + psec[i].PointerToRawData;
			return FOA;
		}
	}
	return -1;
}
//存盘成功了，但是最后一个节点是重定位表，然后我给他改错了tmd 7/9/2023.
//现在一个一个看，看看哪里有问题
//报错原因是找不到dll导致无法继续执行代码，因为最后一个我修改的节点是.reloc这个是重定位表。
//用PE打开了生成的文件，发现有一些代码没变。
DWORD ExTheLastSection(char* fileBuffer,DWORD sizeOfEx)
{
	char* imagebuffer = NULL;
	char* Eximagebuffer = NULL;
	char* Exfilebuffer = NULL;

	//filebuffer to imagebuffer
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)fileBuffer;
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(fileBuffer + pDos->e_lfanew);
	PIMAGE_SECTION_HEADER pSec = (PIMAGE_SECTION_HEADER)(pNt + 1);
	DWORD numOfSections = pNt->FileHeader.NumberOfSections;
	pSec = pSec + numOfSections - 1;
	imagebuffer = (char*)malloc(pNt->OptionalHeader.SizeOfImage);
	if (imagebuffer == NULL)
	{
		printf("fail to allocate memory");
		return 0;
	}
	memset(imagebuffer, 0, pNt->OptionalHeader.SizeOfImage);
	memcpy(imagebuffer, fileBuffer, pNt->OptionalHeader.SizeOfHeaders);
	pSec = (PIMAGE_SECTION_HEADER)(pNt + 1);
	for (int i = 0; i < numOfSections; i++)
	{
		memcpy(imagebuffer + pSec[i].VirtualAddress, fileBuffer + pSec[i].PointerToRawData, pSec[i].SizeOfRawData);
	}

	//Expand the imagebuffer to Eximagebuffer 
	DWORD sizePreImageSize;
	DWORD size;
	char* cpysize = NULL;
	pDos = (PIMAGE_DOS_HEADER)imagebuffer;
	pNt = (PIMAGE_NT_HEADERS)(imagebuffer + pDos->e_lfanew);
	pSec = (PIMAGE_SECTION_HEADER)(pNt + 1);
	sizePreImageSize = pNt->OptionalHeader.SizeOfImage;
	numOfSections = pNt->FileHeader.NumberOfSections;
	pSec += numOfSections - 1;
	size = max(pSec->SizeOfRawData, Align(pSec->Misc.VirtualSize, 0x1000));
	pSec->SizeOfRawData = Align(sizeOfEx + size,0x200);
	pSec->Misc.VirtualSize = size + Align(sizeOfEx,0x1000);
	pNt->OptionalHeader.SizeOfImage = pSec->VirtualAddress + pSec->Misc.VirtualSize;
	//cpysize = (char*)&pSec->SizeOfRawData;

	//copy imagebuffer to Eximagebuffer
	Eximagebuffer = (char*)malloc(pNt->OptionalHeader.SizeOfImage);
	memset(Eximagebuffer, 0, pNt->OptionalHeader.SizeOfImage);
	memcpy(Eximagebuffer, imagebuffer, sizePreImageSize);

	//Eximagebuffer to Exfilebuffer
	pDos = (PIMAGE_DOS_HEADER)Eximagebuffer;
	pNt = (PIMAGE_NT_HEADERS)(Eximagebuffer + pDos->e_lfanew);
	PIMAGE_FILE_HEADER pFileHeader = (PIMAGE_FILE_HEADER)((char*)pNt + 4);
	PIMAGE_OPTIONAL_HEADER pOptionalHeader = PIMAGE_OPTIONAL_HEADER((char*)pFileHeader + sizeof(IMAGE_FILE_HEADER));
	pSec = PIMAGE_SECTION_HEADER(pNt + 1);
	numOfSections =	pNt->FileHeader.NumberOfSections;
	DWORD sizeOfFile = ((PIMAGE_SECTION_HEADER)(pSec + numOfSections - 1))->PointerToRawData + ((PIMAGE_SECTION_HEADER)(pSec + numOfSections - 1))->SizeOfRawData;
	Exfilebuffer = (char*)malloc(sizeOfFile);
	if (Exfilebuffer == NULL)
	{
		printf("fail to allocate memory");
		return 0;
	}
	memset(Exfilebuffer, 0, sizeOfFile);
	memcpy(Exfilebuffer, Eximagebuffer, pOptionalHeader->SizeOfHeaders);
	for (int i = 0; i < numOfSections; i++)
	{
		memcpy(Exfilebuffer + pSec[i].PointerToRawData, Eximagebuffer + pSec[i].VirtualAddress, pSec->SizeOfRawData);
	}
	
	//试一下用以下方式把一些地方的东西改了
	/*
	pDos = (PIMAGE_DOS_HEADER)Exfilebuffer;
	pNt = (PIMAGE_NT_HEADERS)(pDos->e_lfanew + Exfilebuffer);
	pSec = (PIMAGE_SECTION_HEADER)(pNt + 1);
	pSec += numOfSections - 1;
	memcpy(Exfilebuffer+pSec->PointerToRawData, cpysize, 4);
	memcpy(Exfilebuffer + pSec->PointerToRawData + 4, cpysize, 4);
	*/

	writeFile(Exfilebuffer);
	free(Eximagebuffer);
	free(Exfilebuffer);
	free(imagebuffer);
	return sizeOfEx;
}

DWORD Align(DWORD src, DWORD des)
{
	return des*(src/des) + ((src%des == 0)?0:des);
}
