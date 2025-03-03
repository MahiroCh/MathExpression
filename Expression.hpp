#ifndef EXPRESSION_H
#define EXPRESSION_H

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

template <typename T>
class Expression {

public:
    
    // КОНСТРУКТОРЫ И ДЕСТРУКТОРЫ

    Expression(); // Для объявления пустых переменных
    Expression(const char* arg); // Конструктор для строки-выражения
    Expression(const T& arg); // Конструктор для числа
    Expression(const Expression<T>& other); // Конструктор копирования
    
    // ПОЛЬЗОВАТЕЛЬСКИЕ МЕТОДЫ
    
    void debugAST() const; // Вывод AST для дебага
    std::string toString() const; // Выражение в строку
    void subsVar(const std::string& varStr); // Подстановка значений
    T evaluate() const; // Вычисление выражения

    // ОПЕРАТОРЫ ДЛЯ ТИПА EXPRESSION

    Expression<T> operator+(const Expression<T>& rhs);
    Expression<T> operator-(const Expression<T>& rhs); 
    Expression<T> operator*(const Expression<T>& rhs);
    Expression<T> operator/(const Expression<T>& rhs);
    Expression<T> operator^(const Expression<T>& rhs);
    Expression<T>& operator=(const Expression<T>& other); // Оператор присваивания

private:

    // УЗЛЫ AST

    struct Node { // Абстрактный класс для узла AST
        virtual ~Node() = default;
        virtual std::string toString() const = 0;
        // virtual void print(int indent = 0) const = 0;
    };

    std::unique_ptr<Node> root; // Корень AST

    struct NumberNode : Node { // Узел для чисел
        T value;
        NumberNode(T value) : value(value) {}
        std::string toString() const override;
        // void print(int indent) const override;
    };

    struct VariableNode : Node { // Узел для переменных
        std::string name;
        VariableNode(const std::string& name) : name(name) {}
        std::string toString() const override;
        // void print(int indent) const override;
    };

    struct BinaryOperationNode : Node { // Узел для бинарных операций
        char operation;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
        BinaryOperationNode(char operation, std::unique_ptr<Node> left, std::unique_ptr<Node> right)
            : operation(operation), left(std::move(left)), right(std::move(right)) {}
        std::string toString() const override;
        // void print(int indent) const override;
    };

    struct FunctionNode : Node { // Узел для функций
        std::string function;
        std::unique_ptr<Node> arg;
        FunctionNode(const std::string& function, std::unique_ptr<Node> arg)
            : function(function), arg(std::move(arg)) {}
        std::string toString() const override;
        // void print(int indent) const override;
    };
    
    // ФУНКЦИИ ДЛЯ ПАРСИНГА

    std::vector<std::string> tokenize(const std::string& expr); // Токенизация выражения для последующего парсинга

    // // ПАРСИНГ МАТЕМАТИКИ В СООТВЕТСВИИ С PEMDAS:

    std::unique_ptr<Node> parseExpression(const std::vector<std::string>& tokens, size_t& pos); // Парсинг выражения AS
    std::unique_ptr<Node> parseTerm(const std::vector<std::string>& tokens, size_t& pos); // Парсинг терма MD
    std::unique_ptr<Node> parseExponent(const std::vector<std::string>& tokens, size_t& pos); // Парсинг возведения в степень E
    std::unique_ptr<Node> parseFactor(const std::vector<std::string>& tokens, size_t& pos); // Парсинг фактора P 
                                                                                            // и атомарных элементов
    // // ПАРСИНГ АТОМАРНЫХ ЭЛЕМЕНТОВ:     

    std::unique_ptr<Node> parseNumber(const std::vector<std::string>& tokens, size_t& pos); // Парсинг числа
    std::unique_ptr<Node> parseVariable(const std::vector<std::string>& tokens, size_t& pos); // Парсинг переменной
    std::unique_ptr<Node> parseFunction(const std::vector<std::string>& tokens, size_t& pos); // Парсинг функции

    // ДРУГИЕ ФУНКЦИИ
    
    T evaluateHelper(const Node* node) const;
    std::unique_ptr<Node> copyNode(const Node* node) const;
    std::unique_ptr<Node> subsVarHelper(std::unique_ptr<Node> node, 
                                        const std::unordered_map<std::string, T>& varMap);
};

#endif  