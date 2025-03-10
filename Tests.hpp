#ifndef EXPR_TEST_HPP
#define EXPR_TEST_HPP

void TEST_CASE(std::string, bool);
void Tests();
bool areActuallyEqual(long double, long double, long double epsilon = 1e-10);

#endif