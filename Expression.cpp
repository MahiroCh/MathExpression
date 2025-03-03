#include "Expression.hpp"

// КОНСТРУКТОРЫ И ДЕСТРУКТОРЫ

template <typename T>
Expression<T>::Expression() : root(nullptr) {} // Для объявления пустых пеменных

template <typename T>
Expression<T>::Expression(const char* arg) { // Конструктор для строки-выражения
    std::vector<std::string> tokens = tokenize((std::string)arg);
    size_t pos = 0;
    root = parseExpression(tokens, pos);
    if (pos != tokens.size())
        throw std::runtime_error("Invalid expression");
}

template <typename T>
Expression<T>::Expression(const T &arg) { // Конструктор для числа
    std::vector<std::string> tokens;

    if constexpr (std::is_same_v<T, std::complex<long double>>) {
        std::string argg = std::to_string(arg.real()) + '+' + std::to_string(arg.imag()) + 'I';
        tokens = tokenize(argg);
    }
    else {
        tokens = tokenize(std::to_string(arg));
    }
    
    size_t pos = 0;
    root = parseExpression(tokens, pos);
    if (pos != tokens.size())
        throw std::runtime_error("Invalid expression");
}

template <typename T>
Expression<T>::Expression(const Expression<T>& other) { // Конструктор копирования
    if (other.root) {
        root = copyNode(other.root.get());
    }
}

// ПОЛЬЗОВАТЕЛЬСКИЕ МЕТОДЫ

// template <typename T>
// void Expression<T>::debugAST() const { // Вывод AST для дебага
//     if (root) {
//         std::cout << "\nCurrent AST tree state:\n";
//         root->print();
//     }
//     else std::cout << "Empty AST tree.\n";
// }

template <typename T>
std::string Expression<T>::toString() const { // Выражение в строку
    if (root) 
        return root->toString();
    return "";
}

template <typename T>
void Expression<T>::subsVar(const std::string& varStr) { // Замена переменных
    std::unordered_map<std::string, T> varMap;
    std::vector<std::string> tokens = tokenize(varStr);

    for (unsigned i = 0; i < tokens.size(); i += 3) {
        std::string varName = tokens[i];
        std::string varValue = tokens[i + 2];
        

        if constexpr (std::is_same_v<T, std::complex<long double>>) { // TODO: Уметь обрабатывать неадыкватные значения по типу I2314 в качестве комплексного
            if (i + 3 < tokens.size() && tokens[i + 3][0] == '+') {
                std::string varPlusValue = tokens[i + 4];
                i += 2;
                
                if (varPlusValue.back() == 'I') {
                    varPlusValue.pop_back();
                    varMap[varName] = std::complex<long double>(std::stold(varValue), std::stold(varPlusValue));
                }
                else if (varValue.back() == 'I') {
                    varValue.pop_back();
                    varMap[varName] = std::complex<long double>(std::stold(varPlusValue), std::stold(varValue));
                }
                
            }
            else {
                if (varValue.back() == 'I') {
                    varValue.pop_back();
                    varMap[varName] = std::complex<long double>(0.0, std::stold(varValue));
                }
                else {
                    varMap[varName] = std::complex<long double>(std::stold(varValue), 0.0);
                }
            }
        }
        else if constexpr (std::is_same_v<T, long double>) { // Если T — это long double, просто преобразуем строку в число
            varMap[varName] = std::stold(varValue);
        }
    }

    if (root)
        root = subsVarHelper(std::move(root), varMap);
}

template <typename T>
T Expression<T>::evaluate() const {
    if (!root) {
        throw std::runtime_error("Expression tree is empty");
    }
    return evaluateHelper(root.get());
}

// МЕТОДЫ УЗЛОВ AST
 
// template <typename T>
// void Expression<T>::NumberNode::print(int indent) const { // Узел для чисел: метод чисто для дебага — print
//     std::cout << std::string(indent, ' ') << "Number: " << value << "\n";
// }

