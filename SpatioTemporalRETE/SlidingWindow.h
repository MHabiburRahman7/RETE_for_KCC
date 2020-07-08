#pragma once
#include "Event.h"
#include <queue>
#include "Utilities.h"
#include <assert.h>

class SlidingWindow
{
public:
	SlidingWindow(int len, int step = -1);
	void refresh(queue<EventPtr>& eventRes);
	void refreshDouble();
	void addEvent(EventPtr e);
	void addResultEvent(EventPtr e);
	void addEvent(EventPtr a, EventPtr b);

	int duplicateCheck(EventPtr a, queue<EventPtr> eventRes);
	int duplicateCheck(EventPtr a, EventPtr b);

	queue<EventPtr> getFinalRes();
	queue<EventPtr> getOriginalRes();
	float getLowestOriginalTime();
	float getHigheststOriginalTime();
	float getInitTime();
	float setInitTime(float newTime);
	int getTriggerTime();
	queue<pair<EventPtr, EventPtr>> getDoubleRes();

private:
	queue<EventPtr> win_buffer;
	queue<EventPtr> win_res_buffer;
	queue<pair<EventPtr, EventPtr>> double_win_buffer;
	float initTime = 0;

	int winLen = -1, winStep = -1;
};

