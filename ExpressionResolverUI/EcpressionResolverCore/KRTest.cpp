// KRTest.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include "ExpressionResolver.h"


using namespace std;




int main()
{
    std::cout << "Hello World!\n";
    string s;
    while (s != "stop")
    {
        getline(cin, s);
        ExpressionResolver* resolver = new ExpressionResolver();
        string result = resolver->resolveExpression(s, INFIX, true);
        cout << result << endl;
    }
   /* vector<string> elements = split(s, ' ');
    vector<string> splitted = splitElements(elements);
    resolveUnaryMinuses(splitted);*/
    //double result = calculate(splitted, INFIX);
    /*resolveUnaryMinuses(splitted);
    bool res = checkBrackets(splitted);
    int kind = getExprKind(splitted);
    bool valid = validate(splitted);
    ExpressionTree* tree = new ExpressionTree(splitted, INFIX);
    vector<string> postf = tree->getPostfixForm();
    vector<string> inf = tree->getInfixForm();
    vector<string> pref = tree->getPrefixForm();
    tree->destruct(tree->head);*/
    //string result = postfix(splitted);
   // double calculated = calculatePostfix(splitted);
   // cout << result;
}



// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
