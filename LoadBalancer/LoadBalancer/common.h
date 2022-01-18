#pragma once
#include <WS2tcpip.h>
#include <iostream>
#include <winsock2.h>
#include <thread>

using namespace std;
/*
struct worker_queue
{
	worker_queue* next;
	thread* value;
};
*/
struct socket_queue
{
	socket_queue* next;
	SOCKET value;
};



struct Request {
	Request(SOCKET ca, char* mesage) {
		clientAdress = ca;
		message = mesage;
	}
	Request() {
		SOCKET help;
		memset(&help, 0, sizeof(SOCKET));
		clientAdress = help;
		message = (char *)"EMPTY_MESSAGE\0";
	}
	SOCKET clientAdress;
	char* message;
};



struct CircleBuffer {
	CircleBuffer() {
		pushId = 0;
		popId = 0;
		full = false;
		//BUFFER = (Request *)malloc(sizeof(Request) * 20);
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

void socket_enqueue(socket_queue*, SOCKET*);
SOCKET socket_dequeue(socket_queue*);


void execute(Request r);
//void work(Request r, thread* worker, socket_queue* shead);

void recieve(CircleBuffer cb, bool* stop);

void receive(bool*, CircleBuffer*);


