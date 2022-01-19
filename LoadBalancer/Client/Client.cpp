#include <iostream>
#include <thread>
#include <WS2tcpip.h>
#include <winsock2.h>

using namespace std;

#pragma comment(lib,"Ws2_32.lib")

#define SERVER_PORT 5001
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

	iResult = connect(clientSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress));
	if (iResult == SOCKET_ERROR)
	{
		cout << "Bad connect";
		return NULL;
	}

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

int main()
{
	printf("Client starting... \n");
	InitializeWindowsSockets();
	SOCKET clientSocket = NULL;

	bool returning = true;

	char access_buffer[ACCESS_BUFFER_SIZE];

	int iResult;

	while (1)
	{
		if (clientSocket == NULL)
			clientSocket = create_and_bind_socket();
		if (clientSocket == INVALID_SOCKET)
		{
			cout << "Error during transmission, closing socket";
			break;
		}
		if (returning)
		{
			memset(access_buffer, '1', ACCESS_BUFFER_SIZE);
			iResult = send(clientSocket, access_buffer, 1, 0);

			if (iResult == SOCKET_ERROR)
			{
				printf("recv failed with error: %d\n", WSAGetLastError());
				break;
			}
			returning = false;
			continue;
		}
		memset(access_buffer, 0, ACCESS_BUFFER_SIZE);

		iResult = recv(clientSocket, access_buffer, ACCESS_BUFFER_SIZE, 0);

		if (iResult == SOCKET_ERROR)
		{
			printf("recv failed with error: %d\n", WSAGetLastError());
			break;
		}
		cout << access_buffer << endl;
		returning = true;

		if (getchar() == 'q')
			break;
		closesocket(clientSocket);
		clientSocket = NULL;
	}
	cleanup(clientSocket);
}