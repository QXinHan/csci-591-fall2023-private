#include"function.h"

/*
* 
*/
char pngSignature[] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };
char pngEndChunk[] = { 0x0,0x0,0x0,0x0,0x49,0x45,0x4e,0x44,0xae,0x42,0x60,0x82 };
char testMalwarePath[] = "D:\\Downloads\\malware1";
BinaryFileInfo* ClasifyMalware::get_FileInfo()
{
	return &FileInfo;
}

inline bool ClasifyMalware::Load_Binary_File(char* pFilename) {
	if (pFilename == nullptr) {
		cout << "Error: The file name is NULL!" << endl;
		return false;
	}

	fopen_s(&pFile, (const char*)pFilename, "rb");

	if (pFile == nullptr) {
		cout << "Error: There is no such file!" << endl;
		return false;
	}

	fseek(pFile, 0, SEEK_END);
	FileInfo.sizeOfFile = ftell(pFile);
	FileInfo.sizeOfFile = FileInfo.sizeOfFile;

	if (FileInfo.sizeOfFile == 0) {
		cout << "Error: The size of the file is zero!" << endl;
		fclose(pFile);
		return false;
	}

	fseek(pFile, 0, SEEK_SET);

	FileInfo.pFileDataByte = (char*)malloc(FileInfo.sizeOfFile);

	if (FileInfo.pFileDataByte == nullptr) {
		cout << "Error: Fail to allocate the memory for the file data!" << endl;
		fclose(pFile);
		return false;
	}


	fread_s(FileInfo.pFileDataByte, FileInfo.sizeOfFile, FileInfo.sizeOfFile, 1, pFile);

	if (!Convert_To_Int(FileInfo.pFileDataByte, FileInfo.sizeOfFile)) {
		cout << "Error: Fail to convert byte data to int data" << endl;
		fclose(pFile);
		return false;
	}

	fclose(pFile);

	cout << "Successfully load the file in memory!" << endl;

	return true;

}

inline bool ClasifyMalware::Convert_To_Int(char* pFileData, int dataSize)
{
	FileInfo.pFileDataInt = (int*)malloc(dataSize * 4);

	if (FileInfo.pFileDataInt == nullptr) {
		cout << "Error: Fail to allocate memory for int data!" << endl;
		return false;
	}

	memset(FileInfo.pFileDataInt, 0, dataSize * 4);
	for (int i = 0; i < FileInfo.sizeOfFile; i++) {
		FileInfo.pFileDataInt[i] = (int)pFileData[i];
	}

	return true;
}

//这个函数用来转换得到的数据插入png的数据区，从而得到一个新的png图片
inline bool ClasifyMalware::Convert_Binary_To_PNG(int* pFileData, char* pathTopng, int dataSize)
{

	return true;
}

char* ClasifyMalware::Get_Data_For_CRC(_CHUNK_DATA dataChunk)
{
	char* data = (char*)malloc(dataChunk.Length + 4);

	if (data == nullptr) {
		cout << "Error: Fail to allocate mamory foe data!" << endl;
		return nullptr;
	}

	memcpy(data, (char*) & dataChunk + 4, 4);
	memcpy(data + 4, dataChunk.pChunkData, dataChunk.Length);

	return data;
}

inline uint32_t ClasifyMalware::Caculate_CRC32(_CHUNK_DATA dataChunk)
{
	char* data = Get_Data_For_CRC(dataChunk);
	int length = dataChunk.Length + 4;
	uint8_t i;
	uint32_t crc = 0xffffffff;        // Initial value
	while (length--)
	{
		crc ^= *data++;                // crc ^= *data; data++;
		for (i = 0; i < 8; ++i)
		{
			if (crc & 1)
				crc = (crc >> 1) ^ 0xEDB88320;// 0xEDB88320= reverse 0x04C11DB7
			else
				crc = (crc >> 1);
		}
	}
	data -= dataChunk.Length + 4;
	free(data);
	return ~crc;
}

inline int ClasifyMalware::jud_File_Width(int fileSize)
{
	//这里的size是bytes为单位的，除以1024再比较
	int temp = fileSize / 1024; //size单位转换成kb
	if (temp <= 10) {
		return 32;
	}
	else if (temp <= 30) {
		return 64;
	}
	else if (temp <= 60) {
		return 128;
	}
	else if (temp <= 100) {
		return 256;
	}
	else if (temp <= 200) {
		return 384;
	}
	else if (temp <= 500) {
		return 512;
	}
	else if (temp <= 1000) {
		return 768;
	}
	else {
		return 1024;
	}
	return 0;
}

