#include "pch.h"
#include "ExpressionResolver.h"
#include "ErrorMessages.h"

using namespace std;

vector<string> ExpressionResolver::infixToPostfix(vector<string> expression)
{
    vector<string> op;
    vector<string> result;
    for (int i = 0; i < expression.size(); i++)
    {
        int operatorkind = getElementKind(expression[i]);
        switch (operatorkind) {
        case OPERATOR:
        {
            while (op.size() > 0 && !(op.back() == "(") && getPriority(op.back()) >= getPriority(expression[i]))
            {
                result.push_back(op.back());
                op.pop_back();
            }
            op.push_back(expression[i]);
            break;
        }
        case OPENBRACKET:
        {
            op.push_back(expression[i]);
            break;
        }
        case CLOSEBRACKET:
        {
            while (!(op.back() == "("))
            {
                result.push_back(op.back());
                op.pop_back();
            }
            op.pop_back();
            break;
        }
        case OPERAND:
        {
            result.push_back(expression[i]);
            break;
        }
        }
    }
    while (op.size() > 0)
    {
        result.push_back(op.back());
        op.pop_back();
    }
    return result;
}

ExpressionNode* ExpressionResolver::getTreeRoot(string expression)
{
    this->resolveExpression(expression, 0, false, true);
    return this->tree->head;
}

void ExpressionResolver::setExpressionTree(vector<string> expression, int expressionKind)
{
    this->tree = new ExpressionTree(expression, expressionKind);
}

vector<string> ExpressionResolver::postfixToInfix(vector<string> expression)
{
    if(this->tree==NULL)
        this->tree = new ExpressionTree(expression, POSTFIX);
    return tree->getInfixForm();
}

vector<string> ExpressionResolver::infixToPrefix(vector<string> expression)
{
    if (this->tree == NULL)
        this->tree = new ExpressionTree(expression, INFIX);
    return tree->getPrefixForm();
}

vector<string> ExpressionResolver::prefixToInfix(vector<string> expression)
{
    if (this->tree == NULL)
        this->tree = new ExpressionTree(expression, PREFIX);
    return tree->getInfixForm();
}

vector<string> ExpressionResolver::prefixToPostfix(vector<string> expression)
{
    if (this->tree == NULL)
        this->tree = new ExpressionTree(expression, PREFIX);
    return tree->getPostfixForm();
}

vector<string> ExpressionResolver::postfixToPrefix(vector<string> expression)
{
    if (this->tree == NULL)
        this->tree = new ExpressionTree(expression, POSTFIX);
    return tree->getPrefixForm();
}

string ExpressionResolver::getExpressionString(vector<string> expression)
{
    string expressionString;
    for (int i = 0; i < expression.size(); i++)
    {
        expressionString += expression[i] + ' ';
    }
    return expressionString;
}

vector<string> ExpressionResolver::getExpression(string expression)
{
    vector<string> initiallySplitted = split(expression, ' ');
    return splitElements(initiallySplitted);
}

