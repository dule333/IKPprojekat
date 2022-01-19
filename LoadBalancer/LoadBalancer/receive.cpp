#include "common.h"

#pragma comment(lib,"Ws2_32.lib")

#define SERVER_PORT 5001
#define ACCESS_BUFFER_SIZE 1024
#define IP_ADRESS_LENGTH 16

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

SOCKET create_and_bind_socket(int port)
{
	sockaddr_in serverAddress;
	memset((char*)&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(port);

	int iResult = 0;
	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
	if (listenSocket == INVALID_SOCKET)
	{
		printf("Creating socket failed with error: %d\n", WSAGetLastError());
		cleanup(listenSocket);
		return NULL;
	}

	u_long mode = 1;
	iResult = ioctlsocket(listenSocket, FIONBIO, &mode);
	if (iResult == SOCKET_ERROR)
	{
		cout << "Bad FIONBIO.";
		return NULL;
	}
	iResult = bind(listenSocket, (LPSOCKADDR)&serverAddress, sizeof(serverAddress));
	if (iResult == SOCKET_ERROR)
	{
		cout << "Bad bind.";
		return NULL;
	}

	iResult = listen(listenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR)
	{
		printf("listen failed with error: %d\n", WSAGetLastError());
		cleanup(listenSocket);
		return 1;
	}

	return listenSocket;
}


void receive(bool *halt, CircleBuffer *buffer, request_queue **queue)
{
	InitializeWindowsSockets();
	SOCKET interaction_socket = NULL;
	char accessBuffer[ACCESS_BUFFER_SIZE];
	SOCKET accepted_socket = INVALID_SOCKET;
	timeval timeVal;
	timeVal.tv_sec = 0;
	timeVal.tv_usec = 5;
	FD_SET set;

	interaction_socket = create_and_bind_socket(SERVER_PORT);
	if (interaction_socket == NULL)
	{
		printf("An error has occured during the socket creation process.");
		cleanup(interaction_socket);
		return;
	}

	while (*halt)
	{
		if (*queue != NULL)
		{
			Request* current_client = request_dequeue(queue);

			char* buffer = (char*)malloc(3);

			if (buffer == NULL) return;

			strcpy_s(buffer, 3, "ok");

			int iResult = send(*(current_client->clientAdress), buffer, 3, 0);
			if (iResult == SOCKET_ERROR)
			{
				cout << "ERROR DURING REPORT" << WSAGetLastError();
			}
			closesocket(*(current_client->clientAdress));

			free_request(current_client);

			free(buffer);
		}

		if (accepted_socket == INVALID_SOCKET)
		{
			accepted_socket = accept(interaction_socket, NULL, NULL);
			if (accepted_socket == INVALID_SOCKET)
			{
				Sleep(10);
				continue;
			}
		}
		FD_ZERO(&set);
		FD_SET(accepted_socket, &set);
		memset(accessBuffer, 0, ACCESS_BUFFER_SIZE);


		int iResult = select(0, &set, NULL, NULL, &timeVal);
		if (iResult == SOCKET_ERROR)
		{
			printf("Error during communication.");
			cleanup(interaction_socket);
			return;
		}
		if (iResult == 0)
		{
			cout << "Waiting on port " << SERVER_PORT << endl;
			Sleep(10);
			continue;
		}


		iResult = recv(accepted_socket, accessBuffer, ACCESS_BUFFER_SIZE, 0);

		if (iResult == SOCKET_ERROR)
		{
			printf("recv failed with error: %d\n", WSAGetLastError());
			continue;
		}
		cout << "Request received." << endl;
		if (buffer->full)
		{
			Sleep(10);
		}
		if (buffer->full)
		{
			cout << "Rejected connection due to buffer being full." << endl;
			closesocket(accepted_socket);
			accepted_socket = INVALID_SOCKET;
			continue;
		}
		SOCKET* new_socket = (SOCKET*)malloc(sizeof(SOCKET));
		if (new_socket == NULL)
		{
			cout << "Memory full. Exiting." << endl;
			break;
		}
		memcpy(new_socket, &accepted_socket, sizeof(SOCKET));
		Request* new_request = (Request*)malloc(sizeof(Request));
		if (new_request == NULL)
		{
			cout << "Memory full. Exiting." << endl;
			break;
		}
		new_request->clientAdress = new_socket;
		new_request->message = (char*)malloc(sizeof(accessBuffer));
		if (new_request->message == NULL)
		{
			cout << "Memory full. Exiting." << endl;
			break;
		}
		memcpy(new_request->message, accessBuffer, sizeof(accessBuffer));
		buffer->addElement(new_request);
		accepted_socket = INVALID_SOCKET;
	}

	cleanup(interaction_socket);
	cout << "Receive shut down gracefully." << endl;
	return;
}

