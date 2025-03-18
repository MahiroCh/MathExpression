#include "Expression.hpp"

// ---------------------------------------------------------------------------------------------------- //
// КОНСТРУКТОРЫ И ДЕСТРУКТОРЫ
// ---------------------------------------------------------------------------------------------------- //

/*
Конструктор для объявления переменной без выражения.
*/
template <typename T>
Expression<T>::Expression() : root{nullptr} {}

// --------------------------------------------------------------- //

/*
Конструктор выражения из строки.
*/
template <typename T>
Expression<T>::Expression(const char* arg) { 

    size_t pos = 0;
    std::vector<std::string> tokens = tokenize(arg);
    root = parseExpression(tokens, pos);
    if (pos != tokens.size())
        throw std::runtime_error("Invalid expression");
}

// --------------------------------------------------------------- //

/*
Конструктор выражения из числа.
*/
template <typename T>
Expression<T>::Expression(const T &arg) {
    
    size_t pos = 0;
    std::vector<std::string> tokens;

    if constexpr (std::is_same_v<T, std::complex<long double>>) {
        std::string number = numToString(arg.real()) + '+' + numToString(arg.imag()) + 'I';
        tokens = tokenize(number);
    }
    else {
        tokens = tokenize(numToString(arg));
    }
    
    root = parseExpression(tokens, pos);
    if (pos != tokens.size())
        throw std::runtime_error("Invalid expression");
}

// --------------------------------------------------------------- //

/*
Конструктор копирования.
*/
template <typename T>
Expression<T>::Expression(const Expression<T>& other) {

    this->root = copyTree(other.root.get());
}

// --------------------------------------------------------------- //

/*
Конструктор перемещения.
*/
template <typename T>
Expression<T>::Expression(Expression<T>&& other) noexcept : root(std::move(other.root)) {}





















// ---------------------------------------------------------------------------------------------------- //
// ПОЛЬЗОВАТЕЛЬСКИЕ МЕТОДЫ
// ---------------------------------------------------------------------------------------------------- //

/*
Выражение в строку.
*/
template <typename T>
std::string Expression<T>::toString() const { 

    if (root) return root->nodeToString();
    return "";
}

// --------------------------------------------------------------- //

/*
Замена переменных.
*/
template <typename T>
void Expression<T>::subsVar(const std::string& varStr) { 

    std::unordered_map<std::string, T> varMap;
    std::vector<std::string> tokens = tokenize(varStr);
    
    for (size_t i = 1; i < tokens.size(); i++) {

        if (tokens[i] == "=") {

            std::string varName = tokens[i - 1];
            std::complex<long double> varValue(0,0);
            bool sign = false;
            i++;

            for (; i < tokens.size() && 
                (!std::isalpha(tokens[i][0]) || tokens[i][0] == 'I'); 
                i++) {

                if (tokens[i] == "-") sign = true;
                else if (tokens[i] == "+") sign = false;
                else if (tokens[i] == "*") continue;
                else {

                    if (sign)
                        varValue -= interpretComplex(tokens[i]);
                    else 
                        varValue += interpretComplex(tokens[i]);
                }
            }

            if constexpr (std::is_same_v<T, std::complex<long double>>)
                varMap[varName] = varValue;
            else
                varMap[varName] = varValue.real();
            
            i--;
        }
    }
    if (root) 
        root = subsVarHelper(std::move(root), varMap);
}



// --------------------------------------------------------------- //

/*
Вычислить значение выражения.
*/
template <typename T>
T Expression<T>::evaluate() const {

    if (!root) {
        throw std::runtime_error("Expression tree is empty");
    }

    return evaluateHelper(root.get());
}

// --------------------------------------------------------------- //

/*
Продифференцировать по переменной.
*/
template <typename T>
Expression<T> Expression<T>::differentiate(const std::string& var) const {
    Expression<T> result;
    result.root = differentiateHelper(this->root.get(), var);
    return result;
}






















// ---------------------------------------------------------------------------------------------------- //
// МЕТОДЫ УЗЛОВ AST
// ---------------------------------------------------------------------------------------------------- //

/*
Узел с числом в строку.
*/
template <typename T>
std::string 
Expression<T>::NumberNode::nodeToString() const {

    std::string str_value;

    if constexpr (std::is_same_v<T, long double>) {
        str_value = numToString(value);
    }
    else {
        if (value.real()) 
            str_value = numToString(value.real());
        else 
            str_value = numToString(value.imag()) + 'I';
    }

    return str_value;
}

