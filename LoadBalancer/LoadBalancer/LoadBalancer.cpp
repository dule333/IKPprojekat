// LoadBalancer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <WS2tcpip.h>
#include <iostream>
#include<winsock2.h>
//#include<thread>

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


struct Request {
	Request(sockaddr_in ca, char* mesage) {
		clientAdress = ca;
		message = mesage;
	}
	Request() {
		sockaddr_in help;
		memset(&help, 0, sizeof(sockaddr_in));
			clientAdress = help;
			message = (char *)"EMPTY_MESSAGE\0";
	}
	sockaddr_in clientAdress;
    char* message;
};


struct CircleBuffer {
	CircleBuffer() {
		pushId = 0;
		popId = 0;
		full = false;
	}
	Request BUFFER[10];
	int pushId = 0;
	int popId = 0;
	bool full = false;
	bool addElement(Request element) {

		if (pushId != popId || !full) {
			BUFFER[pushId] = element;
			pushId++;
			if (pushId == 10) pushId = 0;
			if (pushId == popId) full = true;
			
			return true;
		}
		return false;
	}
	Request removeElement() {

		if (pushId != popId || !full) {

			Request ret = BUFFER[popId];
			popId++;
			if (popId == 10) popId = 0;
			if (full) full = false;
			return ret;
		}
		Request ret;
		return ret;


	}

	int elementNum() {
		if (pushId < popId)
			return pushId + 10 - popId;
		return pushId - popId;
	}

};



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
				return ;
			}

			SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
			if (serverSocket == INVALID_SOCKET) {
				printf("SERVER SOCKET INITIALISATION FAILED! Error: %d \n", WSAGetLastError());
				return ;
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

	




int main()
{
	CircleBuffer CB;
	
	Reciever rec = Reciever(&CB);
	
	rec.start();




	
	/*
	sockaddr_in e;
	memset(&e, 0, sizeof(sockaddr_in));

	e.sin_port = SERVER_PORT;
	e.sin_family = AF_INET;
	e.sin_addr.S_un.S_addr = INADDR_ANY;

	char* message = (char *)"MESSAGE 1";
	char* message2 = (char *)"MESSAGE 2";
	char* message3 = (char *)"MESSAGE 3";

	Request r1 = Request(e, message);
	Request r2 = Request(e, message2);
	Request r3 = Request(e, message3);

	CB.addElement(r1);
	CB.addElement(r1);
	CB.addElement(r1);
	CB.addElement(r1);
	CB.addElement(r1);
	CB.addElement(r1);
	CB.addElement(r1);
	CB.addElement(r1);
	CB.addElement(r2);
	CB.addElement(r3);

	for (int i = 0; i < 10; i++) {
		Request ret = CB.removeElement();
		std::cout << ret.message;
	}


	*/
}

