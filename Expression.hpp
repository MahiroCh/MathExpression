#ifndef EXPR_H
#define EXPR_H

#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <cctype>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <complex>
#include <type_traits>
#include <sstream>

template <typename T>
class Expression {
public:

    // ---------------------------------------------------------------------------------------------------- //
    // КОНСТРУКТОРЫ И ДЕСТРУКТОРЫ
    // ---------------------------------------------------------------------------------------------------- //

    /*
    Конструктор для объявления переменной без выражения.
    */
    Expression(); 

    /*
    Конструктор выражения из строки.
    */
    Expression(const char*); 

    /*
    Конструктор выражения из числа.
    */
    Expression(const T&); 

    /*
    Конструктор копирования.
    */
    Expression(const Expression<T>&);

    /*
    Конструктор перемещения.
    */
    Expression(Expression<T>&& other) noexcept;
    
    // ---------------------------------------------------------------------------------------------------- //
    // ПОЛЬЗОВАТЕЛЬСКИЕ МЕТОДЫ
    // ---------------------------------------------------------------------------------------------------- //

    /*
    Выражение в строку.
    */
    std::string toString() const;

    /*
    Замена переменных.
    */
    void subsVar(const std::string&);

    /*
    Вычислить значение выражения.
    */
    T evaluate() const;

    /*
    Продифференцировать по переменной.
    */
    Expression<T> differentiate(const std::string&) const;
    
    /*
    Для дебага.: Вывод АСТ-дерева.
    */
    void debugAST() const;

    // ---------------------------------------------------------------------------------------------------- //
    // ОПЕРАТОРЫ ДЛЯ ТИПА EXPRESSION
    // ---------------------------------------------------------------------------------------------------- //

    Expression<T> operator+(const Expression<T>&);
    Expression<T> operator-(const Expression<T>&); 
    Expression<T> operator*(const Expression<T>&);
    Expression<T> operator/(const Expression<T>&);
    Expression<T> operator^(const Expression<T>&);
    Expression<T>& operator=(const Expression<T>&); // Оператор присваивания.
    Expression<T>& operator=(Expression<T>&&) noexcept; // Оператор перемещения.


private:

    // ---------------------------------------------------------------------------------------------------- //
    // AST (АБСТРАКТНОЕ СИНТАКСИЧЕСКОЕ ДЕРЕВО)
    // ---------------------------------------------------------------------------------------------------- //

    /*
    Абстрактный класс для узла AST.
    */
    struct Node {

        virtual ~Node() = default;
        virtual std::string nodeToString() const = 0;
        virtual void print(int indent = 0) const = 0; // Для дебага.
    };

    /*
    Узел для чисел.
    */
    struct NumberNode : Node {

        T value;
        NumberNode(T value) : value{value} {}
        std::string nodeToString() const override;
        void print(int) const override; // Для дебага.
    };

    /*
    Узел для переменных.
    */
    struct VariableNode : Node {

        std::string name;
        VariableNode(const std::string& name) : name{name} {}
        std::string nodeToString() const override;
        void print(int) const override; // Для дебага.
    };

    /*
    Узел для бинарных операций.
    */
    struct BinaryOperationNode : Node {

        char operation;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
        BinaryOperationNode(char operation, std::unique_ptr<Node> left, std::unique_ptr<Node> right) 
            : operation{operation}, left{std::move(left)}, right{std::move(right)} {}
        std::string nodeToString() const override;
        void print(int) const override; // Для дебага.
    };

    /*
    Узел для унарных операций.
    */
    struct UnaryOperationNode : Node {

        char operation;
        std::unique_ptr<Node> arg;
        UnaryOperationNode(char operation, std::unique_ptr<Node> operand) 
            : operation{operation}, arg{std::move(operand)} {}
        std::string nodeToString() const override;
        void print(int) const override; // Для дебага.
    };

    /*
    Узел для функций.
    */
    struct FunctionNode : Node { 

        std::string function;
        std::unique_ptr<Node> arg;
        FunctionNode(const std::string& function, std::unique_ptr<Node> arg) 
            : function{function}, arg{std::move(arg)} {}
        std::string nodeToString() const override;
        void print(int) const override; // Для дебага.
    };

    /*
    Корень АСТ-дерева.
    */
    std::unique_ptr<Node> root;
    
    // ---------------------------------------------------------------------------------------------------- //
    // НУЛЕВОЙ ЭТАП ПАРСИНГА СТРОКИ В ВЫРАЖЕНИЕ (ХРАНЕНИЕ В AST-ДЕРЕВЕ)
    // ---------------------------------------------------------------------------------------------------- //

    /*
    Токенизация выражения для последующего парсинга.
    */
    std::vector<std::string> tokenize(const std::string&);

    // ---------------------------------------------------------------------------------------------------- //
    // ПЕРВЫЙ ЭТАП ПАРСИНГА СТРОКИ В ВЫРАЖЕНИЕ (НА УРОВНЕ ОПЕРАЦИЙ В СООТВЕТСВИИ С PEMDAS)
    // ---------------------------------------------------------------------------------------------------- //

    /*
    Сложение и вычитание.
    */
    std::unique_ptr<Node> parseExpression(const std::vector<std::string>&, size_t&);

    /*
    Умножение и деление.
    */
    std::unique_ptr<Node> parseTerm(const std::vector<std::string>&, size_t&);

    /*
    Возведение в степень.
    */
    std::unique_ptr<Node> parseExponent(const std::vector<std::string>&, size_t&);

    /*
    Скобки.
    */
    std::unique_ptr<Node> parseFactor(const std::vector<std::string>&, size_t&);

    // ---------------------------------------------------------------------------------------------------- //
    // ВТОРОЙ ЭТАП ПАРСИНГА СТРОКИ В ВЫРАЖЕНИЕ (НА УРОВНЕ АТОМАРНЫХ ЭЛЕМЕНТОВ)
    // ---------------------------------------------------------------------------------------------------- //

    /*
    Числа.
    */
    std::unique_ptr<Node> parseNumber(const std::vector<std::string>&, size_t&);

    /*
    Переменные.
    */
    std::unique_ptr<Node> parseVariable(const std::vector<std::string>&, size_t&);

    /*
    Функции.
    */
    std::unique_ptr<Node> parseFunction(const std::vector<std::string>&, size_t&);

    // ---------------------------------------------------------------------------------------------------- //
    // ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ
    // ---------------------------------------------------------------------------------------------------- //

    /*
    Вычисление значения выражения (основное тело).
    */
    T evaluateHelper(const Node*) const;

    /*
    Замена переменных в выражении (основное тело).
    */
    std::unique_ptr<Node> subsVarHelper(std::unique_ptr<Node>, const std::unordered_map<std::string, T>&);

    /*
    Дифференцирование выражения (основное тело).
    */
    std::unique_ptr<Node> differentiateHelper(const Node*, const std::string&) const;

    /*
    Копирование дерева.
    */
    std::unique_ptr<Node> copyTree(const Node*) const;

    /*
    Конвертация числа в строку.
    */
    static std::string numToString(const long double&);

    /*
    Конвертация одночлена в комплексное число.
    */
    static std::complex<long double> interpretComplex(const std::string& str);
};

std::ostream& operator<<(std::ostream&, const std::complex<long double>&);

#endif
