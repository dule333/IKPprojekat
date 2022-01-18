#include "common.h"

int main()
{
	//Initialization logic
	bool halt = true;
	CircleBuffer buffer = CircleBuffer();
	socket_queue* queue = NULL;

	thread receiver = thread(receive, &halt, &buffer);
	receiver.detach();
	thread reporter = thread(report, queue, &halt);
	reporter.detach();
	//Shutdown logic

	while (getchar() != 'q')
	{

	}

	halt = false;
	Sleep(1000);
	return 0;
}