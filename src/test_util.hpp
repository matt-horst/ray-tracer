#include <iostream>

template<typename T>
void assert_eq(T a, T b) {
    if (a == b) {
        return;
    }

    std::cerr << "Assertion Failed: " << a << " != " << b << '\n';
    exit(EXIT_FAILURE);
}
