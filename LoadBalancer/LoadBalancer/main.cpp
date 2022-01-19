#include "common.h"

int main()
{
	//Initialization logic
	bool halt = true;
	CircleBuffer *buffer = (CircleBuffer*)malloc(sizeof(CircleBuffer));
	if (buffer == NULL)
	{
		cout << "Memory full. Exiting." << endl;
		halt = false;
		Sleep(1000);
		return 1;
	}
	buffer->popId = 0;
	buffer->pushId = 0;
	buffer->full = false;
	request_queue* queue = NULL;

	thread receiver = thread(receive, &halt, buffer, &queue);
	receiver.detach();
	Sleep(1000);
	thread balancer = thread(load_balancer, buffer, &queue, &halt);
	balancer.detach();
	//Shutdown logic

	while (getchar() != 'q')
	{

	}

	halt = false;
	Sleep(1000);
	free(buffer);
	return 0;
}