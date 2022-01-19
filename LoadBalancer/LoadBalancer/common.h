#pragma once
#include <WS2tcpip.h>
#include <cstdlib>
#include <iostream>
#include <winsock2.h>
#include <thread>

using namespace std;

struct worker_queue
{
	worker_queue* next;
	SOCKET* value;
};


struct Request {
	SOCKET* clientAdress;
	char* message;
};

struct request_queue
{
	request_queue* next;
	Request* value;
};

struct CircleBuffer {
	Request* BUFFER[10];
	int pushId = 0;
	int popId = 0;
	bool full = false;
	bool addElement(Request *element) {
		if (!full) {
			BUFFER[pushId] = element;
			pushId++;
			if (pushId == 10) pushId = 0;
			if (pushId == popId) full = true;

			return true;
		}
		return false;
	}
	Request* removeElement() {
		if (pushId != popId) {
			Request* ret = BUFFER[popId];
			popId++;
			if (popId == 10) popId = 0;
			if (full) full = false;
			return ret;
		}
		return NULL;
	}

	int elementNum() {
		if (pushId < popId)
			return pushId + 10 - popId;
		return pushId - popId;
	}
};

request_queue* request_enqueue(request_queue*, Request*);
Request* request_dequeue(request_queue**);

worker_queue* worker_enqueue(worker_queue*, SOCKET*);
SOCKET* worker_dequeue(worker_queue**);

void receive(bool*, CircleBuffer*, request_queue**);

void free_request(Request*);

void load_balancer(CircleBuffer*, request_queue**, bool*);

bool InitializeWindowsSockets();

void cleanup(SOCKET);

SOCKET create_and_bind_socket(int);
