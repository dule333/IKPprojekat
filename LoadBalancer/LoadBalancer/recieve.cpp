#include "common.h"
#include <WS2tcpip.h>
#include <iostream>
#include <winsock2.h>
#include<thread>

#pragma comment(lib,"Ws2_32.lib")


#define SERVER_PORT 5001
#define ACCESS_BUFFER_SIZE 1024
#define IP_ADRESS_LENGTH 16


bool SocketInit() {
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		printf("WSA STARTUP FAILED WITH ERROR %d \n", WSAGetLastError());
		return false;
	}
	return true;
}




void Recieve(CircleBuffer* CB, bool* stop) {

	printf("Recieving started! \n")

	sockaddr_in serverAdress;
	memset(&serverAdress, 0, sizeof(serverAdress));

	serverAdress.sin_port = SERVER_PORT;
	serverAdress.sin_addr.S_un.S_addr = INADDR_ANY;
	serverAdress.sin_family = AF_INET;

	if (!SocketInit()) {
		return;
	}

	SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (serverSocket == INVALID_SOCKET) {
		printf("SERVER SOCKET INITIALISATION FAILED! Error: %d \n", WSAGetLastError());
		return;
	}


	int iResult = bind(serverSocket, (LPSOCKADDR)&serverAdress, sizeof(serverAdress));
	if (iResult == SOCKET_ERROR) {
		printf("BINDING FAILED! ERROR: %d \n", WSAGetLastError());
		closesocket(serverSocket);
		return;
	}


	printf("Server Socket Initialised, Binding Done. Recieveing messages... \n");

	while (1) {
		if (&stop) break;
		sockaddr_in clientAddr;
		memset(&clientAddr, 0, sizeof(clientAddr));

		char messageBuffer[20];
		memset(&messageBuffer, 0, 20);
		int socksize = sizeof(sockaddr);
		iResult = recvfrom(serverSocket, messageBuffer, 20, 0, (LPSOCKADDR)&clientAddr, &socksize);
		if (iResult == SOCKET_ERROR) {

			printf("PROBLEM WHILE RECIEVING! ERROR: %d \n", WSAGetLastError());
			continue;

		}

		Request r = Request(clientAddr, messageBuffer);

		if (!CB->addElement(r)) {
			printf("CircleBuffer full! Waiting 3 seconds. \n");
			Sleep(3000);

			if (!CB->addElement(r)) printf("CircleBuffer Still Full, ignoring message\n");
			else printf("Added Request to CircleBuffer! Current number of elements in buffer: %d \n", CB->elementNum());
		}
		else 
			printf("Added Request to CircleBuffer! Current number of elements in buffer: %d \n", CB->elementNum());




	}

	printf("Reciever stopped. Elements left in CircleBuffer %d \n", CB->elementNum());



}

void start_reciever(CircleBuffer* CB, bool* stop) {
	std::thread t(Recieve, CB, stop);

	t.join();

	std::cout << "Reciever stopped working. \n";

}


/*

struct Reciever {
	Reciever(CircleBuffer* cb) {
		c = cb;

		memset(&serverAdress, 0, sizeof(serverAdress));

		serverAdress.sin_port = SERVER_PORT;
		serverAdress.sin_addr.S_un.S_addr = INADDR_ANY;
		serverAdress.sin_family = AF_INET;
		stop = false;
	}

	CircleBuffer* c;

	void Stop() {
		stop = true;
	}

	bool stop;

	sockaddr_in serverAdress;

	void start() {

		if (!SocketInit()) {
			return;
		}

		SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (serverSocket == INVALID_SOCKET) {
			printf("SERVER SOCKET INITIALISATION FAILED! Error: %d \n", WSAGetLastError());
			return;
		}

		int iResult = bind(serverSocket, (LPSOCKADDR)&serverAdress, sizeof(serverAdress));
		if (iResult == SOCKET_ERROR) {
			printf("BINDING FAILED! ERROR: %d \n", WSAGetLastError());
			closesocket(serverSocket);
			return;
		}

		printf("Server Socket Initialised, Binding Done. Recieveing messages... \n");

		while (1) {
			if (stop) break;
			sockaddr_in clientAddr;
			memset(&clientAddr, 0, sizeof(clientAddr));

			char messageBuffer[20];
			memset(&messageBuffer, 0, 20);
			int socksize = sizeof(sockaddr);
			iResult = recvfrom(serverSocket, messageBuffer, 20, 0, (LPSOCKADDR)&clientAddr, &socksize);
			if (iResult == SOCKET_ERROR) {

				printf("PROBLEM WHILE RECIEVING! ERROR: %d \n", WSAGetLastError());
				continue;

			}

			Request r = Request(clientAddr, messageBuffer);

			if (!c->addElement(r)) {
				printf("CircleBuffer full! Waiting 3 seconds. \n");
				Sleep(3000);

				if (!c->addElement(r)) printf("CircleBuffer Still Full, ignoring message\n");
				else printf("Added Request to CircleBuffer! Current number of elements in buffer: %d \n", c->elementNum());
			}




		}

		printf("Reciever stopped. Elements left in CircleBuffer %d \n", c->elementNum());


	}



};

*/