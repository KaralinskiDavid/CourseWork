#pragma once

#include "expressionNode.h"

class ExpressionTree
{
public:
    std::vector<std::string> getPrefixForm();
    std::vector<std::string> getInfixForm();
    std::vector<std::string> getPostfixForm();
    ExpressionNode* head;
    ExpressionTree();
    ExpressionTree(std::vector<std::string> expression, int expressionKind);
    ~ExpressionTree();
private:
    std::vector<std::string> prefixForm;
    std::vector<std::string> infixForm;
    std::vector<std::string> postfixForm;
    void goAroundStraight(ExpressionNode* node);
    void goAroundInversely(ExpressionNode* node);
    void goAroundSymmetricaly(ExpressionNode* node, int previousPriority, bool needBrackets, int openBracketsCount, int closeBracketsCount, bool isLeft);
    void destruct(ExpressionNode* node);
    ExpressionNode* parseInfix(std::vector<std::string> expression, ExpressionNode* parent);
    ExpressionNode* parsePrefix(std::vector<std::string> expression, ExpressionNode* parent, int& parsed);
    ExpressionNode* parsePostfix(std::vector<std::string> expression, ExpressionNode* parent, int& parsed);
};