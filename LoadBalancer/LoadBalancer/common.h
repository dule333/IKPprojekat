#pragma once
#include <WS2tcpip.h>
#include <iostream>
#include <winsock2.h>
#include <thread>

using namespace std;

struct worker_queue
{
	worker_queue* previous;
	thread* value;
};

struct socket_queue
{
	socket_queue* previous;
	SOCKET* value;
};

void worker_enqueue(worker_queue*, thread*);
thread* worker_dequeue(worker_queue*);

void socket_enqueue(socket_queue*, SOCKET*);
SOCKET* socket_dequeue(socket_queue*);