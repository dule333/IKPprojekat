#pragma once
#include <WS2tcpip.h>
#include <iostream>
#include <winsock2.h>
#include <thread>

using namespace std;

struct worker_queue
{
	worker_queue* next;
	thread* value;
};

struct socket_queue
{
	socket_queue* next;
	SOCKET* value;
};



struct Request {
	Request(sockaddr_in ca, char* mesage) {
		clientAdress = ca;
		message = mesage;
	}
	Request() {
		sockaddr_in help;
		memset(&help, 0, sizeof(sockaddr_in));
		clientAdress = help;
		message = (char *)"EMPTY_MESSAGE\0";
	}
	sockaddr_in clientAdress;
	char* message;
};



struct CircleBuffer {
	CircleBuffer() {
		pushId = 0;
		popId = 0;
		full = false;
	}
	Request BUFFER[10];
	int pushId = 0;
	int popId = 0;
	bool full = false;
	bool addElement(Request element) {

		if (pushId != popId || !full) {
			BUFFER[pushId] = element;
			pushId++;
			if (pushId == 10) pushId = 0;
			if (pushId == popId) full = true;

			return true;
		}
		return false;
	}
	Request removeElement() {

		if (pushId != popId || !full) {

			Request ret = BUFFER[popId];
			popId++;
			if (popId == 10) popId = 0;
			if (full) full = false;
			return ret;
		}
		Request ret;
		return ret;


	}

	int elementNum() {
		if (pushId < popId)
			return pushId + 10 - popId;
		return pushId - popId;
	}

};




void worker_enqueue(worker_queue*, thread*);
thread* worker_dequeue(worker_queue*);

void socket_enqueue(socket_queue*, SOCKET*);
SOCKET* socket_dequeue(socket_queue*);


void execute(Request r);
void work(Request r, thread* worker, worker_queue* head, socket_queue* shead);




