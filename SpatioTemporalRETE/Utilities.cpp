#include "Utilities.h"

unsigned long Utilities::id = 0;
unsigned long Utilities::time = 0;

Utilities::Utilities()
{
}

vector<pair<string, string>> Utilities::tokenize(string temp)
{
	//example
	//If distance < 10000 and elevation > 12000 then danger_obj
	//will return:
	//1. pair<"alpha" , "distance < 10000">
	//2. pair<"alpha" , "elevation > 12000">
	//3. pair<"term" , "danger_obj">

	//if danger_obj and speed > 20000 then alert_obj
	//will return:
	//1. pair<"beta" , "danger_obj">
	//2. pair<"alpha" , "speed > 20000">>
	//3. pair<"term" , "alert_obj">

	vector<string> tempor;
	vector<pair<string, string>> conditionMade;

	tempor = {};
	conditionMade = {};

	char* str = (char*)malloc(500);
	strcpy(str, temp.c_str());
	// Returns first token  
	char* token = strtok(str, " ");
	int itt = 0;
	int ittMade = 0;

	if (itt == 0 && strcmp(ToUpper(token).c_str(), "IF") == 0) {
		itt++;
	}
	else if (itt == 0 && strcmp(ToUpper(token).c_str(), "IF") != 0) {
		return conditionMade;
	}

	// Keep printing tokens while one of the 
	// delimiters present in str[].
	while (token != NULL)
	{
		//printf("%s\n", token);
		token = strtok(NULL, " ");
		//strcpy(token, ToUpper(token).c_str());

		//considered alpha
		if (token == NULL) {
			string cond = {};
			//cout << "considered product " << tempor[0] << endl;
			conditionMade.push_back({ "Term", tempor[0] });
			break;
		}
		else if ((strcmp(ToUpper(token).c_str(), "AND") == 0
			|| strcmp(ToUpper(token).c_str(), "THEN") == 0
			|| strcmp(ToUpper(token).c_str(), "OR") == 0)
			&& tempor.size() > 1) {

			string cond = {};
			for (int i = 0; i < tempor.size(); i++) {
				cond += tempor[i];
				cond += " ";
			}
			//cout << "considered alpha " << cond << endl;

			conditionMade.push_back({ "Alpha", cond });

			tempor = {};
		}
		else if ((strcmp(ToUpper(token).c_str(), "AND") == 0
			|| strcmp(ToUpper(token).c_str(), "THEN") == 0
			|| strcmp(ToUpper(token).c_str(), "OR") == 0)
			&& tempor.size() == 1) {
			string cond = {};
			cond = tempor[0];
			//cout << "considered beta " << cond << endl;

			conditionMade.push_back({ "Beta", cond });

			tempor = {};
		}
		else if (itt > 0 && (strcmp(ToUpper(token).c_str(), "AND") != 0
			|| strcmp(ToUpper(token).c_str(), "OR") == 0
			|| (strcmp(ToUpper(token).c_str(), "THEN") != 0))) {
			tempor.push_back(token);
		}

		//Get OR/AND
		if ((strcmp(ToUpper(token).c_str(), "AND") == 0)
			|| (strcmp(ToUpper(token).c_str(), "OR") == 0)) {
			conditionMade.push_back({ "condition", ToUpper(token).c_str() });
		}
		/*if ((strcmp(ToUpper(token).c_str(), "&") == 0)) {
			conditionMade.push_back({ "condition", "AND" });
		}*/

		//token = strtok(NULL, " ");
		itt++;
	}

	return conditionMade;
}

