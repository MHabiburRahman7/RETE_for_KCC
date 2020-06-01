//
// Created by USER on 12/4/2018.
//

#ifndef CONTINUOUSPROCESSING_HASHEVENT_H
#define CONTINUOUSPROCESSING_HASHEVENT_H

#include <iostream>
#include <unordered_map>
#include <set>
#include <map>
#include <sstream>
#include <memory>
#include "Event.h"
#include <string>

using namespace std;

/**
 * A simple attribute
 */
 //typedef struct EventAttribute {
 //	string name;
 //	ValType type;
 //	union {
 //		int intVal;
 //		float floatVal;
 //		bool boolVal;
 //		string stringVal;
 //	};
 //} Attribute;

class Event {
private:
    unordered_map<string, string> attrMap;

    friend std::ostream& operator<< (std::ostream& os, Event& e);
public:
    Event(long id, long long time);
    long getId() { return getInt("id"); }
    long long getTime();
    int getInt(string attrName);
    float getFloat(string attrName);
    string getString(string attrName);
    void print(ostream& out);
    string toString();

    //it returns: current attributes + extension attributes
    Event* extend(const map<string, string>& extendedEntry);
    Event* clone();

    void addAttr(string attrName, int value);
    void addAttr(string attrName, float value);
    void addAttr(string attrName, string value);
    void setDestination(string _destination);
    string getDestination();

protected:
    string destination = "";//the destination the event will be passed to.
};

typedef shared_ptr<Event> EventPtr;

#endif //CONTINUOUSPROCESSING_HASHEVENT_H
