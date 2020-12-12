#pragma once

#include <vector>
#include <string>
#include "Enums.h"

using namespace std;

int getExprKind(vector<string> expression);
int getPriority(string element);
int getElementKind(string element);
int getOperatorKind(string element);
vector<string> splitElements(vector<string> expression);
bool checkBrackets(vector<string> expression);
bool checkoperandscount(vector<string> expression);
bool checkForUnknownSymbols(vector<string> expression);
int validatePostfix(vector<string> expression);
int validatePrefix(vector<string> expression);
int validateInfix(vector<string> expression);
vector<string> split(const string& s, char delimiter);
bool isUnaryMinus(vector<string> expression, int elementnumber);
void resolveUnaryMinuses(vector<string>& expression);