// template <typename T>
// void Expression<T>::VariableNode::print(int indent) const { // Узел для переменных: метод чисто для дебага — print
//     std::cout << std::string(indent, ' ') << "Variable: " << name << "\n";
// }

// template <typename T>
// void Expression<T>::BinaryOperationNode::print(int indent) const { // Узел для бинарных операций: метод чисто для дебага — print
//     std::cout << std::string(indent, ' ') << "Operation: " << operation << "\n";
//     left->print(indent + 2);
//     right->print(indent + 2);
// }

// template <typename T>
// void Expression<T>::FunctionNode::print(int indent) const { // Узел для функций: метод чисто для дебага — print
//     std::cout << std::string(indent, ' ') << "Function: " << function << "\n";
//     arg->print(indent + 2);
// }

template <typename T>
std::string Expression<T>::NumberNode::toString() const { // Узел для чисел: для конструирования строки из Expression
    std::string str_value;

    if constexpr (std::is_same_v<T, long double>) {
        str_value = std::to_string(value);
    }
    else {
        if (value.real()) str_value = std::to_string(value.real());
        else str_value = std::to_string(value.imag());
    }

    // Уберу ведущие незначащие нули
    int n = 0;
    for (int i = 0; str_value[i] == '0' && str_value[i + 1] != '.'; ++i) ++n;
    str_value.erase(0, n);

    // Уберу конечные незначащие нули
    n = 0;
    for (unsigned i = str_value.size() - 1; str_value[i] == '0' || str_value[i] == '.'; --i) {
        ++n;
        if (str_value[i] == '.') break;
    }
    str_value.erase(str_value.size() - n);

    if constexpr (std::is_same_v<T, long double>) {
        return str_value;
    }
    else {
        if (value.real()) return str_value;
        else return str_value + 'I';
    }
}

template <typename T>
std::string Expression<T>::VariableNode::toString() const { // Узел для переменных: для конструирования строки из Expression
    return name;
}

template <typename T>
std::string Expression<T>::BinaryOperationNode::toString() const { // Узел для бинарных операций: для конструирования строки из Expression
    std::string leftStr = left->toString();
    std::string rightStr = right->toString();
    
    if (operation == '^')
        return "(" + leftStr + "^" + rightStr + ")";
    else
        return "(" + leftStr + " " + operation + " " + rightStr + ")";
}

template <typename T>
std::string Expression<T>::FunctionNode::toString() const { // Узел для функций: для конструирования строки из Expression
    return function + "(" + arg->toString() + ")";
}

// ОПЕРАТОРЫ ДЛЯ ТИПА EXPRESSION

template <typename T>
Expression<T> Expression<T>::operator+(const Expression<T>& rhs) {
    Expression<T> thatone = rhs;
    Expression<T> thisone = *this;
    Expression<T> result;
    result.root = std::make_unique<BinaryOperationNode>('+', std::move(thisone.root), std::move(thatone.root));
    return result; 
}

template <typename T>
Expression<T> Expression<T>::operator-(const Expression<T>& rhs) {
    Expression<T> thatone = rhs;
    Expression<T> thisone = *this;
    Expression<T> result;
    result.root = std::make_unique<BinaryOperationNode>('-', std::move(thisone.root), std::move(thatone.root));
    return result;  
}

template <typename T>
Expression<T> Expression<T>::operator*(const Expression<T>& rhs) {
    Expression<T> thatone = rhs;
    Expression<T> thisone = *this;
    Expression<T> result;
    result.root = std::make_unique<BinaryOperationNode>('*', std::move(thisone.root), std::move(thatone.root));
    return result; 
}

template <typename T>
Expression<T> Expression<T>::operator/(const Expression<T>& rhs) {
    Expression<T> thatone = rhs;
    Expression<T> thisone = *this;
    Expression<T> result;
    result.root = std::make_unique<BinaryOperationNode>('/', std::move(thisone.root), std::move(thatone.root));
    return result; 
}

