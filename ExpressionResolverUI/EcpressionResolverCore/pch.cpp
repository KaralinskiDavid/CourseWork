// pch.cpp: файл исходного кода, соответствующий предварительно скомпилированному заголовочному файлу

#include "pch.h"
#include "ExpressionResolver.h"

// При использовании предварительно скомпилированных заголовочных файлов необходим следующий файл исходного кода для выполнения сборки.

ExpressionResolver* resolver;

extern __declspec(dllexport) string resolveExpression(string expression, int to, bool calculate) {
	if(resolver==NULL)
		resolver = new ExpressionResolver();
	return resolver->resolveExpression(expression, to, calculate);
}

extern __declspec(dllexport) ExpressionNode* getExpressionTree(string expression) {
	if (resolver == NULL)
		resolver = new ExpressionResolver();
	return resolver->getTreeRoot(expression);
}