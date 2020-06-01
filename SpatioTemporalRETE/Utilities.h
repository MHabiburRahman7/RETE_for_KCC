#pragma once
#include <iostream>
#include <vector>

#include <time.h>
#include <string>
#include <sstream>
#include <vector>
#include <chrono>
#include <cstring>
#include <ctime>

#include <mutex>

#include "Node.h"

using namespace std;

class Utilities
{
public:

	Utilities();

	//system wise id
	static unsigned long id;

	//tokenizing
	static vector<pair<string, string>> tokenize(string temp);
	static vector<pair<string, string>> tokenizeMoreDetail(string temp);

	static long long getTime();

	static string ToUpper(string ori);
	static string ToLower(string ori);

	static string trim(string& str);
	static string lTrim(string& str);
	static string rTrim(string& str);

	static void deleteAllMark(string& s, const string& delim);
	static vector<string> splitDelim(string s, const string delim);

	static double randomFloat(int min, int max);

	//About token and tokens
	static bool tokenizeSingleExp(const string expression, string& stream, string& fieldName, string& mid, string& right, string& sepcialOp);
	static bool tokenizeBetaExp(const string expression, string& left, string& right, string& specialOperation,
		string& limit, string& condition, string& product);
	static string getComparisonExp(const string expression);
};

