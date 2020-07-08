#include "SlidingWindow.h"

SlidingWindow::SlidingWindow(int len, int step)
{
	winLen = len;
	winStep = step;//think later
}

void SlidingWindow::refresh(queue<EventPtr>& eventRes)
{
	int latest_time = eventRes.back()->getInt("time"); // time n until n - len
	
	//FOR GREATER GOOD :"
	if (eventRes.front()->map_size() > 999999 || eventRes.front()->map_size() < -1) {
		eventRes.pop();
	}
	int a = 11;
	while (!eventRes.empty() && eventRes.front()->getInt("time") < latest_time - winLen) {
		eventRes.pop();
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
	/*
		Event* ec = e->clone();

	if (win_buffer.size() > 0) {
	//	std::sort(win_buffer->front(), win_buffer->back());
		//duplicate check
		int duplicate = duplicateCheck(EventPtr(ec), win_buffer);

		if(duplicate == 0)
			win_buffer.push(EventPtr(ec));
	}
	else
		win_buffer.push(EventPtr(ec));
	*/

	if (win_buffer.size() > 0) {
	//	std::sort(win_buffer->front(), win_buffer->back());
		//duplicate check
		int duplicate = duplicateCheck(e, win_buffer);
		
		if(duplicate == 0)
			win_buffer.push(e);
	}
	else
		win_buffer.push(e);
	//
	refresh(win_buffer);
}

void SlidingWindow::addResultEvent(EventPtr e)
{
	Event *ev = e->clone();
	//EventPtr ep;// = Event(ev);
	//EventPtr(ev);
	
	if (win_res_buffer.size() > 0) {
		//	std::sort(win_buffer->front(), win_buffer->back());
		
		//duplicate check
		Event* backup_event = e->clone();
		int duplicate = duplicateCheck(EventPtr(ev), win_res_buffer);

		if (duplicate == 0)
			win_res_buffer.push(EventPtr(backup_event));


		//duplicate check
		//int duplicate = duplicateCheck(e, win_res_buffer);

		//if (duplicate == 0)
		//	win_res_buffer.push(e);
	}
	else
		win_res_buffer.push(EventPtr(ev));

	refresh(win_res_buffer);
}

void SlidingWindow::addEvent(EventPtr a, EventPtr b)
{
	double_win_buffer.push({ a, b });
	refreshDouble();
}

int SlidingWindow::duplicateCheck(EventPtr a, queue<EventPtr> eventRes)
{
	queue<EventPtr> tempQueue = eventRes;
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

queue<EventPtr> SlidingWindow::getFinalRes()
{
	//refresh(win_res_buffer);

	//for(;win_res_buffer.size() > 0; )

	return win_res_buffer;
}

queue<EventPtr> SlidingWindow::getOriginalRes()
{
	refresh(win_buffer);
	return win_buffer;
}

float SlidingWindow::getLowestOriginalTime()
{
	if (win_buffer.size() > 0) {
		return win_buffer.front()->getFloat("time");
	}
	return -1;
}

float SlidingWindow::getHigheststOriginalTime()
{
	if (win_buffer.size() > 0) {
		return win_buffer.back()->getFloat("time");
	}
	return -1;
}

float SlidingWindow::getInitTime()
{
	return initTime;
}

float SlidingWindow::setInitTime(float newTime)
{
	initTime = newTime;
	return 1;
}

int SlidingWindow::getTriggerTime()
{
	return winStep;
}

queue<pair<EventPtr, EventPtr>> SlidingWindow::getDoubleRes()
{
	refreshDouble();
	return double_win_buffer;
}
