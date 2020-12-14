#pragma once

#include <string>

using namespace std;

class HistoryLine
{
public:
	HistoryLine(wstring _username, wstring _date, wstring _expression, wstring _operation, wstring _notation, wstring _result)
	{
		username = _username;
		date = _date;
		expression = _expression;
		operation = _operation;
		notation = _notation;
		result = _result;
	}
	wstring username;
	wstring date;
	wstring expression;
	wstring operation;
	wstring notation;
	wstring result;
};