vector<pair<string, string>> Utilities::tokenizeMoreDetail(string temp)
{
	vector<string> tempor = {};
	vector<pair<string, string>> conditionMade = {};

	char* str = (char*)malloc(500);
	strcpy(str, temp.c_str());
	// Returns first token  
	char* token = strtok(str, " ");
	int itt = 0;
	int ittMade = 0;

	//IF
	if (itt == 0 && strcmp(ToUpper(token).c_str(), "IF") == 0) {
		itt++;
		while (token != NULL)
		{
			token = strtok(NULL, " ");

			//considered alpha
			if (token == NULL && tempor.size() > 1) {
				string cond = {};
				for (int i = 0; i < tempor.size(); i++) {
					cond += tempor[i];
					cond += " ";
				}
				conditionMade.push_back({ "Alpha", cond });

				tempor = {};
				break;
			}
			else if (token == NULL && tempor.size() == 1) {

				conditionMade.push_back({ "Beta", tempor[0] });

				tempor = {};
				break;
			}
			else if ((strcmp(ToUpper(token).c_str(), "AND") == 0
				|| strcmp(ToUpper(token).c_str(), "THEN") == 0
				|| strcmp(ToUpper(token).c_str(), "OR") == 0)
				&& tempor.size() > 1) {

				string cond = {};
				for (int i = 0; i < tempor.size(); i++) {
					cond += tempor[i];
					cond += " ";
				}
				conditionMade.push_back({ "Alpha", cond });

				tempor = {};
			}
			//Considered beta
			else if ((strcmp(ToUpper(token).c_str(), "AND") == 0
				|| strcmp(ToUpper(token).c_str(), "THEN") == 0
				|| strcmp(ToUpper(token).c_str(), "OR") == 0)
				&& tempor.size() == 1) {
				string cond = {};
				cond = tempor[0];
				//cout << "considered beta " << cond << endl;

				conditionMade.push_back({ "Beta", cond });

				tempor = {};
			}
			else if (itt > 0 && (strcmp(ToUpper(token).c_str(), "AND") != 0
				|| strcmp(ToUpper(token).c_str(), "OR") == 0
				|| (strcmp(ToUpper(token).c_str(), "THEN") != 0))) {
				tempor.push_back(token);
			}
			//Get OR/AND
			if ((strcmp(ToUpper(token).c_str(), "AND") == 0)
				|| (strcmp(ToUpper(token).c_str(), "OR") == 0)) {
				conditionMade.push_back({ "condition", ToUpper(token).c_str() });
			}

			itt++;
		}
	}
	//FROM
	else if (itt == 0 && strcmp(ToUpper(token).c_str(), "FROM") == 0) {
		token = strtok(NULL, " ");
		conditionMade.push_back({ "Beta", token });
		//return conditionMade;
	}
	//THEN
	else if (itt == 0 && strcmp(ToUpper(token).c_str(), "THEN") == 0) {
		token = strtok(NULL, " ");
		conditionMade.push_back({ "Term", token });
		//return conditionMade;
	}
	else {
		string otherForm = token;
		token = strtok(NULL, " ");
		conditionMade.push_back({ ToUpper(otherForm), token });
		//return conditionMade;
	}
	return conditionMade;
}

long long Utilities::getTime()
{
	std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch());
	return ms.count();
}

string Utilities::ToUpper(string ori)
{
	for (int i = 0; i < ori.size(); i++) {
		ori[i] = toupper(ori[i]);
	}
	return ori;
}

string Utilities::ToLower(string ori)
{
	for (int i = 0; i < ori.size(); i++) {
		ori[i] = tolower(ori[i]);
	}
	return ori;
}

string Utilities::trim(string& str)
{
	string temp = rTrim(str);
	return lTrim(temp);
}

string Utilities::lTrim(string& str)
{
	if (str.length() < 1) return "";
	return str.substr(str.find_first_not_of(" \n\r\t"));
}

string Utilities::rTrim(string& str)
{
	if (str.length() < 1) return "";
	return str.substr(0, str.find_last_not_of(" \n\r\t") + 1);
}

void Utilities::deleteAllMark(string& s, const string& delim)
{
	size_t nSize = delim.size();
	while (1) {
		size_t pos = s.find(delim);
		if (pos == string::npos) {
			return;
		}
		s.erase(pos, nSize);
	}
}

vector<string> Utilities::splitDelim(string str, const string delim)
{
	std::vector<std::string> spiltCollection;
	if (str.size() == 0)
		return spiltCollection;
	int start = 0;
	int idx = str.find(delim, start);
	while (idx != std::string::npos) {
		spiltCollection.push_back(str.substr(start, idx - start));
		start = idx + delim.size();
		idx = str.find(delim, start);
	}
	spiltCollection.push_back(str.substr(start));
	return spiltCollection;
}

