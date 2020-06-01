#include "WorkingMemory.h"

void WorkingMemory::clearWM()
{
	while (inputQueues.size() > 0) {
		inputQueues.pop();
	}
}

void WorkingMemory::reduceMemory(int numberFromFront)
{
	int itt = 0;
	for (int i = 0; i < numberFromFront && inputQueues.size() > 0; i++) {
		inputQueues.pop();
	}
}

queue<EventPtr> WorkingMemory::getWMInputQueue()
{
	return inputQueues;
}

void WorkingMemory::cloneInputQueues(queue<EventPtr> clon)
{
	inputQueues = clon;
}
