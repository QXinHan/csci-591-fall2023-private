
#include <SFML/Graphics.hpp>
#include <iostream>
using namespace std;

/*

// Create the image
sf::Image image;
image.create(width, height);
*/

struct FileDataInfo
{
	char* pFileDataByte;
	int size;
};
FileDataInfo fileData;
int* IntData = nullptr;
char path[] = "D:\\gitspace\\csci-591-fall2023-private\\Assignmetn7\\assignment7-main\\";

char* numberMalware[] = {
	(char*)"malware1",
	(char*)"malware2",
	(char*)"malware3",
	(char*)"malware4",
	(char*)"malware5",
	(char*)"malware6",
	(char*)"malware7",
	(char*)"malware8",
	(char*)"malware9",
	(char*)"malware10",
	(char*)"malware11",
	(char*)"malware12",
	(char*)"malware13",
};

int jud_File_Width(int fileSize)
{
	
	int temp = fileSize / 1024; 
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

bool loadMalwareFile(char* pFilename) {
	//free(fileData.pFileDataByte);
	FILE* pFile = nullptr;
	int sizeOfFile = 0;
	char* pFileDataByte = nullptr;
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

	sizeOfFile = ftell(pFile);

	if (sizeOfFile == 0) {
		cout << "Error: The size of the file is zero!" << endl;
		fclose(pFile);
		return false;
	}

	fseek(pFile, 0, SEEK_SET);

	pFileDataByte = (char*)malloc(sizeOfFile);

	if (pFileDataByte == nullptr) {
		cout << "Error: Fail to allocate the memory for the file data!" << endl;
		fclose(pFile);
		return false;
	}

	fread_s(pFileDataByte, sizeOfFile, sizeOfFile, 1, pFile);

	fclose(pFile);
	fileData.size = sizeOfFile;
	fileData.pFileDataByte = pFileDataByte;


	return true;


}

int* converToInt(char* ByteData) {
	IntData = (int*)malloc(fileData.size * 4);
	for (int i = 0; i < fileData.size; i++) {
		IntData[i] = (int)fileData.pFileDataByte[i];
	}
	return IntData;
}

void convert(char* filepath, char* topath) {

    //Create the image
    sf::Image image;
	int count = 0;
	int width = 0;
	int height = 0;
	while (true) {

		char filename[_MAX_PATH] = { 0 };
		memcpy(filename, filepath, 74);
		strcat_s(filename, numberMalware[count]);

		char saveToFile[_MAX_PATH] = { 0 };
		memcpy(saveToFile, filename, 74);
		strcat_s(saveToFile, numberMalware[count]);
		strcat_s(saveToFile, ".png");

		if (!loadMalwareFile(filename)) {
			cout << "Finish converting!" << endl;
			break;
		}

		width = jud_File_Width(fileData.size);
		height = fileData.size / width + (((width % width) == 0) ? 0 : 1);
		char* fileDataInByte = fileData.pFileDataByte;

		image.create(width, height);

		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				// Set the pixel color based on the data from the file
				int curByte = (int)fileDataInByte[x + y * width];
				sf::Color color(curByte, curByte, curByte);
				image.setPixel(x, y, color);
			}
		}

		image.saveToFile(saveToFile);

		count++;
	}



}

int main()
{
	convert(path, path);
	//free(IntData);
	free(fileData.pFileDataByte);
    return 0;
}
