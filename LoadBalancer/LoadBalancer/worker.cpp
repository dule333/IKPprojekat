#include "common.h"
#include <thread>
#include <iostream>

using namespace std;

void execute(Request r) {
	std::this_thread::sleep_for(std::chrono::seconds(1));
	printf("Request executed succesfully. Message: %s \n", r.message);

}

void work(Request r, thread* worker, worker_queue* head, socket_queue* shead) {
	printf("Worker %d starting... \n", worker->get_id());
	std::thread t(execute, r);

	t.join();

	worker = &(std::thread t());

	worker_enqueue(head, worker);

	socket_enqueue(shead, &r);

}