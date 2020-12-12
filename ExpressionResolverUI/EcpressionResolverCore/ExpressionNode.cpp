#include "pch.h"
#include "expressionNode.h"

using namespace std;

ExpressionNode::ExpressionNode(string element, ExpressionNode* parent)
{
    this->element = element;
    this->parent = parent;
    this->leftChild = NULL;
    this->rightChild = NULL;
}

bool ExpressionNode::isAuxiliaryNode()
{
    return (this && this->element == "0" && this->parent && getOperatorKind(this->parent->element) == MINUS
        && this->parent->leftChild == this);
}



