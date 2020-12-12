#pragma once

using namespace std;


enum Expressionkinds
{
    POSTFIX,
    INFIX,
    PREFIX,
    ONESYMBOL
};

enum Elementkinds
{
    OPERATOR,
    OPERAND,
    OPENBRACKET,
    CLOSEBRACKET,
    INVALID
};

enum OperatorKinds
{
    PLUS,
    MINUS,
    OBELUS,
    TIMES,
    EXPONENTIATION
};

enum ValidationErrors
{
    NO_ERRORS,
    UNKNOWN_SYMBOLS,
    INVALID_BRACKETS_SEQUENCE,
    INVALID_OPERATORS_COUNT,
    WRONG_OPERATORS_SEQUENCE
};


