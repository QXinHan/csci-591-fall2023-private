#include<stdio.h>
#include<Windows.h>
#include<iostream>

//the path of the file
char readPath[] = ".\\testPNG.png";
//the path that the file to be written
char writePath_png[] = ".\\testPNG_write.png";
char writePath_txt[] = ".\\testPNG_write.txt";

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
		ExitProcess(1);
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
		ExitProcess(1);
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
			ExitProcess(1);//failed to open/create the files
	}

	//write the binary into the new files
	if (fwrite(fileData, sizeofFile, 1, fileWrite_txt)!=1 || fwrite(fileData, sizeofFile, 1, fileWrite_png)!=1)
	{
		std::cout << "Error:failed to write\n";
		fclose(file);
		fclose(fileWrite_txt);
		fclose(fileWrite_png);
		ExitProcess(1);
	}

	//close the stream of the file
	fclose(file);
	fclose(fileWrite_txt);
	fclose(fileWrite_png);

}


int main()
{
	read_write(readPath,writePath_png,writePath_txt);
	return 0;
}