
// LoadBalancer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <ws2tcpip.h>
#include "common.h"
#include <thread>
#include <cstdlib>
#pragma comment(lib,"Ws2_32.lib")
#define SERVER_PORT 5001
#define ACCESS_BUFFER_SIZE 1024
#define IP_ADRESS_LENGTH 16


int main()
{


/*
 STEPS:

 Create needed buffers.
 start background process - recieve

 initiate loop

		
		check if you need to create a worker or if a worker is supposed to be shut down

		check if a worker sent you something
					if a worker sent something, find him in the worker queue and update it so he is free or shut him off 

		check if there are free workers and a request to be processed
			if there are not - dont try to send
			if there are - 	take workers information and send him the request
					set the workers state to busy in worker queue
			check if someone sent you something
					if a worker sent something, find him in the worker queue and update it so he is free

		if exit vriteria is met exit loop

		*/
	CircleBuffer CB;
	bool stop = false;

	int numOfWorkers = 0;

	thread t = std::thread(recieve, CB, stop);
	t.detach();
	


    CreateWorker();
	numOfWorkers++;

	sockaddr_in serverAddress;
	memset((char*)&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET; /*set server address protocol family*/
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(SERVER_PORT);


	int iResult = 0;
	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	long mode = 1;
	iResult = ioctlsocket(listenSocket, FIONBIO, &mode);
	if (iResult == SOCKET_ERROR)
		return NULL;

	iResult = bind(listenSocket, (LPSOCKADDR)&serverAddress, sizeof(serverAddress));
	if (iResult == SOCKET_ERROR)
		return NULL;

	FD_SET set;
	FD_ZERO(&set);
	FD_SET(listenSocket, &set);

	timeval timeVal;
	timeVal.tv_sec = 0;
	timeVal.tv_usec = 5;

	socket_queue* head;

	char accessBuffer[ACCESS_BUFFER_SIZE];

	while (!stop) {

		


		

		bool shutDownFlag = false;
		int num = CircleBufferCount(CB);

		if (num == 1) if (numOfWorkers < 6) { CreateWorker(head); numOfWorkers++; }
		if (num == -1) if (numOfWorkers > 1) {shutDownFlag = true; numOfWorkers--;		}
		
			memset(accessBuffer, 0, ACCESS_BUFFER_SIZE);

			int iResult = select(0, &set, NULL, NULL, &timeVal);
			if (iResult == SOCKET_ERROR)
			{
				printf("Error during communication.");
				return;
			}
			if (iResult > 0)
			{
				//recieve a message from a worker, get his sockaddr_in and update the worker queue
				if (shutDownFlag) //shutDownWorker(head, worker); ShutDownWorker = false;
			}

			if (CB.elementNum != 0) {
				sockaddr_in worker = getWorker(head);
				if (worker == NULL)
				{
					//Sleep(10);
					continue;
				}

				Request r = CB.removeElement();

				sendRequest(r, worker);

			}

		
			//exit criteria is still to be decided

	}



}

void CreateWorker(socket_queue* head) {
	//run a worker
	// result of accept() place in queue

}

int CircleBufferCount(CircleBuffer CB) {
	int num = CB.elementNum();
	if (num >= 7) return 1;
	if (num <= 3) return -1;
	return 0;
}

void ShutDownWorker(socket_queue* head, sockaddr_in worker) {
	//remove a worker from queue
	// send him a message to shut down


	socket_queue* prev = head;
	socket_queue* element = head->next;
	while (element != NULL) {
		if (element->address == worker)
			prev->next = element->next;
		//free(element);

	}
}

sockaddr_in getWorker(socket_queue* head) {
	//get a free worker and set him as busy
	
	socket_queue* element = head;
	while (element != NULL) {
		if (element->state == 1)
			return element->address;
		element = element->next;
		
	}
	return NULL;
}

void sendRequest(Request r, sockaddr_in worker) {
	//send request to worker using TCP

}
/*
int main()
{
	system("start Worker.exe");
	cout << "I'm still working dumbass";
	system("pause");
}*/