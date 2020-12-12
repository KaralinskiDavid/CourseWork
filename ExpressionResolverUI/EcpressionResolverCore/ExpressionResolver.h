#pragma once

#include "expressionTree.h"

using namespace std;

class ExpressionResolver
{
public:
    string resolveExpression(string expressionString, int to, bool calculate, bool isOnlyTree=false);
    ExpressionNode* getTreeRoot(string expression);
    ExpressionResolver();
private:
    ExpressionTree* tree;
    string previousExpression;
    string previousTranslation;
    int previousTo;
    void setExpressionTree(vector<string> expression, int expressionKind);
    vector<string> getExpression(string expression);
    string getExpressionString(vector<string> expression);
    vector<string> infixToPostfix(vector<string> expression);
    vector<string> prefixToPostfix(vector<string> expression);
    vector<string> infixToPrefix(vector<string> expression);
    vector<string> postfixToPrefix(vector<string> expression);
    vector<string> postfixToInfix(vector<string> expression);
    vector<string> prefixToInfix(vector<string> expression);
    double calculatePostfix(vector<string> expression);
};