string ExpressionResolver::resolveExpression(string expressionString, int to, bool calculate = false, bool isOnlyTree)
{
    if (isOnlyTree && expressionString == this->previousExpression)
        return "";
    if (!isOnlyTree && expressionString == this->previousExpression && to == this->previousTo)
        return this->previousTranslation;
    this->previousExpression == expressionString;
    this->previousTo == to;
    vector<string> expression = getExpression(expressionString);
    int expressionKind = getExprKind(expression);
    switch (expressionKind)
    {
    case ONESYMBOL:
    {
        if (getElementKind(expressionString) != OPERAND)
        {
            this->tree = new ExpressionTree();
            this->tree->head = new ExpressionNode(expressionString, NULL);
            return "Not expression";
        }
        return expressionString;
    }
    case INFIX:
    {
        int validationResult = validateInfix(expression);
        switch (validationResult)
        {
        case UNKNOWN_SYMBOLS:
        {
            this->previousTranslation = UNKNOWN_SYMBOLS_ERROR;
            return UNKNOWN_SYMBOLS_ERROR;
        }
        case INVALID_BRACKETS_SEQUENCE:
        {
            this->previousTranslation = BRACKETS_SEQUENCE_ERROR;
            return BRACKETS_SEQUENCE_ERROR;
        }
        case INVALID_OPERATORS_COUNT:
        {
            this->previousTranslation = OPERATORS_COUNT_ERROR;
            return OPERATORS_COUNT_ERROR;
        }
        case WRONG_OPERATORS_SEQUENCE:
        {
            this->previousTranslation = OPERATORS_SEQUENCE_ERROR;
            return OPERATORS_SEQUENCE_ERROR;
        }
        case NO_ERRORS:
        {
            resolveUnaryMinuses(expression);
            setExpressionTree(expression, INFIX);
            if (isOnlyTree)
                return "";
			if (calculate)
			{
				string s = to_string(calculatePostfix(infixToPostfix(expression)));
				return s.substr(0, s.find_first_of('.') < s.find_last_not_of('0') ? s.find_last_not_of('0') + 1 : s.find_last_not_of('0'));
			}
			switch (to)
			{
			case INFIX:
				this->previousTranslation = expressionString;
				return expressionString;
			case POSTFIX:
				this->previousTranslation = getExpressionString(infixToPostfix(expression));
				return this->previousTranslation;
			case PREFIX:
				this->previousTranslation = getExpressionString(infixToPrefix(expression));
				return this->previousTranslation;
			}

		}
		}
        break;
    }
    case POSTFIX:
    {
        int validationResult = validatePostfix(expression);
        switch (validationResult)
        {
        case UNKNOWN_SYMBOLS:
        {
            this->previousTranslation = UNKNOWN_SYMBOLS_ERROR;
            return UNKNOWN_SYMBOLS_ERROR;
        }
        case INVALID_OPERATORS_COUNT:
        {
            this->previousTranslation = OPERATORS_COUNT_ERROR;
            return OPERATORS_COUNT_ERROR;
        }
        case WRONG_OPERATORS_SEQUENCE:
        {
            this->previousTranslation = OPERATORS_SEQUENCE_ERROR;
            return OPERATORS_SEQUENCE_ERROR;
        }
        case NO_ERRORS:
        {
            setExpressionTree(expression, POSTFIX);
            if (isOnlyTree)
                return "";
			if (calculate)
			{
				string s = to_string(calculatePostfix(expression));
				return s.substr(0, s.find_first_of('.') < s.find_last_not_of('0') ? s.find_last_not_of('0') + 1 : s.find_last_not_of('0'));
			}
			switch (to)
			{
			case POSTFIX:
				this->previousTranslation = expressionString;
				return expressionString;
			case INFIX:
				this->previousTranslation = getExpressionString(postfixToInfix(expression));
				return this->previousTranslation;
			case PREFIX:
				this->previousTranslation = getExpressionString(postfixToPrefix(expression));
				return this->previousTranslation;
			}
        }
        }
    }
    case PREFIX:
    {
        int validationResult = validatePrefix(expression);
        switch (validationResult)
        {
        case UNKNOWN_SYMBOLS:
        {
            this->previousTranslation = UNKNOWN_SYMBOLS_ERROR;
            return UNKNOWN_SYMBOLS_ERROR;
        }
        case INVALID_OPERATORS_COUNT:
        {
            this->previousTranslation = OPERATORS_COUNT_ERROR;
            return OPERATORS_COUNT_ERROR;
        }
        case WRONG_OPERATORS_SEQUENCE:
        {
            this->previousTranslation = OPERATORS_SEQUENCE_ERROR;
            return OPERATORS_SEQUENCE_ERROR;
        }
        case NO_ERRORS:
        {
            setExpressionTree(expression, PREFIX);
            if (isOnlyTree)
                return "";
			if (calculate)
			{
				string s = to_string(calculatePostfix(prefixToPostfix(expression)));
				return s.substr(0, s.find_first_of('.') < s.find_last_not_of('0') ? s.find_last_not_of('0') + 1 : s.find_last_not_of('0'));
			}
			switch (to)
			{
			case PREFIX:
				this->previousTranslation = expressionString;
				return expressionString;
			case INFIX:
				this->previousTranslation = getExpressionString(prefixToInfix(expression));
				return this->previousTranslation;
			case POSTFIX:
				this->previousTranslation = getExpressionString(prefixToPostfix(expression));
				return this->previousTranslation;
			}

        }
        }
    }
    }
    return "Impossible";
}

double ExpressionResolver::calculatePostfix(vector<string> expression)
{
    vector<double> operandStack;
    for (int i = 0; i < expression.size(); i++)
    {
        int elementKind = getElementKind(expression[i]);
        if (elementKind == OPERAND)
            operandStack.push_back(stod(expression[i]));
        if (elementKind == OPERATOR)
        {
            double right = operandStack.back();
            operandStack.pop_back();
            double left = operandStack.back();
            operandStack.pop_back();
            double result;
            int operatorKind = getOperatorKind(expression[i]);
            switch (operatorKind)
            {
            case PLUS:
            {
                result = left + right;
                break;
            }
            case MINUS:
            {
                result = left - right;
                break;
            }
            case TIMES:
            {
                result = left * right;
                break;
            }
            case OBELUS:
            {
                result = left / right;
                break;
            }
            case EXPONENTIATION:
            {
                result = pow(left, right);
                break;
            }
            }
            operandStack.push_back(result);
        }
    }
    return operandStack.back();
}

ExpressionResolver::ExpressionResolver()
{
    this->tree = NULL;
    this->previousTo = 0;
    this->previousExpression = "AAA";
    this->previousTranslation = "AAA";
}