double Utilities::randomFloat(int min, int max)
{
	double m1 = (double)(rand() % 101) / 101;
	min++; 
	double m2 = (double)((rand() % (max - min + 1)) + min);
	m2 = m2 - 1; 
	return m1 + m2; 
}

bool Utilities::tokenizeSingleExp(const string expression, string& stream, string& fieldName, string& mid, string& right, string& specialOp)
{
	int index = -1;
	string left;
	if ((index = expression.find(">=")) != -1) {
		left = expression.substr(0, index);
		right = expression.substr(index + 2, expression.size() - index - 2);
		mid = ">=";
	}
	else if ((index = expression.find(">")) != -1) {
		left = expression.substr(0, index);
		right = expression.substr(index + 1, expression.size() - index - 1);
		mid = ">";
	}
	else if ((index = expression.find("<=")) != -1) {
		left = expression.substr(0, index);
		right = expression.substr(index + 2, expression.size() - index - 2);
		mid = "<=";
	}
	else if ((index = expression.find("<")) != -1) {
		left = expression.substr(0, index);
		right = expression.substr(index + 1, expression.size() - index - 1);
		mid = "<";
	}
	else if ((index = expression.find("==")) != -1) {
		left = expression.substr(0, index);
		right = expression.substr(index + 2, expression.size() - index - 2);
		mid = "==";
	}
	else if ((index = expression.find("=")) != -1) {
		left = expression.substr(0, index);
		right = expression.substr(index + 1, expression.size() - index - 1);
		mid = "=";
	}
	else {
		//maybe it is exist expression
		if ((index = expression.find("(")) != -1) {
			specialOp = expression.substr(0, index);
			stream = expression.substr(index + 1, expression.size() - index - 2);
			mid = "=";
			right = "All";
		}
		else {
			//else nothing
			std::cout << "no expression predicate";
			throw runtime_error("");
		}
	}
	if ((index = left.find(".")) != -1) {

		int index2 = left.find("(");
		if (index2 == -1) {
			stream = left.substr(0, index);
			fieldName = left.substr(index + 1, left.size() - index - 1);
		}
		else if (index < index2) {
			stream = left.substr(0, index);
			fieldName = left.substr(index + 1, left.size() - index - 1);
		}
		else {
			fieldName = left;
		}

	}
	else if ((index = left.find("(")) != -1) {
		int index2 = left.find(")");

		specialOp = expression.substr(0, index);
		stream = expression.substr(index + 1, left.size() - index - 2);
	}
	else {
		fieldName = left;
	}
	return true;
}

bool Utilities::tokenizeBetaExp(const string expression, string& leftSource, string& rightSource, string& specialOperation, 
	string& limit, string & condition, string&product)
{


	int index = -1;
	string left, right;
	if ((index = expression.find("then")) < 0) {
		if ((index = expression.find(",")) > 0) { ///might be distance(a,b)<10
			int leftBr = expression.find("(");
			int rightBr = expression.find(")");

			specialOperation = expression.substr(0, leftBr);
			leftSource = expression.substr(leftBr + 1, index - leftBr-1);
			rightSource = expression.substr(index + 1, rightBr - index - 1);

			condition = getComparisonExp(expression);

			int TempLimit = expression.find(condition);

			limit = expression.substr(TempLimit + 1, expression.size() - TempLimit);

			product = expression;

			return true;
		}
		return false;
	}
	else {
		left = expression.substr(0, index-1);
		product = expression.substr(index + 5, expression.size() - index);

		int joinCondition;
		bool specialOp = false;

		vector<string> joinString = { " and ", " or " };
		for (string js : joinString) {
			joinCondition = expression.find(js);
			if (joinCondition > -1) {
				string str = rTrim(js);
				condition = lTrim(str);
				index = joinCondition;

				rightSource = expression.substr(index +1 + condition.size(), left.size() - index - condition.size());
				leftSource = expression.substr(0, index);
			}
		}
	}

	return true;
}

string Utilities::getComparisonExp(const string expression)
{
	vector<string> comparison_exp = { ">", ">=", "<", "<=", "==", "=" };
	for (string s : comparison_exp) {
		int find = expression.find(s);
		if (find > -1)
			return s;
	}
	return "";
}
