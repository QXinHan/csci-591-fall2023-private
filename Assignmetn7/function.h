#pragma once
#include<iostream>
#include<Windows.h>
using namespace std;

struct BinaryFileInfo
{
	DWORD64 sizeOfFile = 0; //bytes 为单位的大小，比较的时候要除以12
	char* pFileDataByte = nullptr;
	int* pFileDataInt = nullptr;
};


struct _PNG_HEADER_CHUNK
{
	int Width;
	int Height;
	char BitDepth;
	char ColorType;
	char CompressionMethod;
	char FilterMethod;
	char InterlaceMethod;
}PNG_HEADER_CHUNK;

struct _CHUNK_DATA
{
	int Length;
	int ChunkType;
	char* pChunkData;//no more than 2^31-1, the pointer to image data
	int CRC;
};

class ClasifyMalware
{


private:

	FILE* pFile = nullptr;
	BinaryFileInfo FileInfo;

public:
	BinaryFileInfo* get_FileInfo();
	bool Load_Binary_File(char* Filename);
	bool Convert_To_Int(char* pFileData, int dataSize);
	bool Convert_Binary_To_PNG(int* pFileData, char* pathTopng, int dataSize);
	char* Get_Data_For_CRC(_CHUNK_DATA dataChunk);
	uint32_t Caculate_CRC32(_CHUNK_DATA dataChunk);
	int jud_File_Width(int fileSize);
	int jud_File_Height(int fileSize);
	void Free();
};






