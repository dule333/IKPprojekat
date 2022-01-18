#include "common.h"

void report(socket_queue *return_sockets, bool *halt)
{
	while (*halt)
	{
		if (return_sockets != NULL)
		{
			SOCKET current_client = socket_dequeue(return_sockets);

			char* buffer = (char*)malloc(3);

			if (buffer == NULL) return;

			strcpy_s(buffer, 3, "ok");

			send(current_client, buffer, 3, 0);

			free(buffer);
		}
		Sleep(10);
	}
}