#include "common.h"

void report(request_queue **return_sockets, bool *halt)
{
	while (*halt)
	{
		if (*return_sockets != NULL)
		{
			Request* current_client = request_dequeue(return_sockets);

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
		Sleep(10);
	}
	cout << "Report shut down gracefully." << endl;
}