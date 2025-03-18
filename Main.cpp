#include "Expression.hpp"
#include "Tests.hpp"

int main(int argc, char* argv[]) {

    if ((std::string)argv[1] == "test") Tests();
    
    else if (std::string(argv[1]) == "--eval") {
        
        std::string subs_vars;
        for (int i = 3; i < argc; i++)
            subs_vars += (' ' + std::string(argv[i]));
        
        if (std::string(argv[2]).find('I') != std::string::npos ||
            subs_vars.find('I') != std::string::npos) {

            Expression<std::complex<long double>> expr(argv[2]);
            expr.subsVar(subs_vars);
            expr.debugAST();
            std::cout << expr.evaluate() << std::endl;
        }
        else {

            Expression<long double> expr(argv[2]);
            expr.debugAST();
            expr.subsVar(subs_vars);
            std::cout << expr.evaluate() << std::endl;
        }
    }

    else if (std::string(argv[1]) == "--diff") {

        if (std::string(argv[2]).find('I') != std::string::npos) {

            Expression<std::complex<long double>> expr(argv[2]);
            expr.debugAST();
            std::cout << expr.differentiate(argv[4]).toString() << std::endl;
        }
        else {
            Expression<long double> expr(argv[2]);
            expr.debugAST();
            std::cout << expr.differentiate(argv[4]).toString() << std::endl;
        }
    }

    else {
        std::cout << "Invalid commad.";
    }
}
