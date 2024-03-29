#pragma once
#include <WS2tcpip.h>
#include <cstdlib>
#include <iostream>
#include <winsock2.h>
#include <thread>
#define MAX_SIZE 100
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
	Request* BUFFER[MAX_SIZE];
	int pushId = 0;
	int popId = 0;
	bool full = false;
	bool addElement(Request *element) {
		if (!full) {
			BUFFER[pushId] = element;
			pushId++;
			if (pushId > MAX_SIZE-1) pushId = 0;
			if (pushId == popId) full = true;

			return true;
		}
		return false;
	}
	Request* removeElement() {
		if ((pushId != popId) || full) {
			Request* ret = BUFFER[popId];
			popId++;
			if (popId > MAX_SIZE-1) popId = 0;
			if (full) full = false;
			return ret;
		}
		return NULL;
	}

	int elementNum() {
		if (pushId < popId)
			return pushId + MAX_SIZE - popId;
		if (full)
			return MAX_SIZE;
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

void requests_add(request_queue**, Request*);
Request* requests_remove(request_queue**);