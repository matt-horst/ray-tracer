#include "vec3.hpp"
#include "test_util.hpp"
#include <cstdlib>
#include <iostream>

void test_add() {
    {
        // Test Vec<int> + Vec<int>
        const Vec3<int> a(1, 2, 3);
        const Vec3<int> b(4, 5, 6);
        
        const auto c = a + b;
        const Vec3<int> expected(5, 7, 9);
        assert_eq(c, expected);
    }

    {
        // Test Vec<double> + Vec<double>
        const Vec3<double> a(1.1, 2.2, 3.3);
        const Vec3<double> b(4.4, 5.5, 6.6);
        
        const auto c = a + b;
        const Vec3<double> expected(1.1 + 4.4, 2.2 + 5.5, 3.3 + 6.6);
        assert_eq(c, expected);
    }

    {
        // Test Vec<int> + Vec<double>
        const Vec3<int> a(1, 2, 3);
        const Vec3<double> b(4.5, 5.5, 6.0);

        const auto c = a + b;
        const Vec3<double> expected(5.5, 7.5, 9.0);
        assert_eq(c, expected);
    }

    {
        // Test += operator
        Vec3<int> a(1, 2, 3);
        const Vec3<int> b(4, 5, 6);
        a += b;
        const Vec3<int> expected(5, 7, 9);
        assert_eq(a, expected);
    }
}

void test_sub() {
    {
        // Test Vec<int> - Vec<int>
        const Vec3<int> a(1, 2, 3);
        const Vec3<int> b(3, 2, 1);
        
        const auto c = a - b;
        const Vec3<int> expected(-2, 0, 2);
        assert_eq(c, expected);
    }

    {
        // Test Vec<double> - Vec<double>
        const Vec3<double> a(1.1, 2.2, 3.3);
        const Vec3<double> b(3.3, 2.2, 1.1);
        
        const auto c = a - b;
        const Vec3<double> expected(1.1 - 3.3, 0.0, 3.3 - 1.1);
        assert_eq(c, expected);
    }

    {
        // Test Vec<int> - Vec<double>
        const Vec3<int> a(1, 2, 3);
        const Vec3<double> b(3.3, 2.2, 1.1);

        const auto c = a - b;
        const Vec3<double> expected(1 - 3.3, 2 - 2.2, 3 - 1.1);
        assert_eq(c, expected);
    }

    {
        // Test -= operator
        Vec3<int> a(1, 2, 3);
        const Vec3<int> b(3, 2, 1);
        a -= b;
        const Vec3<int> expected(-2, 0, 2);
        assert_eq(a, expected);
    }
}

void test_mul() {
    {
        // Test Vec<int> * int
        const Vec3<int> a(1, 2, 3);
        const auto b = a * 2;
        const Vec3<int> expected( 2, 4, 6);
        assert_eq(b, expected);
    }

    {
        // Test Vec<double> * double
        const Vec3<double> a(1.1, 2.2, 3.3);
        const auto b = a * 3.14;
        const Vec3<double> expected(1.1 * 3.14, 2.2 * 3.14, 3.3 * 3.14);
        assert_eq(b, expected);
    }

    {
        // Test Vec<int> * double
        const Vec3<int> a(1, 2, 3);
        const auto b = a * 3.14;
        const Vec3<double> expected(1 * 3.14, 2 * 3.14, 3 * 3.14);
        assert_eq(b, expected);
    }

    {
        // Test Vec<double> * int
        const Vec3<double> a(1.1, 2.2, 3.3);
        const auto b = a * 2;
        const Vec3<double> expected(1.1 * 2, 2.2 * 2, 3.3 * 2);
        assert_eq(b, expected);
    }
    
    {
        // Test *= operator
        Vec3<int> a(1, 2, 3);
        a *= 2;
        const Vec3<int> expected(2, 4, 6);
        assert_eq(a, expected);
    }
}

void test_div() {
    {
        // Test Vec<int> / double
        const Vec3<int> a(1, 2, 3);
        const auto b =  a / 2;
        const Vec3<double> expected(1 / 2.0, 2 / 2.0, 3 / 2.0);
        assert_eq(b, expected);
    }

    {
        // Test Vec<double> / double
        const Vec3<double> a(1.1, 2.2, 3.3);
        const auto b =  a / 2;
        const Vec3<double> expected(1.1 / 2.0, 2.2 / 2.0, 3.3 / 2.0);
        assert_eq(b, expected);
    }

    {
        // Test /= operator
        Vec3<double> a(1.1, 2.2, 3.3);
        a /= 2;
        const Vec3<double> expected(1.1 / 2.0, 2.2 / 2.0, 3.3 / 2.0);
        assert_eq(a, expected);
    }
}

void test_dot() {
    {
        // Test dot(Vec<int>, Vec<int>)
        const Vec3<int> a(1, 2, 3);
        const Vec3<int> b(4, 5, 6);
        const auto c = dot(a, b);
        const int expected(32);
        assert_eq(c, expected);
    }

    {
        // Test dot(Vec<double>, Vec<double>)
        const Vec3<double> a(1.1, 2.2, 3.3);
        const Vec3<double> b(1.0, 2.0, 3.0);
        const auto c = dot(a, b);
        const double expected(1.1 * 1.0 + 2.2 * 2.0 + 3.3 * 3.0);
        assert_eq(c, expected);
    }

    {
        // Test dot(Vec<int>, Vec<double>)
        const Vec3<double> a(1.1, 2.2, 3.3);
        const Vec3<int> b(1, 2, 3);
        const auto c = dot(a, b);
        const double expected(1.1 * 1 + 2.2 * 2 + 3.3 * 3);
        assert_eq(c, expected);
    }
}

void test_cross() {
    {
        // Test cross(Vec<int>, Vec<int>)
        const Vec3<int> a(1, 2, 3);
        const Vec3<int> b(4, 5, 6);
        const auto c = cross(a, b);
        const Vec3<int> expected(-3, 6, -3);
        assert_eq(c, expected);
    }
}

void test_length() {
    {
        const Vec3<int> a(2, 10, 11);
        const auto len_sqr = a.length_sqr();
        const auto len = a.length();

        const double expected_len_sqr = 225.0;
        const double expected_len = 15.0;

        assert_eq(len_sqr, expected_len_sqr);
        assert_eq(len, expected_len);
    }
}


int main(void) {
    test_add();
    test_sub();
    test_mul();
    test_div();
    test_dot();
    test_cross();
    test_length();
}
