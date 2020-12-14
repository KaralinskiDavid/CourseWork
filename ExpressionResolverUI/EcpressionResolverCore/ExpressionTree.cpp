#include "pch.h"
#include "expressionTree.h"

using namespace std;


ExpressionTree::~ExpressionTree()
{
    destruct(this->head);
}

void ExpressionTree::goAroundStraight(ExpressionNode* node)
{
    if (node)
    {
        this->prefixForm.push_back(node->element);
        goAroundStraight(node->leftChild);
        goAroundStraight(node->rightChild);
    }
    return;
}

void ExpressionTree::goAroundInversely(ExpressionNode* node)
{
    if (node)
    {
        goAroundInversely(node->leftChild);
        goAroundInversely(node->rightChild);
        this->postfixForm.push_back(node->element);
    }
    return;
}

void ExpressionTree::goAroundSymmetricaly(ExpressionNode* node, int previousPriority = -1, bool needBrackets = false, int openBracketsCount = 0, int closeBracketsCount = 0, bool isLeft = false)
{
    if (node)
    {
        if (getElementKind(node->element) == OPERATOR)
        {
            bool flag = needBrackets;
            int openBrackets = openBracketsCount;
            int closeBrackets = closeBracketsCount;
            int off = 0;
            int priority = getPriority(node->element);

            if (priority < previousPriority)
            {
                flag = true;
                openBrackets++;
                closeBrackets++;
            }

            if (flag || needBrackets)
                off = 1;

            goAroundSymmetricaly(node->leftChild, priority, flag, openBrackets, closeBrackets > 0 ? closeBrackets - off : closeBrackets, true);
            this->infixForm.push_back(node->element);
            goAroundSymmetricaly(node->rightChild, priority, flag, openBrackets > 0 ? openBrackets - off : openBrackets, closeBrackets, false);
        }
        else
        {
            if (isLeft && needBrackets)
                for (int i = 0; i < openBracketsCount; i++)
                {
                    this->infixForm.push_back("(");
                }
            if (!node->isAuxiliaryNode())
                this->infixForm.push_back(node->element);
            if (!isLeft && needBrackets)
                for (int i = 0; i < closeBracketsCount; i++)
                {
                    this->infixForm.push_back(")");
                }
        }

    }
    return;
}


vector<string> ExpressionTree::getPrefixForm()
{
    this->goAroundStraight(this->head);
    return this->prefixForm;
}

vector<string> ExpressionTree::getInfixForm()
{
    this->goAroundSymmetricaly(this->head, -1, false, 0, false);
    return this->infixForm;
}

vector<string> ExpressionTree::getPostfixForm()
{
    this->goAroundInversely(this->head);
    return this->postfixForm;
}

void ExpressionTree::destruct(ExpressionNode* node)
{
    if (node->leftChild)
        destruct(node->leftChild);
    if (node->rightChild)
        destruct(node->rightChild);
    delete node;
}

ExpressionNode* ExpressionTree::parseInfix(vector<string> expression, ExpressionNode* parent)
{
    int openBrackets = 0;
    int minPriority = INT32_MAX;
    int minPriorityPosition = 0;
    bool isOnlyInBrackets = false;
    for (int i = 0; i < expression.size(); i++)
    {
        int elementkind = getElementKind(expression[i]);
        switch (elementkind)
        {
        case OPENBRACKET:
        {
            openBrackets++;
            break;
        }
        case CLOSEBRACKET:
        {
            openBrackets--;
            break;
        }
        case OPERATOR:
        {
            if (openBrackets == 0)
            {
                int priority = getPriority(expression[i]);
                if (priority < minPriority)
                {
                    minPriority = priority;
                    minPriorityPosition = i;
                }
            }
            break;
        }
        }
    }

    ExpressionNode* node;

    if (minPriority == INT32_MAX)
        isOnlyInBrackets = true;

    if (!isOnlyInBrackets)
        node = new ExpressionNode(expression[minPriorityPosition], parent);
    else
    {
        vector<string> inBrackets(expression.cbegin() + 1, expression.cend() - 1);
        node = parseInfix(inBrackets, parent);
        return node;
    }

    vector<string> left(expression.cbegin(), expression.cbegin() + minPriorityPosition);
    vector<string> right(expression.cbegin() + minPriorityPosition + 1, expression.cend());

    if (left.size() > 1)
        node->leftChild = parseInfix(left, node);
    else if (left.size() == 1)
        node->leftChild = new ExpressionNode(left.back(), node);

    if (right.size() > 1)
        node->rightChild = parseInfix(right, node);
    else if (right.size() == 1)
        node->rightChild = new ExpressionNode(right.back(), node);

    return node;
}

ExpressionNode* ExpressionTree::parsePrefix(vector<string> expression, ExpressionNode* parent, int& parsed)
{
    ExpressionNode* node = NULL;
    int count = 0;
    if (expression.size() > 0)
    {
        if (true)
            parsed++;
        node = new ExpressionNode(expression.back(), parent);
        if (getElementKind(node->element) != OPERATOR)
        {
            return node;
        }
        vector<string> childExpressionLeft(expression.cbegin(), expression.cend() - 1);
        node->leftChild = parsePrefix(childExpressionLeft, node, count);
        vector<string> childExpressionRight(expression.cbegin(), expression.cend() - 1 - count);
        node->rightChild = parsePrefix(childExpressionRight, node, count);
        parsed += count;
        return node;
    }
    return node;
}

ExpressionNode* ExpressionTree::parsePostfix(vector<string> expression, ExpressionNode* parent, int& parsed)
{
    ExpressionNode* node = NULL;
    int count = 0;
    if (expression.size() > 0)
    {
        if (true)
            parsed++;
        node = new ExpressionNode(expression.back(), parent);
        if (getElementKind(node->element) != OPERATOR)
        {
            return node;
        }
        vector<string> childExpressionRight(expression.cbegin(), expression.cend() - 1);
        node->rightChild = parsePostfix(childExpressionRight, node, count);
        vector<string> childExpressionLeft(expression.cbegin(), expression.cend() - 1 - count);
        node->leftChild = parsePostfix(childExpressionLeft, node, count);
        parsed += count;
        return node;
    }
    return node;
}

ExpressionTree::ExpressionTree()
{}

ExpressionTree::ExpressionTree(vector<string> expression, int expressionKind)
{
    switch (expressionKind)
    {
    case PREFIX:
    {
        int parsed = -1;
        reverse(expression.rbegin(), expression.rend());
        this->head = parsePrefix(expression, NULL, parsed);
        break;
    }
    case INFIX:
    {
        this->head = parseInfix(expression, NULL);
        break;
    }
    case POSTFIX:
    {
        int parsed = -1;
        this->head = parsePostfix(expression, NULL, parsed);
        break;
    }
    }
}