inline int ClasifyMalware::jud_File_Height(int fileSize)
{
	float temp = (float)fileSize / 1024.0;
	if (temp <= 10) {
		return int(temp * 31.2);
	}
	else if (temp <= 30) {
		return int(156 + (temp - 10.0) * (468-156)/20.0);
	}
	else if (temp <= 60) {
		return int(234 + (temp - 30.0)*(468-234)/40.0);
	}
	else if (temp <= 100) {
		return int(234 + (temp - 60.0) * (390 - 234) / 40.0);
	}
	else if (temp <= 200) {
		return int(260 + (temp - 100.0) * (520 - 260) / 100.0);
	}
	else if (temp <= 500) {
		return int(390 + (temp - 200.0) * (976 - 390) / 300.0);
	}
	else if (temp <= 1000) {
		return int(651 + (temp-500.0)  * (1302 - 651) / 500.0);
	}
	else {
		return int(976 + (temp - 1000) * 1.1);
	}
	return 0;
}

void ClasifyMalware::Free()
{
	free(FileInfo.pFileDataByte);
	free(FileInfo.pFileDataInt);
	fclose(pFile);
}


int main(int argc, char* argv[])
{
	/*
	if (argc == 1) {
		cout << "Error: please pass a file name" << endl;
		exit(1);
	}
	*/
	

	char* binaryFileName = testMalwarePath;


	ClasifyMalware clasifyMalware;
	BinaryFileInfo* FileInfo;
	//Define the IHDR
	_CHUNK_DATA png_Header_Chunk_Data, dataForIDAT;
	_PNG_HEADER_CHUNK stableForIHDR;

	//填充IHDR数据块
	png_Header_Chunk_Data.Length = 13;
	int a = 218103808;
	png_Header_Chunk_Data.ChunkType = 0x52444849; // "IHDR"
	png_Header_Chunk_Data.pChunkData = (char*)malloc(png_Header_Chunk_Data.Length);
	if (png_Header_Chunk_Data.pChunkData == nullptr) {
		cout << "Error: Fail to allocate memory for IHDR data" << endl;
		exit(1);
	}


	clasifyMalware.Load_Binary_File(binaryFileName);
	//获取头数据块的结构体，并且填充数据块。
	FileInfo = clasifyMalware.get_FileInfo();
	stableForIHDR.Width = clasifyMalware.jud_File_Width(FileInfo->sizeOfFile);
	stableForIHDR.Height = clasifyMalware.jud_File_Height(FileInfo->sizeOfFile);
	stableForIHDR.BitDepth = 0x1;
	stableForIHDR.ColorType = 0x0;
	stableForIHDR.FilterMethod = 0x0;
	stableForIHDR.CompressionMethod = 0x0;
	stableForIHDR.InterlaceMethod = 0x0;
	// 把这些数据写入chunk data里面
	memcpy(png_Header_Chunk_Data.pChunkData, &stableForIHDR, 0xd);

	png_Header_Chunk_Data.CRC = clasifyMalware.Caculate_CRC32(png_Header_Chunk_Data);
	//获取二进制文件的数据，填充IDAT数据块
	dataForIDAT.ChunkType = 0x54414449;//"IDAT"
	dataForIDAT.Length = FileInfo->sizeOfFile*4;//char to int
	dataForIDAT.pChunkData = (char*)malloc(dataForIDAT.Length);
	memcpy(dataForIDAT.pChunkData, FileInfo->pFileDataInt, dataForIDAT.Length);
	dataForIDAT.CRC = clasifyMalware.Caculate_CRC32(dataForIDAT);

	//把所有的数据都用一个字节流存起来
	int ImageDatasize = 8 + 12 + png_Header_Chunk_Data.Length + 12 + dataForIDAT.Length + 12;
	char* AllTheImageData = (char*)malloc(ImageDatasize);
	//标志位
	int offset = 0;
	memcpy(AllTheImageData+offset, pngSignature, 8);
	offset += 8;
	//第一个Chunk
	memcpy(AllTheImageData + offset, &a, 4);
	_CHUNK_DATA* atemp = &png_Header_Chunk_Data;
	offset += 4;
	memcpy(AllTheImageData+offset,&png_Header_Chunk_Data.ChunkType , 4);
	offset += 4;
	memcpy(AllTheImageData + offset, png_Header_Chunk_Data.pChunkData, png_Header_Chunk_Data.Length);
	offset += png_Header_Chunk_Data.Length;
	memcpy(AllTheImageData + offset, &png_Header_Chunk_Data.CRC, 4);
	offset += 4;
	//第二个Chunk
	memcpy(AllTheImageData + offset, &dataForIDAT, 8);
	offset += 8;
	memcpy(AllTheImageData + offset, dataForIDAT.pChunkData, dataForIDAT.Length);
	offset += dataForIDAT.Length;
	memcpy(AllTheImageData + offset, &dataForIDAT.CRC, 4);
	offset += 4;
	// 第三个chunk
	memcpy(AllTheImageData + offset, pngEndChunk, 12);


	FILE* malwarePNG = nullptr;
	fopen_s(&malwarePNG, "D:\\Downloads\\malware1.png", "wb");
	fwrite(AllTheImageData, ImageDatasize, 1, malwarePNG);
	
	clasifyMalware.Free();
	fclose(malwarePNG);
	free(png_Header_Chunk_Data.pChunkData);
	free(dataForIDAT.pChunkData);

	return 0;
}


