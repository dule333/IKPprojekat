#include "common.h"

#pragma region worker_queue

worker_queue* worker_enqueue(worker_queue* tail, SOCKET* worker)
{
	worker_queue* new_worker = (worker_queue*)malloc(sizeof(worker_queue));
	if (new_worker == NULL)
		return NULL;
	if(tail != NULL)
		tail->next = new_worker;
	new_worker->next = NULL;
	new_worker->value = worker;
	return new_worker;
}


SOCKET* worker_dequeue(worker_queue** head)
{
	worker_queue* temp = (*head)->next;
	SOCKET* temp_thread = (*head)->value;
	free(*head);
	*head = temp;
	return temp_thread;
}


#pragma endregion

#pragma region request_queue

request_queue* request_enqueue(request_queue* tail, Request* clientAddress)
{
	request_queue* new_socket = (request_queue*)malloc(sizeof(request_queue));
	if (new_socket == NULL)
		return NULL;
	if(tail != NULL)
		tail->next = new_socket;
	new_socket->next = NULL;
	new_socket->value = clientAddress;
	return new_socket;
}

Request* request_dequeue(request_queue** head)
{
	request_queue* temp = (*head)->next;
	Request* temp_socket = (*head)->value;
	free(*head);
	*head = temp;
	return temp_socket;
}

#pragma endregion

#pragma region request_list


void requests_add(request_queue** list, Request* value)
{
	if (*list == NULL)
	{
		*list = (request_queue*)malloc(sizeof(request_queue));
		if (*list == NULL)
			return;
		(*list)->value = value;
		(*list)->next = NULL;
		return;
	}
	request_queue* temp = (request_queue*)malloc(sizeof(request_queue)); 
	if (temp == NULL)
		return;
	temp->next = *list;
	temp->value = value;
	*list = temp;
}
Request* requests_remove(request_queue** list)
{
	Request* result = (*list)->value;
	request_queue* next = (*list)->next;
	free(*list);
	*list = next;
	return result;
}

#pragma endregion

#pragma region request_methods

void free_request(Request* request)
{
	free(request->clientAdress);
	free(request->message);
	free(request);
}

#pragma endregion