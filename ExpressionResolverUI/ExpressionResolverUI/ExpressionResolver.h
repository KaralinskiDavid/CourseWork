#pragma once

#include <string>

class ExpressionResolver
{
public:
    std::string resolveExpression(std::string expressionString, int to, bool calculate);
};
