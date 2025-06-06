#include "color.hpp"
#include "test_util.hpp"

void test_from_float() {
    {
        // Test Color(Float, Float, Float)
        const Color a(1.0, 0.0, 0.25);
        assert_eq(a.r_int(), static_cast<uint32_t>(255));
        assert_eq(a.g_int(), static_cast<uint32_t>(0));
        assert_eq(a.b_int(), static_cast<uint32_t>(63));
    }
}

int main(void) {
    test_from_float();
}
