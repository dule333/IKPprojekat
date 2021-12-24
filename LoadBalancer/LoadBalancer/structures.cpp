#include "common.h"

using namespace std;

#pragma region worker_queue

void worker_enqueue(worker_queue* head, thread* worker)
{
	worker_queue* new_worker = (worker_queue*)malloc(sizeof(worker_queue));
	if (new_worker == NULL)
		return;
	new_worker->previous = head;
	new_worker->value = worker;
	head = new_worker;
}


thread* worker_dequeue(worker_queue* head)
{
	worker_queue* temp = head->previous;
	thread* temp_thread = head->value;
	free(head);
	head = temp;
	return temp_thread;
}


#pragma endregion

#pragma region socket_queue

void socket_enqueue(socket_queue* head, SOCKET* socket)
{
	socket_queue* new_socket = (socket_queue*)malloc(sizeof(socket_queue));
	if (new_socket == NULL)
		return;
	new_socket->previous = head;
	new_socket->value = socket;
	head = new_socket;
}

SOCKET* socket_dequeue(socket_queue* head)
{
	socket_queue* temp = head->previous;
	SOCKET* temp_socket = head->value;
	free(head);
	head = temp;
	return temp_socket;
}

#pragma endregion
