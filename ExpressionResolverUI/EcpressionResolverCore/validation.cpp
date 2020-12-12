#include "pch.h"
#include <sstream>
#include "validation.h"

using namespace std;

string operators = "-+*/^";
vector<int> prioritets{ 0, 0, 1, 1, 2 };

int getPriority(string element)
{
    return  prioritets[operators.find(element)];
}

int getElementKind(string element)
{
    if (operators.find(element) != -1)
        return OPERATOR;
    if (element == "(")
        return OPENBRACKET;
    if (element == ")")
        return CLOSEBRACKET;
    for (int i = 0; i < element.length(); i++)
    {
        if (element[i] < 48 || element[i]>57)
            return INVALID;
    }
    return OPERAND;
}

int getOperatorKind(string element)
{
    if (element == "+")
        return PLUS;
    if (element == "-")
        return MINUS;
    if (element == "/")
        return OBELUS;
    if (element == "*")
        return TIMES;
    return EXPONENTIATION;
}

vector<string> split(const string& s, char delimiter)
{
    vector<string> elements;
    stringstream ss(s);
    string element;
    while (getline(ss, element, delimiter))
    {
        elements.push_back(element);
    }
    return elements;
}

bool isUnaryMinus(vector<string> expression, int elementnumber)
{
    if (getExprKind(expression)==INFIX && getOperatorKind(expression[elementnumber]) == MINUS)
    {
        if (elementnumber == 0)
            return true;
        if (getElementKind(expression[elementnumber - 1]) == OPENBRACKET)
            return true;
    }
    return false;
}

int getExprKind(vector<string> expression)
{
    if (expression.size() == 1)
        return ONESYMBOL;
    string last = expression.back();
    string first = expression.front();
    if (getElementKind(last)==OPERATOR)
        return POSTFIX;
    if ((getElementKind(first) == OPERATOR && getOperatorKind(first)!=MINUS) ||
        (getOperatorKind(first) == MINUS && expression.size()>2 && getElementKind(expression[expression.size()-2])!=OPERATOR))
        return PREFIX;
    return INFIX;
}

bool checkBrackets(vector<string> expression)
{
    int open = 0;
    for (int i = 0; i < expression.size(); i++)
    {
        if (expression[i] == "(")
            open++;
        if (expression[i] == ")" && open < 1)
            return false;
        if (expression[i] == ")")
            open--;
        if (open < 0)
            return false;
    }
    if (open > 0)
        return false;
    return true;
}

bool checkoperandscount(vector<string> expression)
{
    int operatorscount = 0;
    int operandscount = 0;
    for (int i = 0; i < expression.size(); i++)
    {
        int elementkind = getElementKind(expression[i]);
        if (elementkind == OPERATOR && !isUnaryMinus(expression, i))
            operatorscount++;
        if (elementkind == OPERAND)
            operandscount++;
    }
    if (operandscount - operatorscount != 1)
        return false;
    return true;
}

bool checkForUnknownSymbols(vector<string> expression)
{
    for (int i = 0; i < expression.size(); i++)
    {
        if (getElementKind(expression[i]) == INVALID)
            return false;
    }
    return true;
}

int validatePrefix(vector<string> expression)
{
    if (!checkForUnknownSymbols(expression))
        return UNKNOWN_SYMBOLS;
    if (!checkoperandscount(expression))
        return INVALID_OPERATORS_COUNT;
    if (getElementKind(expression[expression.size()-1]) == OPERATOR)
        return WRONG_OPERATORS_SEQUENCE;
    return NO_ERRORS;
}

int validatePostfix(vector<string> expression)
{
    if (!checkForUnknownSymbols(expression))
        return UNKNOWN_SYMBOLS;
    if (!checkoperandscount(expression))
        return INVALID_OPERATORS_COUNT;
    if (getElementKind(expression[1]) == OPERATOR)
        return WRONG_OPERATORS_SEQUENCE;
    return NO_ERRORS;
}

int validateInfix(vector<string> expression)
{
    if (!checkForUnknownSymbols(expression))
        return UNKNOWN_SYMBOLS;
    if (!checkBrackets(expression))
        return INVALID_BRACKETS_SEQUENCE;
    if (!checkoperandscount(expression))
        return INVALID_OPERATORS_COUNT;
    for (int i = 0; i < expression.size() - 1; i++)
    {
        int curOperatorKnd = getOperatorKind(expression[i]);
        int nextOperatorKind = getOperatorKind(expression[i + 1]);
        if ((curOperatorKnd == OPENBRACKET && nextOperatorKind == OPERATOR) ||
            (curOperatorKnd == CLOSEBRACKET && nextOperatorKind == OPENBRACKET) ||
            (curOperatorKnd == OPERATOR && nextOperatorKind == OPERATOR) ||
            (curOperatorKnd == OPERAND && nextOperatorKind == OPERAND))
            return WRONG_OPERATORS_SEQUENCE;
    }
    return NO_ERRORS;
}

vector<string> splitElements(vector<string> expression)
{
    for (int i = 0; i < expression.size(); i++)
    {
        string element = expression[i];
        int length = element.length();
        int deleted = 0;
        if (length > 1)
        {
            int k=0;
            for (int j = 0; j < element.length(); j++)
            {
                if (operators.find(element[j]) != -1 || element[j] == '(' || element[j] == ')')
                {
                    k = 0;
                    auto begin = expression.begin();
                    if (j == 0)
                    {
                        string s(1, element[j]);
                        expression.emplace(begin + i, s);
                        element.erase(element.begin() + j);
                        k += 1;
                        break;
                    }
                    else
                    {
                        string left = element.substr(0, j);
                        string right = element.substr(j + 1, element.length() - j);
                        string s(1, element[j]);
                        if (j < element.length() - 1)
                            element.erase(0, j + 1);
                        else
                            element.erase(0, j);
                        expression.emplace(begin + i, left);
                        expression.emplace(expression.begin() + i + 1, s);
                        if (right == "")
                        {
                            expression.pop_back();
                            k--;
                        }
                        k += 2;
                        break;
                    }
                }
            }
            expression[i + k] = element;
        }
    }
    return expression;
}

void resolveUnaryMinuses(vector<string>& expression)
{
    int i = 0;
    while (i < expression.size())
    {
        if (isUnaryMinus(expression, i))
        {
            expression.insert(expression.cbegin() + i, "0");
            expression.insert(expression.cbegin() + i, "(");
            i += 2;
            if (getElementKind(expression[i + 1]) == OPENBRACKET)
            {
                int openBrackets = 1;
                int j = i + 1;
                while (openBrackets != 0)
                {
                    j++;
                    if (getElementKind(expression[j]) == CLOSEBRACKET)
                        openBrackets--;
                    if (getElementKind(expression[j]) == OPENBRACKET)
                        openBrackets++;
                }
                expression.insert(expression.cbegin() + j, ")");
            }
            else
            {
                expression.insert(expression.cbegin() + i + 2, ")");
            }
        }
        i++;
    }
    return;
}