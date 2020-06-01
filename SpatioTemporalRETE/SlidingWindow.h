#pragma once
#include "Event.h"
#include <queue>
#include "Utilities.h"

class SlidingWindow
{
public:
	SlidingWindow(int len, int step = -1);
	void refresh();
	void refreshDouble();
	void addEvent(EventPtr e);
	void addEvent(EventPtr a, EventPtr b);

	int duplicateCheck(EventPtr a);
	int duplicateCheck(EventPtr a, EventPtr b);

	queue<EventPtr> *getFinalRes();
	queue<pair<EventPtr, EventPtr>> getDoubleRes();

private:
	queue<EventPtr> *win_buffer = new queue<EventPtr>();
	queue<pair<EventPtr, EventPtr>> double_win_buffer;

	int winLen = -1, winStep = -1;
};