// --------------------------------------------------------------- //

/*
Узел с переменной в строку.
*/
template <typename T>
std::string Expression<T>::VariableNode::nodeToString() const {

    return name;
}

// --------------------------------------------------------------- //

/*
Узел с бин. операцией в строку.
*/
template <typename T>
std::string Expression<T>::BinaryOperationNode::nodeToString() const {

    std::string leftStr = left->nodeToString();
    std::string rightStr = right->nodeToString();
    
    if (operation == '^')
        return "(" + leftStr + "^" + rightStr + ")";
    else
        return "(" + leftStr + " " + operation + " " + rightStr + ")";
}

// --------------------------------------------------------------- //

/*
Узел с унарной операцией в строку.
*/
template <typename T>
std::string Expression<T>::UnaryOperationNode::nodeToString() const {

    return "(" + std::string(1, operation) + arg->nodeToString() + ")";
}

// --------------------------------------------------------------- //

/*
Узел с функцией в строку.
*/
template <typename T>
std::string Expression<T>::FunctionNode::nodeToString() const {

    return function + "(" + arg->nodeToString() + ")";
}




















// ---------------------------------------------------------------------------------------------------- //
// ПЕРЕГРУЗКИ ОПЕРАТОРОВ ДЛЯ EXPRESSION
// ---------------------------------------------------------------------------------------------------- //

std::ostream& operator<<(std::ostream& os, const std::complex<long double>& c) {

    char beautify_with_space = '\0';
    if (c.real() && c.imag()) beautify_with_space = ' ';
    
    if (!c.real() && !c.imag()) 
        return os << 0;
    
    if (c.real())
        os << c.real();  
    
    if (c.imag() > 0) {
        if (c.real()) os << beautify_with_space << "+" << beautify_with_space;
        os << c.imag() << 'I';  
    }
    else if (c.imag() < 0)
        os << beautify_with_space << "-" << beautify_with_space << -c.imag() << "I"; 

    return os;
}


// --------------------------------------------------------------- //

template <typename T>
Expression<T> Expression<T>::operator+(const Expression<T>& other) {
    
    Expression<T> result;
    result.root = std::make_unique<BinaryOperationNode>(
        '+', copyTree(this->root.get()), copyTree(other.root.get()));
    return result; 
}

// --------------------------------------------------------------- //

template <typename T>
Expression<T> Expression<T>::operator-(const Expression<T>& other) {

    Expression<T> result;
    result.root = std::make_unique<BinaryOperationNode>
        ('-', copyTree(this->root.get()), copyTree(other.root.get()));
    return result; 
}

// --------------------------------------------------------------- //

template <typename T>
Expression<T> Expression<T>::operator*(const Expression<T>& other) {

    Expression<T> result;
    result.root = std::make_unique<BinaryOperationNode>
        ('*', copyTree(this->root.get()), copyTree(other.root.get()));
    return result; 
}

// --------------------------------------------------------------- //

template <typename T>
Expression<T> Expression<T>::operator/(const Expression<T>& other) {

    Expression<T> result;
    result.root = std::make_unique<BinaryOperationNode>
        ('/', copyTree(this->root.get()), copyTree(other.root.get()));
    return result; 
}

// --------------------------------------------------------------- //

template <typename T>
Expression<T> Expression<T>::operator^(const Expression<T>& other) {

    Expression<T> result;
    result.root = std::make_unique<BinaryOperationNode>
        ('^', copyTree(this->root.get()), copyTree(other.root.get()));
    return result; 
}

// --------------------------------------------------------------- //

template <typename T>
Expression<T>& Expression<T>::operator=(const Expression<T>& other) { // Оператор копирования.

    if (this != &other) {

        if (other.root)
            root = copyTree(other.root.get());
        else 
            root.reset(nullptr);
    }
    
    return *this;
}

// --------------------------------------------------------------- //

template <typename T>
Expression<T>& Expression<T>::operator=(Expression<T>&& other) noexcept { // Оператор перемещения.

    if (this != &other) {
        root = std::move(other.root);
    }
    return *this;
}




















// ---------------------------------------------------------------------------------------------------- //
// НУЛЕВОЙ ЭТАП ПАРСИНГА СТРОКИ В ВЫРАЖЕНИЕ (ХРАНЕНИЕ В AST-ДЕРЕВЕ)
// ---------------------------------------------------------------------------------------------------- //

