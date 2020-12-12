#pragma once

#include "validation.h"

class ExpressionNode
{
public:
    std::string element;
    ExpressionNode* leftChild;
    ExpressionNode* rightChild;
    ExpressionNode* parent;
    ExpressionNode(std::string element, ExpressionNode* parent);
    bool isAuxiliaryNode();
};
