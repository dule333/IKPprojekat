#include "common.h"

#pragma region worker_queue

void worker_enqueue(worker_queue* tail, thread* worker)
{
	worker_queue* new_worker = (worker_queue*)malloc(sizeof(worker_queue));
	if (new_worker == NULL)
		return;
	new_worker->next = tail;
	new_worker->value = worker;
	tail = new_worker;
}


thread* worker_dequeue(worker_queue* head)
{
	worker_queue* temp = head;
	thread* temp_thread = head->value;
	free(head);
	head = temp;
	return temp_thread;
}


#pragma endregion

#pragma region socket_queue

void socket_enqueue(socket_queue* tail, sockaddr_in clientAddress)
{
	socket_queue* new_socket = (socket_queue*)malloc(sizeof(socket_queue));
	if (new_socket == NULL)
		return;
	new_socket->next = tail;
	new_socket->value = clientAddress;
	tail = new_socket;
}

sockaddr_in socket_dequeue(socket_queue* head)
{
	socket_queue* temp = head->next;
	sockaddr_in temp_socket = head->value;
	free(head);
	head = temp;
	return temp_socket;
}

#pragma endregion