template <typename T>
Expression<T> Expression<T>::operator^(const Expression<T>& rhs) {
    Expression<T> thatone = rhs;
    Expression<T> thisone = *this;
    Expression<T> result;
    result.root = std::make_unique<BinaryOperationNode>('^', std::move(thisone.root), std::move(thatone.root));
    return result; 
}

template <typename T>
Expression<T>& Expression<T>::operator=(const Expression<T>& other) { // Оператор присваивания
    if (this != &other) { // Проверяем на самоприсваивание
        if (other.root) {
            root = copyNode(other.root.get());
        } else {
            root.reset();
        }
    }
    return *this;
}

// ФУНКЦИИ ДЛЯ ПАРСИНГА

template <typename T>
std::vector<std::string> Expression<T>::tokenize(const std::string& expr) { // Токенизация выражения для последующего парсинга
    std::vector<std::string> tokens;
    
    for (unsigned i = 0; i < expr.size(); ++i) {
        if (std::isspace(expr[i])) continue;

        if (std::isdigit(expr[i]) || expr[i] == '.' ||
            (expr[i] == 'I' && (i + 1 < expr.size() || !std::isalnum(expr[i+1])))) { // Число TODO: Переписать более скурпулезные условия для отличия компл. числа от переменной на I
            std::string num;
            while (i < expr.size() && 
                   (std::isdigit(expr[i]) || expr[i] == '.' || expr[i] == 'I')) {
                num += expr[i++];
            }
            tokens.push_back(num);
            --i; // Возвращаемся на один символ назад
        } 
        else if (std::isalpha(expr[i])) { // Переменная или функция
            std::string name;
            while (i < expr.size() && std::isalnum(expr[i])) {
                name += std::tolower(expr[i++]);
            }
            tokens.push_back(name);
            --i; // Возвращаемся на один символ назад
        } 
        else
            tokens.push_back(std::string(1, expr[i])); // Операторы, "()" или "=" (для функции замены переменных на значения)
    
    }
    return tokens;
}

// // ПАРСИНГ МАТЕМАТИКИ В СООТВЕТСВИИ С PEMDAS:

template <typename T>
std::unique_ptr<typename Expression<T>::Node> Expression<T>::parseExpression(const std::vector<std::string>& tokens, size_t& pos) { // Парсинг выражения AS
    auto left = parseTerm(tokens, pos);
    while (pos < tokens.size() && (tokens[pos] == "+" || tokens[pos] == "-")) {
        char op = tokens[pos++][0];
        auto right = parseTerm(tokens, pos);
        left = std::make_unique<BinaryOperationNode>(op, std::move(left), std::move(right));
    }
    return left;
}

template <typename T>
std::unique_ptr<typename Expression<T>::Node> Expression<T>::parseTerm(const std::vector<std::string>& tokens, size_t& pos) { // Парсинг терма MD
    auto left = parseExponent(tokens, pos);
    while (pos < tokens.size() && (tokens[pos] == "*" || tokens[pos] == "/")) {
        char op = tokens[pos++][0];
        auto right = parseExponent(tokens, pos);
        left = std::make_unique<BinaryOperationNode>(op, std::move(left), std::move(right));
    }
    return left;
}

template <typename T>
std::unique_ptr<typename Expression<T>::Node> Expression<T>::parseExponent(const std::vector<std::string>& tokens, size_t& pos) { // Парсинг возведения в степень E
    auto left = parseFactor(tokens, pos);
    while (pos < tokens.size() && tokens[pos] == "^") {
        ++pos; // Пропускаем "^"
        auto right = parseFactor(tokens, pos); 
        left = std::make_unique<BinaryOperationNode>('^', std::move(left), std::move(right));
    }

    return left;
}

