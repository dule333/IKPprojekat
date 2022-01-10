#include "common.h"

#pragma comment(lib,"Ws2_32.lib")

#define SERVER_PORT 5001
#define ACCESS_BUFFER_SIZE 1024
#define IP_ADRESS_LENGTH 16


SOCKET create_and_bind_socket()
{
	sockaddr_in serverAddress;
	memset((char*)&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET; /*set server address protocol family*/
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(SERVER_PORT);

	int iResult = 0;
	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	u_long mode = 1;
	iResult = ioctlsocket(listenSocket, FIONBIO, &mode);
	if (iResult == SOCKET_ERROR)
		return NULL;

	iResult = bind(listenSocket, (LPSOCKADDR)&serverAddress, sizeof(serverAddress));
	if (iResult == SOCKET_ERROR)
		return NULL;

	return listenSocket;
}

void receiver()
{
	SOCKET interaction_socket = create_and_bind_socket();
	if (interaction_socket == NULL)
	{
		printf("An error has occured during the socket creation process.");
		WSACleanup();
		return;
	}

	FD_SET set;
	FD_ZERO(&set);
	FD_SET(interaction_socket, &set);

	timeval timeVal;
	timeVal.tv_sec = 0;
	timeVal.tv_usec = 5;

	char accessBuffer[ACCESS_BUFFER_SIZE];

	while (1)
	{
		memset(accessBuffer, 0, ACCESS_BUFFER_SIZE);

		int iResult = select(0, &set, NULL, NULL, &timeVal);
		if (iResult == SOCKET_ERROR)
		{
			printf("Error during communication.");
			return;
		}
		if (iResult == 0)
		{
			Sleep(10);
			continue;
		}

		//iResult = recv(interaction_socket, accessBuffer, )
	}
}

