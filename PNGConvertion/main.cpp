#include<stdio.h>
#include<Windows.h>
#include<iostream>

//the path of the file
char readPath[] = ".\\testPNG.png";
//the path that the file to be written
char writePath_png[] = ".\\testPNG_write.png";
char writePath_txt[] = ".\\testPNG_write.txt";
char readTXTPath[] = ".\\testPNG.txt";
char writePNGPath[] = ".\\ConvertToPNG.png";
//read the file and write
void read_write(char* readPath,char* writePath_png,char* writePath_txt)
{
	FILE* file = nullptr;
	FILE* fileWrite_txt = nullptr;
	FILE* fileWrite_png = nullptr;
	size_t sizeofFile = 0;
	//read the file 
	if (fopen_s(&file, readPath, "rb"))
	{
		std::cout << "Error:failed to open the file\n";
		return;
	}
	//get the size of the file
	fseek(file, 0, SEEK_END);
	sizeofFile = ftell(file);
	fseek(file, 0, SEEK_SET);

	//allocate the memory to store the file data
	byte* fileData = (byte*)malloc(sizeofFile);
	if (fileData == nullptr)
	{
		std::cout << "Error:failed to allocate the memory\n";
		fclose(file);
		return;
	}
	//read the information of the file to memory
	fread_s(fileData, sizeofFile, sizeofFile, 1, file);

	//create the new files and open them
	if (fopen_s(&fileWrite_txt, writePath_txt, "wb") || fopen_s(&fileWrite_png, writePath_png, "wb"))
	{
			std::cout << "Error:failed to open the file\n";
			fclose(file);
			fclose(fileWrite_txt);
			fclose(fileWrite_png);
			return;//failed to open/create the files
	}

	//write the binary into the new files
	if (fwrite(fileData, sizeofFile, 1, fileWrite_txt)!=1 || fwrite(fileData, sizeofFile, 1, fileWrite_png)!=1)
	{
		std::cout << "Error:failed to write\n";
		fclose(file);
		fclose(fileWrite_txt);
		fclose(fileWrite_png);
		return;
	}


	/*
	*
	*To print the data of the png byte by byte
	* 
	*/
	//
	//for (int i = 0; i < sizeofFile; i++) printf("%c", fileData[i]);



	//close the stream of the file
	fclose(file);
	fclose(fileWrite_txt);
	fclose(fileWrite_png);

}

//Get the size of the content in the txt
size_t GetContentSize(char* readPath)
{
	FILE* readFile = nullptr;
	size_t sizeOfData = 0;//to count the size of the png data

	//open the text file 
	if (fopen_s(&readFile, readPath, "rb"))
	{
		std::cout << "Error:fail to open the file\n";
		return 0;
	}

	//count the size of the data 
	char ch;
	while ((ch = fgetc(readFile)) != EOF) sizeOfData++;

	fclose(readFile);
	return sizeOfData;

}

//read the content in the txt and convert them into PNG
void Convert(char* readPath, char* writePath)
{
	FILE* readFile = nullptr;
	FILE* writeFile = nullptr;
	size_t sizeOfData = GetContentSize(readPath);
	BYTE* data = (BYTE*)malloc(sizeOfData);//allocate the memory for the data in text

	//open the text file 
	if (fopen_s(&readFile, readPath, "r"))
	{
		std::cout << "Error:fail to open the file\n";
		return;
	}

	//read the content into the memory
	memset(data, 0, sizeOfData);
	for (int i = 0; i < sizeOfData; i++)
	{
		data[i] = fgetc(readFile);
	}
	
	//open the file that I will write in
	if (fopen_s(&writeFile, writePath, "wb"))
	{
		std::cout << "Error:fail to open the file\n";
		fclose(readFile);
		return;
	}

	//write the data byte by byte into the file
	if (fwrite(data, sizeOfData, 1, writeFile) != 1)
	{
		std::cout << "Error:failed to write\n";
		fclose(readFile);
		fclose(writeFile);
		return;
	}

	fclose(readFile);
	fclose(writeFile);
	return;
}

void printTheContent(char* readPath)
{
	FILE* readFile = nullptr;

	//open the text file 
	if (fopen_s(&readFile, readPath, "r"))
	{
		std::cout << "Error:fail to open the file\n";
		return;
	}

	//count the size of the data 
	char ch;
	while ((ch = fgetc(readFile)) != EOF)
	{
		printf("%c", ch);
	}

	fclose(readFile);
}

int main()
{
	//printTheContent(readTXTPath);
	//Convert(writePath_txt, writePNGPath);
	read_write(readPath, writePath_png, writePath_txt);
	return 0;
}