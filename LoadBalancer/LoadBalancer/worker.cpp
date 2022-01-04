#include "common.h"
#include <thread>
#include <iostream>

using namespace std;

void execute(Request r) {
	this_thread::sleep_for(std::chrono::seconds(1));
	printf("Request executed succesfully. Message: %s \n", r.message);

}

void work(Request r, thread* worker, worker_queue* head, socket_queue* shead) {
	printf("Worker starting... \n");
	thread t(execute, r);

	t.join();

	worker = &t;

	worker_enqueue(head, worker);

	//socket_enqueue(shead, r.clientAdress); Treba prepraviti r da kao client address ima socket a ne sockaddr_in

}