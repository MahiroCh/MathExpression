#include "Expression.hpp"
#include "Tests.hpp"

void TEST_CASE(std::string name, bool expr) {
    if (expr) std::cout  << name << " [ OK ] " << std::endl; 
    else std::cout << name << " [FAIL] " << std::endl;
}

bool areActuallyEqual(long double a, long double b, long double epsilon) {
    return std::fabs(a - b) < epsilon;
}

/* SPOILER:
Все тесты, хоть и выглядят очень уродливо, были кропотливо разными схэмами проверены 
через всевозможные математические движки инетернета на корректность. 
Читатель может самостоятельно удостовериться в корректности, но предупреждаю, что
это может нанести тяжелую психологическую травму, которая потребует длительной
реабилитации.
*/
void Tests() {
    
    Expression<long double> expr_1;
    Expression<long double> expr_1_1;
    Expression<long double> expr_1_1_1;
    Expression<std::complex<long double>> expr_2;
    Expression<std::complex<long double>> expr_2_2;
    Expression<std::complex<long double>> expr_2_2_2;
    Expression<long double> res_1;
    Expression<std::complex<long double>> res_2;


    expr_2 = "-6x^2 -4x^x + 000010 +      sin(y) * exp((-12I + 0003) * x)";
    res_2 = expr_2.differentiate("x");
    // std::cout << std::endl << res_2.toString() << std::endl;
    TEST_CASE("Test 1 (complex derivative): ", 
        res_2.toString() == "((((((-0I) * (x^2)) + ((-6) * ((x^2) * ((0I * ln(x)) + (2 * (1 / x)))))) - ((0I * (x^x)) + (4 * ((x^x) * ((1 * ln(x)) + (x * (1 / x))))))) + 0I) + (((cos(y) * 0I) * exp((((-12I) + 3) * x))) + (sin(y) * (exp((((-12I) + 3) * x)) * ((((-0I) + 0I) * x) + (((-12I) + 3) * 1))))))");


    expr_1 = "-6x^2 -4x^x + 10 +      sin(y) * exp((-12x + 3) * x)";
    res_1 = expr_1.differentiate("x");
    // std::cout << std::endl << res_1.toString() << std::endl;
    TEST_CASE("Test 2 (long double derivative): ", 
        res_1.toString() == "((((((-0) * (x^2)) + ((-6) * ((x^2) * ((0 * ln(x)) + (2 * (1 / x)))))) - ((0 * (x^x)) + (4 * ((x^x) * ((1 * ln(x)) + (x * (1 / x))))))) + 0) + (((cos(y) * 0) * exp(((((-12) * x) + 3) * x))) + (sin(y) * (exp(((((-12) * x) + 3) * x)) * ((((((-0) * x) + ((-12) * 1)) + 0) * x) + ((((-12) * x) + 3) * 1))))))");


    expr_2 = "  -sin(x) *         y";
    expr_2.subsVar("x = -0013.000I + 4 y = -12 - 123I");
    // std::cout << std::endl << expr_2.toString() << std::endl;
    TEST_CASE("Test 3 (complex substitution): ", 
        expr_2.toString() == "((-sin((4 + (-13I)))) * ((-12) + (-123I)))");


    expr_2 = Expression<std::complex<long double>>(-1234) + Expression<std::complex<long double>> ("-sin(x) *         y");
    // std::cout << std::endl << expr_2.toString() << std::endl;
    TEST_CASE("Test 4 (complex addition and construction of expression from long double literal): ", 
        expr_2.toString() == "(((-1234) + 0I) + ((-sin(x)) * y))");


    expr_1 = Expression<long double> ("ln(y+1)") / Expression<long double> ("exp(x^2)");
    expr_1_1 = Expression<long double> ("-sin(t+1)") * Expression<long double> ("-cos(x^2)");
    expr_1_1_1 = expr_1 ^ expr_1_1;
    // std::cout << std::endl << expr_1_1_1.toString() << std::endl;
    TEST_CASE("Test 5 (long double expression arithmetic): ", 
        expr_2.toString() == "(((-1234) + 0I) + ((-sin(x)) * y))");


    expr_1 = Expression<long double> ("000014ln(4y+1)") / Expression<long double> ("exp(y*x^2)");
    expr_1_1 = Expression<long double> ("-sin(t+1)") * Expression<long double> ("-cos(x^2)");
    expr_1_1_1 = expr_1 ^ expr_1_1;
    // std::cout << std::endl << expr_1_1_1.toString() << std::endl;
    expr_1_1_1.subsVar("x = -1 y = 12 t = 11");
    // std::cout << std::endl << expr_1_1_1.evaluate() << std::endl;
    TEST_CASE("Test 6 (long double expression arithmetic, substitution and evaluation): ", 
        areActuallyEqual(expr_1_1_1.evaluate(), 10.17457074525700708802372314211268031810268L));
    

    expr_2 = Expression<std::complex<long double>> ("   0014.05ln   (4   y+1    )") / Expression<std::complex<long double>> ("exp(y*0.145x^2)");
    expr_2_2 = Expression<std::complex<long double>> ("-001.012   sin(t+1)") * Expression<std::complex<long double>> ("-cos(x^2)");
    expr_2_2_2 = expr_2 ^ expr_2_2;
    // std::cout << std::endl << expr_2_2_2.toString() << std::endl;
    expr_2_2_2.subsVar("x = -1+  I y = 12 - I003.00t = 11");
    // std::cout << std::endl << expr_2_2_2.toString() << std::endl;
    // std::cout << std::endl << expr_2_2_2.evaluate() << std::endl;
    TEST_CASE("Test 7 (complex expression arithmetic, substitution, bullshit-styled input and evaluation): ", 
        areActuallyEqual(expr_2_2_2.evaluate().real(), 0.000042446137086360141047899158628566L) &&
        areActuallyEqual(expr_2_2_2.evaluate().imag(), -0.000019545452948635391955159727883452L)
    );
}
