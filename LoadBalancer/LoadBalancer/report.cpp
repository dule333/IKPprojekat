#include "common.h"

void report(socket_queue *return_sockets)
{
	while (1)
	{
		if (return_sockets != NULL)
		{
			SOCKET* current_socket = socket_dequeue(return_sockets);

			char* buffer = (char*)malloc(3);

			if (buffer == NULL) return;

			strcpy_s(buffer, 3, "ok");

			send(*current_socket, buffer, 2, 0);

			free(buffer);
		}
		Sleep(10);
	}
}