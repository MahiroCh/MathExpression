#include "Expression.hpp"

int main() {
    Expression<std::complex<long double>> expr = "  (1 + x)";
    Expression<std::complex<long double>> expr1 = std::complex<long double>(14,0);
    Expression<std::complex<long double>> exprres = expr * expr1;

    std::cout << "\nExpression as string: " << exprres.toString() << std::endl;
    std::cout << "\nExpression as string: " << expr1.toString() << std::endl;
    std::cout << "\nExpression as string: " << expr.toString() << std::endl;
    exprres.subsVar("x=5.123 + 3I");
    std::cout  << "\nExpression as string after sub: " << exprres.toString() << std::endl;
    std::cout << exprres.evaluate() << std::endl;
}