template <typename T>
std::unique_ptr<typename Expression<T>::Node> Expression<T>::parseFactor(const std::vector<std::string>& tokens, size_t& pos) { // Парсинг фактора и атомарных элементов
    if (pos >= tokens.size()) {
        throw std::runtime_error("Unexpected end of expression");
    }

    if (tokens[pos] == "(") {
        ++pos; // Пропускаем "("
        auto node = parseExpression(tokens, pos);
        if (pos >= tokens.size() || tokens[pos] != ")") {
            throw std::runtime_error("Expected ')'");
        }
        ++pos; // Пропускаем ")"
        return node;
    }

    if (std::isdigit(tokens[pos][0]) || tokens[pos][0] == '.' || tokens[pos][0] == 'I') {
        return parseNumber(tokens, pos);
    }

    if (std::isalpha(tokens[pos][0])) {
        if (pos + 1 < tokens.size() && tokens[pos + 1] == "(") 
            return parseFunction(tokens, pos);
        else 
            return parseVariable(tokens, pos);
    }

    throw std::runtime_error("Unexpected token: " + tokens[pos]);
}

// // ПАРСИНГ АТОМАРНЫХ ЭЛЕМЕНТОВ: 

template <typename T>
std::unique_ptr<typename Expression<T>::Node> Expression<T>::parseNumber(const std::vector<std::string>& tokens, size_t& pos) { // Парсинг числа
    
    std::string number = tokens[pos++];
    size_t pos_I = number.find('I');
    std::complex<long double> value;
    if (pos_I == std::string::npos) {
        value = {std::stold(number), 0};
    }
    else {
        long double left = std::stold(number.substr(0, pos_I));
        long double right = 1.0;
        if (!(number.substr(pos_I + 1)).empty()) 
            right = std::stold(number.substr(pos_I + 1));
        value = {0, left * right};
    }

    if constexpr (std::is_same_v<T, long double>) 
        return std::make_unique<NumberNode>(value.real());
    else 
        return std::make_unique<NumberNode>(value);
}

template <typename T>
std::unique_ptr<typename Expression<T>::Node> Expression<T>::parseVariable(const std::vector<std::string>& tokens, size_t& pos) { // Парсинг переменной
    std::string name = tokens[pos++];
    return std::make_unique<VariableNode>(name);
}

template <typename T>
std::unique_ptr<typename Expression<T>::Node> Expression<T>::parseFunction(const std::vector<std::string>& tokens, size_t& pos) { // Парсинг функции
    std::string function = tokens[pos++];
    std::unordered_set<std::string> FUNCS = {"sin", "cos", "ln", "exp"};

    if (FUNCS.find(function) == FUNCS.end()) {
        throw std::runtime_error("Unknown function identifier");
    }
    ++pos; // Пропускаем "("
    auto arg = parseExpression(tokens, pos);
    if (pos >= tokens.size() || tokens[pos] != ")") {
        throw std::runtime_error("Expected ')'");
    }
    ++pos; // Пропускаем ")"
    return std::make_unique<FunctionNode>(function, std::move(arg));
}

// ДРУГИЕ ФУНКЦИИ

