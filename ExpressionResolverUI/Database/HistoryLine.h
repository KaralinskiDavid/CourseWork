#pragma once

#include <string>

using namespace std;

class HistoryLine
{
public:
	HistoryLine(wstring _username, wstring _date, wstring _expression, wstring _notation, wstring _result)
	{
		username = _username;
		date = _date;
		expression = _expression;
		operation = L"Translate";
		notation = _notation;
		result = _result;
	}
	HistoryLine(wstring _date, wstring _expression, wstring _notation, wstring _result)
	{
		date = _date;
		expression = _expression;
		operation = L"Translate";
		notation = _notation;
		result = _result;
	}
	HistoryLine(wstring _date, wstring _expression, wstring _result)
	{
		date = _date;
		expression = _expression;
		operation = L"Calculate";
		notation = L"-";
		result = _result;
	}
	HistoryLine(wstring _date, wstring _expression, wstring _operation)
	{
		date = _date;
		expression = _expression;
		operation = L"View tree";
		notation = L"-";
		result = L"-";
	}
	wstring username;
	wstring date;
	wstring expression;
	wstring operation;
	wstring notation;
	wstring result;
};
