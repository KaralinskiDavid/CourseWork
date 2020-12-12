#pragma once

#include <string>

class ExpressionNode
{
public:
    std::string element;
    ExpressionNode* leftChild;
    ExpressionNode* rightChild;
    ExpressionNode* parent;
};

