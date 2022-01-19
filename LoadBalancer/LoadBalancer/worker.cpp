/*#include "common.h"

void execute(Request r) {
	this_thread::sleep_for(std::chrono::seconds(1));
	printf("Request executed succesfully. Message: %s \n", r.message);

}

void work(request_queue* shead, Request *input, bool* halt) {
	printf("Worker starting... \n");

	while (!(*halt))
	{
		if (input == NULL)
		{
			Sleep(10);
			continue;
		}
		execute(*input);
		input = NULL;
		//socket_enqueue(shead, input->clientAdress);
	}
	//socket_enqueue(shead, r.clientAdress); Treba prepraviti r da kao client address ima socket a ne sockaddr_in?
}*/