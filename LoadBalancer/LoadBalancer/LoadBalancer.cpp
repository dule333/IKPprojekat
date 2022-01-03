// LoadBalancer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <WS2tcpip.h>
#include <iostream>
#include "common.h"
#include <winsock2.h>
#include<thread>

#pragma comment(lib,"Ws2_32.lib")


int main()
{





	
	/*
	sockaddr_in e;
	memset(&e, 0, sizeof(sockaddr_in));

	e.sin_port = SERVER_PORT;
	e.sin_family = AF_INET;
	e.sin_addr.S_un.S_addr = INADDR_ANY;

	char* message = (char *)"MESSAGE 1";
	char* message2 = (char *)"MESSAGE 2";
	char* message3 = (char *)"MESSAGE 3";

	Request r1 = Request(e, message);
	Request r2 = Request(e, message2);
	Request r3 = Request(e, message3);

	CB.addElement(r1);
	CB.addElement(r1);
	CB.addElement(r1);
	CB.addElement(r1);
	CB.addElement(r1);
	CB.addElement(r1);
	CB.addElement(r1);
	CB.addElement(r1);
	CB.addElement(r2);
	CB.addElement(r3);

	for (int i = 0; i < 10; i++) {
		Request ret = CB.removeElement();
		std::cout << ret.message;
	}


	*/
}

