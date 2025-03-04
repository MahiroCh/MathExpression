#include "Expression.hpp"

int main() {
    Expression<std::complex<long double>> expr = "sin(x) * cos(y) + ln(z + 2) - (cos(x + y) / sin(z + 1)) + x^2 * ln(y + 1) - z * cos(y - x)";
    std::cout << "\nExpression as string: " << expr.toString() << std::endl;
    expr.subsVar("x=5.123 z = 0.001400 + 31I y = 0001I");
    std::cout  << "\nExpression as string after sub: " << expr.toString() << std::endl;
    std::cout << expr.evaluate() << std::endl;

    std::cout << "\n\n\n\n";

    Expression<long double> expr1 = "sin(x) * cos(y) + ln(z + 2) - (cos(x + y) / sin(z + 1)) + x^2 * ln(y + 1) - z * cos(y - x)";
    std::cout << "\nExpression as string: " << expr1.toString() << std::endl;
    expr1.subsVar("x=5.123 z = 0.001400 y = 0001");
    std::cout  << "\nExpression as string after sub: " << expr1.toString() << std::endl;
    std::cout << expr1.evaluate() << std::endl;

    std::cout << "\n\n\n\n";

    Expression<long double> expr2 = 0015.23400;
    std::cout << "\nExpression as string: " << expr2.toString() << std::endl;
    std::cout << expr2.evaluate() << std::endl;

    std::cout << "\n\n\n\n";

    expr = "123 + x - y";
    std::cout << "\nExpression as string: " << expr.toString() << std::endl;
    expr.subsVar("x=5.123 z = 0.001400 y = 0001");
    std::cout  << "\nExpression as string after sub: " << expr.toString() << std::endl;
    std::cout << expr.evaluate() << std::endl;

    std::cout << "\n\n\n\n";

    expr1 = "sin(x) * cos(y) + ln(z + 2)";
    expr2 = "123 + x - y";
    Expression<long double> expr3 = expr2 / expr1;
    std::cout << "\nExpression as string: " << expr3.toString() << std::endl;
    expr3.subsVar("x=5.123 z = 0.001400 y = 0001");
    std::cout  << "\nExpression as string after sub: " << expr3.toString() << "\n\n";
    std::cout << expr3.evaluate() << std::endl;
    // не изменились
    std::cout << "\nExpression as string: " << expr1.toString() << std::endl;
    std::cout << "\nExpression as string: " << expr2.toString() << std::endl;

    std::cout << "\n\n\n\n";

    Expression<std::complex<long double>> expr = "sin(cos(x))";
    std::cout << "\nExpression as string: " << expr.toString() << std::endl;
    expr.subsVar("x= 0 + 32I");
    std::cout  << "\nExpression as string after sub: " << expr.toString() << std::endl;
    std::cout << expr.evaluate() << std::endl;
}
