#include "SlidingWindow.h"

SlidingWindow::SlidingWindow(int len, int step)
{
	winLen = len;
	winStep = step;//think later
}

void SlidingWindow::refresh()
{
	int latest_time = win_buffer->back()->getInt("time"); // time n untul n - len
	while (!win_buffer->empty() && win_buffer->front()->getInt("time") < latest_time - winLen) {
		win_buffer->pop();
	}

	/*long long curr = Utilities::getTime();
	while (win_buffer.empty() && win_buffer.front()->getTime() + winLen < curr) {
		win_buffer.pop();
	}*/
}

void SlidingWindow::refreshDouble()
{
	long long curr = Utilities::getTime();
	while (double_win_buffer.empty() && double_win_buffer.front().first->getTime() + winLen < curr) {
		double_win_buffer.pop();
	}
}

void SlidingWindow::addEvent(EventPtr e)
{
	if (win_buffer->size() > 0) {
	//	std::sort(win_buffer->front(), win_buffer->back());
		//duplicate check
		int duplicate = duplicateCheck(e);
		
		if(duplicate == 0)
			win_buffer->push(e);
	}
	else
		win_buffer->push(e);
	//
	refresh();
}

void SlidingWindow::addEvent(EventPtr a, EventPtr b)
{
	double_win_buffer.push({ a, b });
	refreshDouble();
}

int SlidingWindow::duplicateCheck(EventPtr a)
{
	queue<EventPtr> tempQueue = *win_buffer;
	while (tempQueue.size() > 0) {
		if (a == tempQueue.front())
			return 1;
		tempQueue.pop();
	}
	return 0;
}

int SlidingWindow::duplicateCheck(EventPtr a, EventPtr b)
{
	queue<pair<EventPtr, EventPtr>> tempQueue = double_win_buffer;
	//while (tempQueue.size > 0) {
	//	if (a == tempQueue.front())
	//		return 0;
	//	tempQueue.pop();
	//}
	return 1;
}

queue<EventPtr> *SlidingWindow::getFinalRes()
{
	refresh();
	return win_buffer;
}

queue<pair<EventPtr, EventPtr>> SlidingWindow::getDoubleRes()
{
	refreshDouble();
	return double_win_buffer;
}