/*
Токенизация выражения для последующего парсинга.
*/
template <typename T>
std::vector<std::string> Expression<T>::tokenize(const std::string& expr) { 

    std::vector<std::string> tokens;
    
    for (unsigned i = 0; i < expr.size(); ++i) {

        if (std::isspace(expr[i])) continue;

        if (std::isdigit(expr[i]) || 
            expr[i] == '.' ||
            (expr[i] == 'I' && 
            (i + 1 < expr.size() || 
             !std::isalnum(expr[i+1])))) { 

            std::string num;

            while (i < expr.size() && 
                  (std::isdigit(expr[i]) || 
                   expr[i] == '.' || 
                   expr[i] == 'I')) {
                num += expr[i++];
            }

            tokens.push_back(num);
            --i;
        } 
        else if (std::isalpha(expr[i])) { // Переменная или функция

            if (!tokens.empty() && std::isdigit(tokens.back()[0])) 
                tokens.push_back("*");

            std::string name;

            while (i < expr.size() && std::isalnum(expr[i])) 
                name += std::tolower(expr[i++]);

            tokens.push_back(name);
            --i; 
        } 
        else {
            tokens.push_back(std::string(1, expr[i])); // Операторы, "()" или "=" 
                                                       // (для функции замены переменных на значения)
        }
    }
    
    return tokens;
}


















// ---------------------------------------------------------------------------------------------------- //
// ПЕРВЫЙ ЭТАП ПАРСИНГА СТРОКИ В ВЫРАЖЕНИЕ (НА УРОВНЕ ОПЕРАЦИЙ В СООТВЕТСВИИ С PEMDAS)
// ---------------------------------------------------------------------------------------------------- //

/*
Сложение, вычитание.
*/
template <typename T>
std::unique_ptr<typename Expression<T>::Node> 
Expression<T>::parseExpression(const std::vector<std::string>& tokens, size_t& pos) {

    auto left = parseTerm(tokens, pos);
    while (pos < tokens.size() && (tokens[pos] == "+" || tokens[pos] == "-")) {

        char op = tokens[pos++][0];
        auto right = parseTerm(tokens, pos);
        left = std::make_unique<BinaryOperationNode>
            (op, std::move(left), std::move(right));
    }

    return left;
}

// --------------------------------------------------------------- //

/*
Умножение, деление.
*/
template <typename T>
std::unique_ptr<typename Expression<T>::Node> 
Expression<T>::parseTerm(const std::vector<std::string>& tokens, size_t& pos) {

    auto left = parseExponent(tokens, pos);
    while (pos < tokens.size() && (tokens[pos] == "*" || tokens[pos] == "/")) {

        char op = tokens[pos++][0];
        auto right = parseExponent(tokens, pos);
        left = std::make_unique<BinaryOperationNode>
            (op, std::move(left), std::move(right));
    }

    return left;
}

// --------------------------------------------------------------- //

/*
Степень.
*/
template <typename T>
std::unique_ptr<typename Expression<T>::Node> 
Expression<T>::parseExponent(const std::vector<std::string>& tokens, size_t& pos) {

    auto left = parseFactor(tokens, pos);
    while (pos < tokens.size() && tokens[pos] == "^") {

        ++pos; // Пропускаем "^"
        auto right = parseFactor(tokens, pos); 
        left = std::make_unique<BinaryOperationNode>
            ('^', std::move(left), std::move(right));
    }

    return left;
}

// --------------------------------------------------------------- //

