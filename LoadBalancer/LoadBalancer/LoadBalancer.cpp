#include "common.h"
#pragma comment(lib,"Ws2_32.lib")
#define SERVER_PORT 5002
#define ACCESS_BUFFER_SIZE 1024
#define IP_ADRESS_LENGTH 16
#define MAX_WORKERS 12

SOCKET* create_worker(SOCKET socket)
{
	system("start Worker.exe");
	SOCKET accepted_socket = INVALID_SOCKET;
	while(accepted_socket == INVALID_SOCKET)
	{
		accepted_socket = accept(socket, NULL, NULL);
		Sleep(10);
	}
	SOCKET* socket_gotten = (SOCKET*)malloc(sizeof(SOCKET));
	if (!socket_gotten)
		return NULL;
	memcpy(socket_gotten, &accepted_socket, sizeof(SOCKET));
	return socket_gotten;
}

void close_worker(SOCKET* worker)
{
	closesocket(*worker);
	free(worker);
}

int CircleBufferCount(CircleBuffer* CB)
{
	int num = CB->elementNum();
	if (num >= 7) return 1;
	if (num <= 3) return -1;
	//if (num >= MAX_SIZE * 0.7) return 1;
	//if (num <= MAX_SIZE * 0.3) return -1;
	return 0;
}

void load_balancer(CircleBuffer* buffer, request_queue** requests, bool* halt)
{
	worker_queue* head_free = NULL, * head_busy = NULL, * tail_free = NULL, * tail_busy = NULL;

	int numOfWorkers = 0;
	request_queue *queue_head = NULL, *queue_tail = NULL, *requests_head = NULL;
	bool kill = false;
	InitializeWindowsSockets();
	SOCKET accepted_socket = NULL;
	SOCKET interaction_socket = NULL;
	char accessBuffer[ACCESS_BUFFER_SIZE];
	char clearBuffer[3];
	strcpy_s(clearBuffer, 3, "-1");
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

	tail_free = worker_enqueue(tail_free, create_worker(interaction_socket));
	if (!tail_free)
		return;
	if (head_free == NULL)
	{
		head_free = tail_free;
	}
	numOfWorkers++;
	Request* temp_request = NULL;
	while (*halt)
	{
		//Check if work and free workers available and if so give work
		if (buffer->elementNum() && head_free)
		{
			FD_ZERO(&set);
			FD_SET(*(head_free->value), &set);
			if (temp_request == NULL)
			{
				temp_request = buffer->removeElement();
			}
			if (temp_request == NULL)
				cout << "Buffer failed." << endl;

			int iResult = select(0, NULL, &set, NULL, &timeVal);
			if (iResult == SOCKET_ERROR)
			{
				printf("Error during communication.");
				cleanup(interaction_socket);
				return;
			}
			if (iResult > 0)
			{
				strcpy_s(accessBuffer, 3, "ok");
				iResult = send(*(head_free->value), accessBuffer, 3, 0);

				if (iResult == SOCKET_ERROR)
				{
					printf("send failed with error: %d\n", WSAGetLastError());
					continue;
				}
			}
			cout << "Sending task." << *(temp_request->clientAdress) << endl;
			queue_tail = request_enqueue(queue_tail, temp_request);
			if (queue_head == NULL)
				queue_head = queue_tail;
			if (head_free->next == NULL)
				tail_free = NULL;
			tail_busy = worker_enqueue(tail_busy, worker_dequeue(&head_free));
			if (!head_busy)
			{
				head_busy = tail_busy;
			}
			temp_request = NULL;

		}

		//Check if workers need to change
		switch (CircleBufferCount(buffer))
		{
		case 1:
			if (numOfWorkers < MAX_WORKERS)
			{
				cout << "Making worker." << endl;
				tail_free = worker_enqueue(tail_free, create_worker(interaction_socket));
				if (head_free == NULL)
				{
					head_free = tail_free;
				}
				numOfWorkers++;
			}
			break;
		case -1:
	/*		if (numOfWorkers > 1 && head_free)
			{
				cout << "Killing worker." << endl;
				FD_ZERO(&set);
				FD_SET(*(head_free->value), &set);

				int iResult = select(0, NULL, &set, NULL, &timeVal);
				if (iResult == SOCKET_ERROR)
				{
					cout << "Could not select :(" << endl;
					printf("Error during communication.");
					cleanup(interaction_socket);
					return;
				}
				if (iResult > 0)
				{
					iResult = send(*(head_free->value), clearBuffer, 3, 0);

					if (iResult == SOCKET_ERROR)
					{
						printf("recv failed with error: %d\n", WSAGetLastError());
						continue;
					}
				}

				close_worker(head_free->value);
				worker_queue* temp = head_free;
				head_free = head_free->next;
				if (head_free == NULL)
					tail_free = NULL;
				free(temp);
				
			}*/
			if (numOfWorkers > 1)
				kill = true;
			break;
		default:
			kill = false;
			break;
		}

		//Check if worker has finished
		if (head_busy)
		{
			FD_ZERO(&set);
			FD_SET(*(head_busy->value), &set);
			memset(accessBuffer, 0, ACCESS_BUFFER_SIZE);

			int iResult = select(0, &set, NULL, NULL, &timeVal);
			if (iResult == SOCKET_ERROR)
			{
				printf("Error during communication.");
				cleanup(interaction_socket);
				return;
			}
			if (iResult > 0)
			{
				iResult = recv(*(head_busy->value), accessBuffer, 3, 0);
				if (iResult == SOCKET_ERROR)
				{
					printf("recv failed with error: %d\n", WSAGetLastError());
					continue;
				}

				if (head_busy->next == NULL)
					tail_busy = NULL;

				if (kill)
				{
					if (head_busy->next == NULL)
						tail_busy = NULL;
					SOCKET* temp_worker = worker_dequeue(&head_busy);
					cout << "Killing worker." << endl;
					FD_ZERO(&set);
					FD_SET(*(temp_worker), &set);

					int iResult = select(0, NULL, &set, NULL, &timeVal);
					if (iResult == SOCKET_ERROR)
					{
						cout << "Could not select :(" << endl;
						printf("Error during communication.");
						cleanup(interaction_socket);
						return;
					}
					if (iResult > 0)
					{
						iResult = send(*(temp_worker), clearBuffer, 3, 0);

						if (iResult == SOCKET_ERROR)
						{
							printf("recv failed with error: %d\n", WSAGetLastError());
							continue;
						}

						close_worker(temp_worker);
						kill = false;
						numOfWorkers--;
					}
				}
				else
				{
					if (head_busy->next == NULL)
						tail_busy = NULL;
					tail_free = worker_enqueue(tail_free, worker_dequeue(&head_busy));
					if (head_free == NULL)
						head_free = tail_free;
				}
				if (queue_head == NULL)
				{
					cout << "Queue empty somehow." << endl;
					break;
				}
				if (queue_head->next == NULL)
					queue_tail = NULL;
				temp_request = request_dequeue(&queue_head);
				cout << "Receiving task." << endl;
				requests_add(requests, temp_request);
				temp_request = NULL;
			}
		}
	}
	cleanup(interaction_socket);
	cout << "Dispatcher closed successfully." << endl;
}
