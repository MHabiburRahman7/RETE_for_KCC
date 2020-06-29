//
// Created by USER on 12/4/2018.
//
//#include "../stdafx.h"
#include "Event.h"

Event::Event(long id, long long time) {
	stringstream ss;
	ss << id;
	addAttr("id", ss.str());
	stringstream stream;
	stream << time;
	string timeStr;
	stream >> timeStr;
	addAttr("time", timeStr);
}

int Event::getInt(string attrName) {
	if (attrMap.find(attrName) != attrMap.end()) {
		stringstream stream;
		stream << attrMap[attrName];
		int int_temp = 0;
		stream >> int_temp;
		return int_temp;
	}
	else {
		std::cout << "no this element, attrName is " << attrName;
		throw "";
	}
}

float Event::getFloat(string attrName) {
	if (attrMap.find(attrName) != attrMap.end()) {
		stringstream stream(attrMap[attrName]);
		float int_temp = 0;
		stream >> int_temp;
		return int_temp;
	}
	else {
		std::cout << "no this element, attrName is " << attrName;
		throw "";
	}
}

string Event::getString(string attrName) {
	if (attrMap.find(attrName) != attrMap.end()) {
		return attrMap[attrName];
	}
	else {
		std::cout << "no this element, attrName is " << attrName;
		throw "no this element";
	}
}

void Event::addAttr(string attrName, int value) {
	/*if (attrMap.find(attrName) != attrMap.end()) {
		std::cout << "already exist the name";
		throw runtime_error("");
	}*/
	attrMap[attrName] = to_string(value);
}

void Event::addAttr(string attrName, float value) {
	/*if (attrMap.find(attrName) != attrMap.end()) {
		std::cout << "already exist the name";
		throw "already exist the name";
	}*/
	attrMap[attrName] = to_string(value);
}

void Event::addAttr(string attrName, string value) {
	/*if (attrMap.find(attrName) != attrMap.end()) {
		std::cout << "already exist the name";
		throw "already exist the name";
	}*/
	attrMap[attrName] = value;
}

long long Event::getTime() {
	string timeStr = getString("time");
	stringstream s(timeStr);
	long long result = 0;
	s >> result;
	return result;
};

void Event::print(ostream& out) {
	out << toString();
}

string Event::toString() {
	stringstream msg;
	msg << "id:";
	msg << getId() << ",time:";
	msg << getTime();


	for (auto iter = attrMap.begin(); iter != attrMap.end(); iter++) {
		string str = iter->first;
		bool exist = false;
		if (str == "id" || str == "time")
			exist = true;

		if (!exist) {
			msg << "," << iter->first << ":" << iter->second;
		}
	}
	return msg.str();
}

int Event::map_size()
{
	return attrMap.size();
}

void Event::setDestination(string _destination) {
	destination = _destination;
}

string Event::getDestination() {
	return this->destination;
}


Event* Event::extend(const map<string, string>& extendedEntry) {
	Event* he = new Event(getId(), getTime());

	for (auto iter = extendedEntry.begin(); iter != extendedEntry.end(); iter++) {
		he->attrMap[iter->first] = iter->second;
	}
	for (auto iter = attrMap.begin(); iter != attrMap.end(); iter++) {
		he->attrMap[iter->first] = iter->second;
	}

	return he;
}

Event* Event::clone() {
	map<string, string> m;
	return extend(m);
}


ostream& operator<< (ostream& out, Event& e) {
	e.print(out);
	return out;
}