/*
Скобки.
*/
template <typename T>
std::unique_ptr<typename Expression<T>::Node> 
Expression<T>::parseFactor(const std::vector<std::string>& tokens, size_t& pos) { // Парсинг фактора 
                                                                                  // и атомарных элементов

    if (pos >= tokens.size()) {
        throw std::runtime_error("Unexpected end of expression");
    }

    if (tokens[pos] == "-") { 
        ++pos; 
        auto operand = parseFactor(tokens, pos); 
        return std::make_unique<UnaryOperationNode>('-', std::move(operand));
    }

    if (tokens[pos] == "(") {

        ++pos; 
        auto node = parseExpression(tokens, pos);
        if (pos >= tokens.size() || tokens[pos] != ")") {
            throw std::runtime_error("Expected ')'");
        }
        ++pos; 
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



















// ---------------------------------------------------------------------------------------------------- //
// ВТОРОЙ ЭТАП ПАРСИНГА СТРОКИ В ВЫРАЖЕНИЕ (НА УРОВНЕ АТОМАРНЫХ ЭЛЕМЕНТОВ)
// ---------------------------------------------------------------------------------------------------- //

/*
Числа.
*/
template <typename T>
std::unique_ptr<typename Expression<T>::Node> 
Expression<T>::parseNumber(const std::vector<std::string>& tokens, size_t& pos) { // Парсинг числа
    
    std::complex<long double> value = interpretComplex(tokens[pos++]);

    if constexpr (std::is_same_v<T, long double>) 
        return std::make_unique<NumberNode>(value.real());
    else 
        return std::make_unique<NumberNode>(value);
}

// --------------------------------------------------------------- //

/*
Переменные.
*/
template <typename T>
std::unique_ptr<typename Expression<T>::Node> 
Expression<T>::parseVariable(const std::vector<std::string>& tokens, size_t& pos) { // Парсинг переменной

    std::string name = tokens[pos++];
    return std::make_unique<VariableNode>(name);
}

// --------------------------------------------------------------- //

/*
Функции.
*/
template <typename T>
std::unique_ptr<typename Expression<T>::Node> 
Expression<T>::parseFunction(const std::vector<std::string>& tokens, size_t& pos) { // Парсинг функции

    std::string function = tokens[pos++];
    std::unordered_set<std::string> FUNCS = {"sin", "cos", "ln", "exp"};

    if (FUNCS.find(function) == FUNCS.end())
        throw std::runtime_error("Unknown function identifier");

    ++pos; // Пропускаем "("
    auto arg = parseExpression(tokens, pos);

    if (pos >= tokens.size() || tokens[pos] != ")")
        throw std::runtime_error("Expected ')'");

    ++pos; // Пропускаем ")"
    return std::make_unique<FunctionNode>(function, std::move(arg));
}



























// ---------------------------------------------------------------------------------------------------- //
// ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ
// ---------------------------------------------------------------------------------------------------- //

/*
Тело функции замены переменных.
*/
template <typename T>
std::unique_ptr<typename Expression<T>::Node> 
Expression<T>::subsVarHelper(std::unique_ptr<Node> node,
                             const std::unordered_map<std::string, T>& varMap) { // Основное тело 
                                                                                 // функции subsVar() 
                                                                                 // для замены переменной в узле
    
    if (!node) return nullptr;

    if (auto* varNode = dynamic_cast<VariableNode*>(node.get())) { // Узел переменной?

        auto it = varMap.find(varNode->name);
        
        if (it != varMap.end()) { 

            if constexpr (std::is_same_v<T, std::complex<long double>>) {

                std::complex<long double> value = it->second;
    
                if (value.real() != 0 && value.imag() != 0) { // Если обе части ненулевые, раздваиваем узел.

                    std::unique_ptr<Node> realNode, imagNode;

                    if (value.real() < 0) 
                        realNode = std::make_unique<UnaryOperationNode>
                        ('-', std::make_unique<NumberNode>
                            (std::complex<long double>(-value.real(), 0)));
                    else
                        realNode = std::make_unique<NumberNode>
                            (std::complex<long double>(value.real(), 0));
                    
                    if (value.imag() < 0) 
                        imagNode = std::make_unique<UnaryOperationNode>
                            ('-', std::make_unique<NumberNode>
                                (std::complex<long double>(0, -value.imag())));
                    else
                        imagNode = std::make_unique<NumberNode>
                            (std::complex<long double>(0, value.imag()));

                    return std::make_unique<BinaryOperationNode>
                        ('+', std::move(realNode), std::move(imagNode));
                } 
                else if (value.real()) { // Если только реальная часть ненулевая, заменяем значение на реальное.

                    if (value.real() < 0) 
                        return std::make_unique<UnaryOperationNode>
                            ('-', std::make_unique<NumberNode>
                                (std::complex<long double>(-value.real(), 0)));
                    else
                        return std::make_unique<NumberNode>
                            (std::complex<long double>(value.real(), 0));
                } 
                else { // Если только мнимая часть ненулевая, заменяем значение на мнимое.

                    if (value.imag() < 0) 
                        return std::make_unique<UnaryOperationNode>
                            ('-', std::make_unique<NumberNode>
                                (std::complex<long double>(0, -value.imag())));
                    else
                        return std::make_unique<NumberNode>
                            (std::complex<long double>(0, value.imag()));
                }
            } 
            else if constexpr (std::is_same_v<T, long double>) {

                if (it->second >= 0)
                    return std::make_unique<NumberNode>(it->second);
                else
                    return std::make_unique<UnaryOperationNode>
                        ('-', std::make_unique<NumberNode>(-it->second));
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
    else if (auto* unaryOpNode = dynamic_cast<UnaryOperationNode*>(node.get())) { // Узел унарной операции?
        unaryOpNode->arg = subsVarHelper(std::move(unaryOpNode->arg), varMap);
    }

    return node;
}

// --------------------------------------------------------------- //

/*
Тело функции для вычисления выражения.
*/
template <typename T>
T Expression<T>::evaluateHelper(const Node* node) const {

    if (const auto* numNode = dynamic_cast<const NumberNode*>(node)) {
        return numNode->value;
    }
    else if (const auto* binOpNode = dynamic_cast<const BinaryOperationNode*>(node)) {

        T leftValue = evaluateHelper(binOpNode->left.get());
        T rightValue = evaluateHelper(binOpNode->right.get());
        
        switch (binOpNode->operation) {

            case '+': return leftValue + rightValue;

            case '-': return leftValue - rightValue;

            case '*': return leftValue * rightValue;

            case '/': 
                if (rightValue == static_cast<T>(0))
                    throw std::runtime_error("Division by zero");
                return leftValue / rightValue;

            case '^': 
                if constexpr (std::is_same_v<T, long double>) {
                    long double intPart;
                    if (std::abs(rightValue) < 1 && std::modf(1 / std::abs(rightValue), &intPart) == 0.0L && (int)(1 / std::abs(rightValue)) % 2 == 0 && leftValue < 0)
                        throw std::runtime_error("Argument of sqrt < 0 and even sqrt power is not allowed");
                }
                return std::pow(leftValue, rightValue);

            default: throw std::runtime_error("Unknown binary operator");
        }
    }
    else if (const auto* funcNode = dynamic_cast<const FunctionNode*>(node)) {

        T argValue = evaluateHelper(funcNode->arg.get());

        if (funcNode->function == "sin")
            return std::sin(argValue);

        else if (funcNode->function == "cos")
            return std::cos(argValue);

        else if (funcNode->function == "ln") {
            if (argValue == static_cast<T>(0)) 
                throw std::runtime_error("Argument of ln <= 0 is not allowed");
            if constexpr (std::is_same_v<T, long double>)  {
                if (argValue <= 0.0) 
                    throw std::runtime_error("Argument of ln <= 0 is not allowed");
            }
            return std::log(argValue);
        }
        else if (funcNode->function == "exp") 
            return std::exp(argValue);

        else
            throw std::runtime_error("Unknown function: " + funcNode->function);
    
    }
    else if (const auto* unaryOpNode = dynamic_cast<const UnaryOperationNode*>(node)) {

        T argValue = evaluateHelper(unaryOpNode->arg.get());

        switch (unaryOpNode->operation) {
            case '-': return -argValue;
            default:
                throw std::runtime_error("Unknown unary operator");
        }
    }

    throw std::runtime_error("Invalid node type in evaluation");
}

// --------------------------------------------------------------- //

/*
Тело функции дифференцирования.
*/
template <typename T>
std::unique_ptr<typename Expression<T>::Node> 
Expression<T>::differentiateHelper(const Node* node, const std::string& var) const {
    if (!node) return nullptr;

    if (auto* numNode = dynamic_cast<const NumberNode*>(node)) { // Производная числа равна 0.
        return std::make_unique<NumberNode>(0);
    }
    else if (auto* varNode = dynamic_cast<const VariableNode*>(node)) { // Производная переменной: 
                                                                        // 1, если это та переменная, 
                                                                        // по которой дифференцируем, иначе 0.
        return std::make_unique<NumberNode>(varNode->name == var ? 1 : 0);
    }
    else if (auto* binOpNode = dynamic_cast<const BinaryOperationNode*>(node)) { // Производная для 
                                                                                 // бинарных операций.
        
        auto dLeft = differentiateHelper(binOpNode->left.get(), var);
        auto dRight = differentiateHelper(binOpNode->right.get(), var);

        switch (binOpNode->operation) {
            case '+': // (f + g)' = f' + g'
                return std::make_unique<BinaryOperationNode>
                    ('+', std::move(dLeft), std::move(dRight));
            case '-': // (f - g)' = f' - g'
                return std::make_unique<BinaryOperationNode>
                    ('-', std::move(dLeft), std::move(dRight));
            case '*': { // (f * g)' = f' * g + f * g'
                auto leftCopy = copyTree(binOpNode->left.get());
                auto rightCopy = copyTree(binOpNode->right.get());
                return std::make_unique<BinaryOperationNode>
                    ('+', std::make_unique<BinaryOperationNode>
                        ('*', std::move(dLeft), std::move(rightCopy)),
                    std::make_unique<BinaryOperationNode>
                        ('*', std::move(leftCopy), std::move(dRight))
                );
            }
            case '/': { // (f / g)' = (f' * g - f * g') / g^2
                auto leftCopy = copyTree(binOpNode->left.get());
                auto rightCopy = copyTree(binOpNode->right.get());
                auto rightSquared = std::make_unique<BinaryOperationNode>
                    ('^', copyTree(binOpNode->right.get()), std::make_unique<NumberNode>(2));
                return std::make_unique<BinaryOperationNode>
                    ('/', std::make_unique<BinaryOperationNode>
                        ('-', std::make_unique<BinaryOperationNode>
                            ('*', std::move(dLeft), std::move(rightCopy)), std::make_unique<BinaryOperationNode>
                                                                                ('*', std::move(leftCopy), std::move(dRight))
                    ),
                    std::move(rightSquared)
                );
            }
            case '^': {
                // (f^g)' = f^g * (g' * ln(f) + g * f' / f)
                auto leftCopy = copyTree(binOpNode->left.get());
                auto rightCopy = copyTree(binOpNode->right.get());
                auto lnLeft = std::make_unique<FunctionNode>("ln", copyTree(binOpNode->left.get()));
                auto term1 = std::make_unique<BinaryOperationNode>
                    ('*', std::move(dRight), std::move(lnLeft));
                auto term2 = std::make_unique<BinaryOperationNode>
                    ('*', std::move(rightCopy), std::make_unique<BinaryOperationNode>
                        ('/', std::move(dLeft), std::move(leftCopy)));
                auto sum = std::make_unique<BinaryOperationNode>
                    ('+', std::move(term1), std::move(term2));
                return std::make_unique<BinaryOperationNode>
                    ('*', copyTree(node), std::move(sum));
            }
            default:
                throw std::runtime_error("Unknown binary operator");
        }
    }
    else if (auto* funcNode = dynamic_cast<const FunctionNode*>(node)) { // Производная для функций.
        
        auto dArg = differentiateHelper(funcNode->arg.get(), var);
        if (funcNode->function == "sin") { // (sin(f))' = cos(f) * f'
            auto cosArg = std::make_unique<FunctionNode>
                ("cos", copyTree(funcNode->arg.get()));
            return std::make_unique<BinaryOperationNode>
                ('*', std::move(cosArg), std::move(dArg));
        }
        else if (funcNode->function == "cos") {  // (cos(f))' = -sin(f) * f'
           
            auto sinArg = std::make_unique<FunctionNode>
                ("sin", copyTree(funcNode->arg.get()));
            auto negSinArg = std::make_unique<BinaryOperationNode>
                ('*', std::make_unique<NumberNode>(-1), std::move(sinArg));
            return std::make_unique<BinaryOperationNode>
                ('*', std::move(negSinArg), std::move(dArg));
        }
        else if (funcNode->function == "ln") { // (ln(f))' = f' / f
            return std::make_unique<BinaryOperationNode>
                ('/', std::move(dArg), copyTree(funcNode->arg.get()));
        }
        else if (funcNode->function == "exp") { // (exp(f))' = exp(f) * f'
            auto expArg = std::make_unique<FunctionNode>
                ("exp", copyTree(funcNode->arg.get()));
            return std::make_unique<BinaryOperationNode>
                ('*', std::move(expArg), std::move(dArg));
        }
        else {
            throw std::runtime_error("Unknown function: " + funcNode->function);
        }
    }
    else if (auto* unaryOpNode = dynamic_cast<const UnaryOperationNode*>(node)) {

        auto dArg = differentiateHelper(unaryOpNode->arg.get(), var);

        switch (unaryOpNode->operation) {
            case '-': { // (-f)' = -f'
                return std::make_unique<UnaryOperationNode>('-', std::move(dArg));
            }
            default:
                throw std::runtime_error("Unknown unary operator");
        }
    }

    throw std::runtime_error("Unknown node type in differentiation");
}

// --------------------------------------------------------------- //

/*
Копирование дерева.
*/
template <typename T>
std::unique_ptr<typename Expression<T>::Node> Expression<T>::copyTree(const Node* node) const {
    
    if (!node) return nullptr;

    if (auto* numNode = dynamic_cast<const NumberNode*>(node)) {
        return std::make_unique<NumberNode>(numNode->value);
    } 
    else if (auto* varNode = dynamic_cast<const VariableNode*>(node)) {
        return std::make_unique<VariableNode>(varNode->name);
    }
    else if (auto* binOpNode = dynamic_cast<const BinaryOperationNode*>(node)) {
        
        auto left = copyTree(binOpNode->left.get());
        auto right = copyTree(binOpNode->right.get());
        return std::make_unique<BinaryOperationNode>
            (binOpNode->operation, std::move(left), std::move(right));
    }
    else if (auto* funcNode = dynamic_cast<const FunctionNode*>(node)) {
        auto func_arg = copyTree(funcNode->arg.get());
        return std::make_unique<FunctionNode>
            (funcNode->function, std::move(func_arg));
    }
    else if (auto* unaryOpNode = dynamic_cast<const UnaryOperationNode*>(node)) {
        auto arg = copyTree(unaryOpNode->arg.get());
        return std::make_unique<UnaryOperationNode>
            (unaryOpNode->operation, std::move(arg));
    }

    throw std::runtime_error("Unknown node type in copy");
}

// --------------------------------------------------------------- //

/*
Конвертация числа в строку.
Одновременно обрезает все конечные и ведущие нули.
*/
template <typename T>
std::string Expression<T>::numToString(const long double& num) {

    std::ostringstream out;
    out << num; 
    return out.str();
}

/*
Конвертация одночлена в комплексное число.
*/
template <typename T>
std::complex<long double> Expression<T>::interpretComplex(const std::string& str) {

    size_t pos_I = str.find('I');
    std::complex<long double> value;

    if (pos_I == std::string::npos) {
        value = {std::stold(str), 0};
    }
    else {
;
        long double right = 1, left = 1;
        if (!(str.substr(pos_I + 1)).empty()) 
            right = std::stold(str.substr(pos_I + 1));
        if (!(str.substr(0, pos_I)).empty()) 
            left = std::stold(str.substr(0, pos_I));
        
        value = {0, left * right};
    }

    return value;
}











// ---------------------------------------------------------------------------------------------------- //
// ЯВНАЯ ИНСТАНТИЗАЦИЯ (ПОТОМУ ЧТО В ТЗ ПОПРОСИЛИ РАЗДЕЛЯТЬ НА .HPP И .CPP)
// ---------------------------------------------------------------------------------------------------- //

template class Expression<long double>;
template class Expression<std::complex<long double>>;






















// ---------------------------------------------------------------------------------------------------- //
// ДЛЯ ДЕБАГА (ВЫВОД АСТ-ДЕРЕВА)
// ---------------------------------------------------------------------------------------------------- //

template <typename T>
void Expression<T>::NumberNode::print(int indent) const { 
    std::cout << std::string(indent, ' ') << "Number: " << value << "\n";
}

template <typename T>
void Expression<T>::VariableNode::print(int indent) const { 
    std::cout << std::string(indent, ' ') << "Variable: " << name << "\n";
}

template <typename T>
void Expression<T>::BinaryOperationNode::print(int indent) const { 
    std::cout << std::string(indent, ' ') << "Operation: " << operation << "\n";
    left->print(indent + 2);
    right->print(indent + 2);
}

template <typename T>
void Expression<T>::UnaryOperationNode::print(int indent) const {
    std::cout << std::string(indent, ' ') << "UnaryOp: " << operation << "\n";
    arg->print(indent + 2);
}

template <typename T>
void Expression<T>::FunctionNode::print(int indent) const { 
    std::cout << std::string(indent, ' ') << "Function: " << function << "\n";
    arg->print(indent + 2);
}

template <typename T>
void Expression<T>::debugAST() const { // Вывод AST для дебага
    if (root) {
        std::cout << "\nCurrent AST tree state:\n";
        root->print();
    }
    else std::cout << "Empty AST tree.\n";
}
