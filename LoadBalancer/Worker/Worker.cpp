#include <iostream>
#include <thread>
#include <WS2tcpip.h>
#include <winsock2.h>

using namespace std;

#pragma comment(lib,"Ws2_32.lib")

#define SERVER_PORT 5002
#define ACCESS_BUFFER_SIZE 1024
#define IP_ADRESS_LENGTH 16
#define IP_ADDRESS "127.0.0.1"

bool InitializeWindowsSockets()
{
	WSADATA wsaData;
	// Initialize windows sockets library for this process
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		printf("WSAStartup failed with error: %d\n", iResult);
		return false;
	}
	return true;
}

SOCKET create_and_bind_socket()
{
	sockaddr_in serverAddress;
	memset((char*)&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET; /*set server address protocol family*/
	inet_pton(AF_INET, IP_ADDRESS, &(serverAddress.sin_addr.s_addr));
	serverAddress.sin_port = htons(SERVER_PORT);

	int iResult = 0;
	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	iResult = connect(clientSocket, (LPSOCKADDR)&serverAddress, sizeof(serverAddress));
	if (iResult == SOCKET_ERROR)
		return NULL;

	return clientSocket;
}

void cleanup(SOCKET socket)
{
	int iResult;
	iResult = closesocket(socket);
	if (iResult == SOCKET_ERROR)
	{
		printf("closesocket failed with error: %d\n", WSAGetLastError());
		return;
	}

	iResult = WSACleanup();
	if (iResult == SOCKET_ERROR)
	{
		printf("WSACleanup failed with error: %d\n", WSAGetLastError());
		return;
	}
}

void execute() {
	this_thread::sleep_for(chrono::seconds(1));
}

int main()
{
		printf("Worker starting... \n");
		InitializeWindowsSockets();
		SOCKET clientSocket = create_and_bind_socket();
		char bufferfornothing[1024];


		bool returning = false;
		
		char access_buffer[ACCESS_BUFFER_SIZE];
		char* halt_order = (char*)malloc(3);
		if (halt_order == 0)
			return 1;
		strcpy_s(halt_order, 3, "-1");
		timeval timeVal;
		timeVal.tv_sec = 0;
		timeVal.tv_usec = 5;
		FD_SET set;

		strcpy_s(access_buffer, 3, "ok");

		int iResult = send(clientSocket, access_buffer, 3, 0);
		if (iResult == SOCKET_ERROR)
		{
			cleanup(clientSocket);
			return 1;
		}

		while (1)
		{
			FD_ZERO(&set);
			FD_SET(clientSocket, &set);
			if (returning)
			{
				iResult = select(0, NULL, &set, NULL, &timeVal);
				if (iResult == SOCKET_ERROR)
				{
					printf("Error during communication.");
					cleanup(clientSocket);
					return 1;
				}
				if (iResult == 0)
				{
					Sleep(10);
					continue;
				}
				iResult = send(clientSocket, access_buffer, strlen(access_buffer) + 1, 0);

				if (iResult == SOCKET_ERROR)
				{
					printf("send failed with error: %d\n", WSAGetLastError());
					break;
				}
				returning = false;
				continue;
			}
			memset(access_buffer, 0, ACCESS_BUFFER_SIZE);

			iResult = select(0, &set, NULL, NULL, &timeVal);
			if (iResult == SOCKET_ERROR)
			{
				printf("Error during communication.");
				cleanup(clientSocket);
				return 1;
			}
			if (iResult == 0)
			{
				Sleep(10);
				continue;
			}
			
			iResult = recv(clientSocket, bufferfornothing, ACCESS_BUFFER_SIZE, 0);

			if (iResult == SOCKET_ERROR)
			{
				printf("recv failed with error: %d\n", WSAGetLastError());
				break;
			}

			if (!strcmp(halt_order, bufferfornothing))
			{
				cleanup(clientSocket);
				free(halt_order);
				return 0;
			}


			execute();
			returning = true;
		}
}