template <typename T>
std::unique_ptr<typename Expression<T>::Node> Expression<T>::subsVarHelper(
    std::unique_ptr<Node> node,
    const std::unordered_map<std::string, T>& varMap) { // Основное тело функции subsVar() для замены переменной в узле
    if (!node) return nullptr;

    if (auto* varNode = dynamic_cast<VariableNode*>(node.get())) { // Узел переменной?
        auto it = varMap.find(varNode->name);
        if (it != varMap.end()) { 
            if constexpr (std::is_same_v<T, std::complex<long double>>) {
                std::complex<long double> value = it->second;
    
                if (value.real() != 0 && value.imag() != 0) { // Если обе части ненулевые, заменяем узел на узел операции сложения с двумя детьми
                    auto realNode = std::make_unique<NumberNode>(std::complex<long double>(value.real(), 0));
                    auto imagNode = std::make_unique<NumberNode>(std::complex<long double>(0, value.imag()));
                    auto plusOpNode = std::make_unique<BinaryOperationNode>('+', std::move(realNode), std::move(imagNode));
                    return std::move(plusOpNode);
                } 
                else if (value.real()) { // Если только реальная часть ненулевая, просто заменяем значение на реальное
                    return std::make_unique<NumberNode>(std::complex<long double>(value.real(), 0));
                } 
                else { // Если только мнимая часть ненулевая, просто заменяем значение на мнимое
                    return std::make_unique<NumberNode>(std::complex<long double>(0, value.imag()));
                }
            } 
            else {
                return std::make_unique<NumberNode>(it->second);
            }
        }
    } 
    else if (auto* binOpNode = dynamic_cast<BinaryOperationNode*>(node.get())) { // Узел бинарной операции?
        binOpNode->left = subsVarHelper(std::move(binOpNode->left), varMap);
        binOpNode->right = subsVarHelper(std::move(binOpNode->right), varMap);
    } 
    else if (auto* funcNode = dynamic_cast<FunctionNode*>(node.get())) { // Узел функции?
        funcNode->arg = subsVarHelper(std::move(funcNode->arg), varMap);
    }

    return node;
}

template <typename T>
T Expression<T>::evaluateHelper(const Node* node) const {
    if (const auto* numNode = dynamic_cast<const NumberNode*>(node)) {
        return numNode->value;
    }
    else if (const auto* operatorNode = dynamic_cast<const BinaryOperationNode*>(node)) {
        T leftValue = evaluateHelper(operatorNode->left.get());
        T rightValue = evaluateHelper(operatorNode->right.get());
        
        switch (operatorNode->operation) {
            case '+': return leftValue + rightValue;
            case '-': return leftValue - rightValue;
            case '*': return leftValue * rightValue;
            case '/': 
                if (rightValue == static_cast<T>(0)) {
                    throw std::runtime_error("Division by zero");
                }
                return leftValue / rightValue;
            case '^': return std::pow(leftValue, rightValue);
            default: throw std::runtime_error("Unknown binary operator");
        }
    }
    else if (const auto* funcNode = dynamic_cast<const FunctionNode*>(node)) {
        T argValue = evaluateHelper(funcNode->arg.get());

        if (funcNode->function == "sin") {
            return std::sin(argValue);
        } else if (funcNode->function == "cos") {
            return std::cos(argValue);
        } else if (funcNode->function == "ln") {
            return std::log(argValue);
        } else if (funcNode->function == "exp") {
            return std::exp(argValue);
        } else {
            throw std::runtime_error("Unknown function: " + funcNode->function);
        }
    }
    throw std::runtime_error("Invalid node type in evaluation");
}

template <typename T>
std::unique_ptr<typename Expression<T>::Node> Expression<T>::copyNode(const Node* node) const {
    if (!node) return nullptr;

    if (auto* numNode = dynamic_cast<const NumberNode*>(node)) {
        return std::make_unique<NumberNode>(numNode->value);
    } 
    else if (auto* varNode = dynamic_cast<const VariableNode*>(node)) {
        return std::make_unique<VariableNode>(varNode->name);
    }
    else if (auto* binOpNode = dynamic_cast<const BinaryOperationNode*>(node)) {
        auto left = copyNode(binOpNode->left.get());
        auto right = copyNode(binOpNode->right.get());
        return std::make_unique<BinaryOperationNode>(binOpNode->operation, std::move(left), std::move(right));
    }
    else if (auto* funcNode = dynamic_cast<const FunctionNode*>(node)) {
        auto arg = copyNode(funcNode->arg.get());
        return std::make_unique<FunctionNode>(funcNode->function, std::move(arg));
    }

    throw std::runtime_error("Unknown node type in copy");
}

// Я понимаю, что это плохая практика, но мне лень че-то копировать и вставлять куда-то.
template class Expression<long double>;
template class Expression<std::complex<long double>>;