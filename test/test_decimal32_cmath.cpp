// Copyright 2023 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// Propogates up from boost.math
#define _SILENCE_CXX23_DENORM_DEPRECATION_WARNING

#include "mini_to_chars.hpp"
#include <boost/decimal.hpp>
#include <boost/core/lightweight_test.hpp>
#include <boost/math/special_functions/next.hpp>
#include <iostream>
#include <random>
#include <cmath>

static constexpr auto N {1024};

using namespace boost::decimal;

template <typename Dec>
void test_fmax()
{
    BOOST_TEST_EQ(fmax(Dec(1), BOOST_DECIMAL_DEC_NAN), Dec(1));
    BOOST_TEST_EQ(fmax(BOOST_DECIMAL_DEC_NAN, Dec(1)), Dec(1));
    BOOST_TEST(isnan(fmax(BOOST_DECIMAL_DEC_NAN, BOOST_DECIMAL_DEC_NAN)));
    BOOST_TEST_EQ(fmax(BOOST_DECIMAL_DEC_INFINITY, -BOOST_DECIMAL_DEC_INFINITY), BOOST_DECIMAL_DEC_INFINITY);

    BOOST_TEST_EQ(fmax(Dec(1), Dec(0)), Dec(1));
    BOOST_TEST_EQ(fmax(Dec(-2), Dec(1)), Dec(1));
}

template <typename Dec>
void test_isgreater()
{
    BOOST_TEST_EQ(isgreater(Dec(1), BOOST_DECIMAL_DEC_NAN), false);
    BOOST_TEST_EQ(isgreater(BOOST_DECIMAL_DEC_NAN, Dec(1)), false);
    BOOST_TEST_EQ(isgreater(BOOST_DECIMAL_DEC_NAN, BOOST_DECIMAL_DEC_NAN), false);
    BOOST_TEST_EQ(isgreater(BOOST_DECIMAL_DEC_INFINITY, -BOOST_DECIMAL_DEC_INFINITY), true);

    BOOST_TEST_EQ(isgreater(Dec(1), Dec(0)), true);
    BOOST_TEST_EQ(isgreater(Dec(-2), Dec(1)), false);
    BOOST_TEST_EQ(isgreater(Dec(1), Dec(1)), false);
}

template <typename Dec>
void test_isgreaterequal()
{
    BOOST_TEST_EQ(isgreaterequal(Dec(1), BOOST_DECIMAL_DEC_NAN), false);
    BOOST_TEST_EQ(isgreaterequal(BOOST_DECIMAL_DEC_NAN, Dec(1)), false);
    BOOST_TEST_EQ(isgreaterequal(BOOST_DECIMAL_DEC_NAN, BOOST_DECIMAL_DEC_NAN), false);
    BOOST_TEST_EQ(isgreaterequal(BOOST_DECIMAL_DEC_INFINITY, -BOOST_DECIMAL_DEC_INFINITY), true);

    BOOST_TEST_EQ(isgreaterequal(Dec(1), Dec(0)), true);
    BOOST_TEST_EQ(isgreaterequal(Dec(-2), Dec(1)), false);
    BOOST_TEST_EQ(isgreaterequal(Dec(1), Dec(1)), true);
}

template <typename Dec>
void test_fmin()
{
    BOOST_TEST_EQ(fmin(Dec(1), BOOST_DECIMAL_DEC_NAN), Dec(1));
    BOOST_TEST_EQ(fmin(BOOST_DECIMAL_DEC_NAN, Dec(1)), Dec(1));
    BOOST_TEST(isnan(fmin(BOOST_DECIMAL_DEC_NAN, BOOST_DECIMAL_DEC_NAN)));
    BOOST_TEST_EQ(fmin(BOOST_DECIMAL_DEC_INFINITY, -BOOST_DECIMAL_DEC_INFINITY), -BOOST_DECIMAL_DEC_INFINITY);

    BOOST_TEST_EQ(fmin(Dec(1), Dec(0)), Dec(0));
    BOOST_TEST_EQ(fmin(Dec(-2), Dec(1)), Dec(-2));
}

template <typename Dec>
void test_isless()
{
    BOOST_TEST_EQ(isless(Dec(1), BOOST_DECIMAL_DEC_NAN), false);
    BOOST_TEST_EQ(isless(BOOST_DECIMAL_DEC_NAN, Dec(1)), false);
    BOOST_TEST_EQ(isless(BOOST_DECIMAL_DEC_NAN, BOOST_DECIMAL_DEC_NAN), false);
    BOOST_TEST_EQ(isless(BOOST_DECIMAL_DEC_INFINITY, -BOOST_DECIMAL_DEC_INFINITY), false);

    BOOST_TEST_EQ(isless(Dec(1), Dec(0)), false);
    BOOST_TEST_EQ(isless(Dec(-2), Dec(1)), true);
    BOOST_TEST_EQ(isless(Dec(1), Dec(1)), false);
}

template <typename Dec>
void test_islessequal()
{
    BOOST_TEST_EQ(islessequal(Dec(1), BOOST_DECIMAL_DEC_NAN), false);
    BOOST_TEST_EQ(islessequal(BOOST_DECIMAL_DEC_NAN, Dec(1)), false);
    BOOST_TEST_EQ(islessequal(BOOST_DECIMAL_DEC_NAN, BOOST_DECIMAL_DEC_NAN), false);
    BOOST_TEST_EQ(islessequal(BOOST_DECIMAL_DEC_INFINITY, -BOOST_DECIMAL_DEC_INFINITY), false);

    BOOST_TEST_EQ(islessequal(Dec(1), Dec(0)), false);
    BOOST_TEST_EQ(islessequal(Dec(-2), Dec(1)), true);
    BOOST_TEST_EQ(islessequal(Dec(1), Dec(1)), true);
}

template <typename Dec>
void test_islessgreater()
{
    BOOST_TEST_EQ(islessgreater(Dec(1), BOOST_DECIMAL_DEC_NAN), false);
    BOOST_TEST_EQ(islessgreater(BOOST_DECIMAL_DEC_NAN, Dec(1)), false);
    BOOST_TEST_EQ(islessgreater(BOOST_DECIMAL_DEC_NAN, BOOST_DECIMAL_DEC_NAN), false);
    BOOST_TEST_EQ(islessgreater(BOOST_DECIMAL_DEC_INFINITY, -BOOST_DECIMAL_DEC_INFINITY), true);

    BOOST_TEST_EQ(islessgreater(Dec(1), Dec(0)), true);
    BOOST_TEST_EQ(islessgreater(Dec(-2), Dec(1)), true);
    BOOST_TEST_EQ(islessgreater(Dec(1), Dec(1)), false);
}

template <typename Dec>
void test_isunordered()
{
    BOOST_TEST_EQ(isunordered(Dec(1), BOOST_DECIMAL_DEC_NAN), true);
    BOOST_TEST_EQ(isunordered(BOOST_DECIMAL_DEC_NAN, Dec(1)), true);
    BOOST_TEST_EQ(isunordered(BOOST_DECIMAL_DEC_NAN, BOOST_DECIMAL_DEC_NAN), true);
    BOOST_TEST_EQ(isunordered(BOOST_DECIMAL_DEC_INFINITY, -BOOST_DECIMAL_DEC_INFINITY), false);

    BOOST_TEST_EQ(isunordered(Dec(1), Dec(0)), false);
    BOOST_TEST_EQ(isunordered(Dec(-2), Dec(1)), false);
    BOOST_TEST_EQ(isunordered(Dec(1), Dec(1)), false);
}

template <typename Dec>
void test_floor()
{
    BOOST_TEST(isnan(floor(BOOST_DECIMAL_DEC_NAN)));
    BOOST_TEST(isnan(floor(-BOOST_DECIMAL_DEC_NAN)));
    BOOST_TEST(isinf(floor(BOOST_DECIMAL_DEC_INFINITY)));
    BOOST_TEST(isinf(floor(-BOOST_DECIMAL_DEC_INFINITY)));
    BOOST_TEST_EQ(floor(Dec(0, 0)), Dec(0, 0));
    BOOST_TEST_EQ(floor(Dec(-0, 0)), Dec(-0, 0));

    BOOST_TEST_EQ(floor(Dec(27, -1)), Dec(2, 0));
    BOOST_TEST_EQ(floor(Dec(-27, -1)), Dec(-3, 0));
    BOOST_TEST_EQ(floor(Dec(27777, -4)), Dec(2, 0));
    BOOST_TEST_EQ(floor(Dec(-27777, -4)), Dec(-3, 0));

    // Bigger numbers
    BOOST_TEST_EQ(floor(Dec(27777, -2)), Dec(277, 0));
    BOOST_TEST_EQ(floor(Dec(-27777, -2)), Dec(-277, 0));
    BOOST_TEST_EQ(floor(Dec(27777, -1)), Dec(2777, 0));

    // Near zero
    BOOST_TEST_EQ(floor(Dec(3, -1)), Dec(0, 0));
    BOOST_TEST_EQ(floor(Dec(-3, -1)), Dec(-1, 0));
}

template <typename Dec>
void test_ceil()
{
    BOOST_TEST(isnan(ceil(BOOST_DECIMAL_DEC_NAN)));
    BOOST_TEST(isnan(ceil(-BOOST_DECIMAL_DEC_NAN)));
    BOOST_TEST(isinf(ceil(BOOST_DECIMAL_DEC_INFINITY)));
    BOOST_TEST(isinf(ceil(-BOOST_DECIMAL_DEC_INFINITY)));
    BOOST_TEST_EQ(ceil(Dec(0, 0)), Dec(0, 0));
    BOOST_TEST_EQ(ceil(Dec(-0, 0)), Dec(-0, 0));

    BOOST_TEST_EQ(ceil(Dec(27, -1)), Dec(3, 0));
    BOOST_TEST_EQ(ceil(Dec(-27, -1)), Dec(-2, 0));
    BOOST_TEST_EQ(ceil(Dec(27777, -4)), Dec(3, 0));
    BOOST_TEST_EQ(ceil(Dec(-27777, -4)), Dec(-2, 0));

    // Bigger numbers
    BOOST_TEST_EQ(ceil(Dec(27777, -2)), Dec(278, 0));
    BOOST_TEST_EQ(ceil(Dec(-27777, -2)), Dec(-277, 0));
    BOOST_TEST_EQ(ceil(Dec(27777, -1)), Dec(2778, 0));

    // Near zero
    BOOST_TEST_EQ(ceil(Dec(3, -1)), Dec(1, 0));
    BOOST_TEST_EQ(ceil(Dec(-3, -1)), Dec(0, 0));
}

template <typename Dec>
void test_trunc()
{
    BOOST_TEST(isnan(trunc(BOOST_DECIMAL_DEC_NAN)));
    BOOST_TEST(isnan(trunc(-BOOST_DECIMAL_DEC_NAN)));
    BOOST_TEST(isinf(trunc(BOOST_DECIMAL_DEC_INFINITY)));
    BOOST_TEST(isinf(trunc(-BOOST_DECIMAL_DEC_INFINITY)));
    BOOST_TEST_EQ(trunc(Dec(0, 0)), Dec(0, 0));
    BOOST_TEST_EQ(trunc(Dec(-0, 0)), Dec(-0, 0));

    BOOST_TEST_EQ(trunc(Dec(27, -1)), Dec(2, 0));
    BOOST_TEST_EQ(trunc(Dec(-27, -1)), Dec(-2, 0));
    BOOST_TEST_EQ(trunc(Dec(27777, -4)), Dec(2, 0));
    BOOST_TEST_EQ(trunc(Dec(-27777, -4)), Dec(-2, 0));
}

template <typename Dec>
void test_frexp10()
{
    int exp {};
    BOOST_TEST_EQ(frexp10(Dec(0,0), &exp), 0);
    BOOST_TEST_EQ(exp, 0);

    exp = -1;
    BOOST_TEST_EQ(frexp10(BOOST_DECIMAL_DEC_NAN, &exp), -1);
    BOOST_TEST_EQ(exp, 0);

    exp = -1;
    BOOST_TEST_EQ(frexp10(BOOST_DECIMAL_DEC_INFINITY, &exp), -1);
    BOOST_TEST_EQ(exp, 0);

    BOOST_TEST_EQ(frexp10(Dec(10, 0), &exp), 1'000'000);
    BOOST_TEST_EQ(exp, -5);

    BOOST_TEST_EQ(frexp10(Dec(1'000'000, 5), &exp), 1'000'000);
    BOOST_TEST_EQ(exp, 5);

    BOOST_TEST_EQ(frexp10(Dec(-1'000'000, 5), &exp), -1'000'000);
    BOOST_TEST_EQ(exp, 5);
}

template <typename Dec>
void test_scalbn()
{
    BOOST_TEST(isinf(scalbn(BOOST_DECIMAL_DEC_INFINITY, 1)));
    BOOST_TEST(isnan(scalbn(BOOST_DECIMAL_DEC_NAN, 1)));
    BOOST_TEST_EQ(scalbn(Dec(0, 0), 1), Dec(0, 0));

    decimal32 one {1, 0};
    decimal32 ten {1, 1};
    decimal32 hundred {1, 2};

    BOOST_TEST_EQ(scalbn(one, 1), ten);
    BOOST_TEST_EQ(scalbn(one, 2), hundred);
    BOOST_TEST_EQ(scalbn(ten, 1), hundred);
    BOOST_TEST_EQ(scalbn(hundred, -1), ten);
    BOOST_TEST_EQ(scalbn(hundred, -2), one);
    BOOST_TEST_EQ(scalbn(hundred, 0), hundred);
    BOOST_TEST(isinf(scalbn(one, 10000)));
}

template <typename Dec>
void test_scalbln()
{
    BOOST_TEST(isinf(scalbln(BOOST_DECIMAL_DEC_INFINITY, 1)));
    BOOST_TEST(isnan(scalbln(BOOST_DECIMAL_DEC_NAN, 1)));
    BOOST_TEST_EQ(scalbln(Dec(0, 0), 1), Dec(0, 0));

    decimal32 one {1, 0};
    decimal32 ten {1, 1};
    decimal32 hundred {1, 2};

    BOOST_TEST_EQ(scalbln(one, 1), ten);
    BOOST_TEST_EQ(scalbln(one, 2), hundred);
    BOOST_TEST_EQ(scalbln(ten, 1), hundred);
    BOOST_TEST_EQ(scalbln(hundred, -1), ten);
    BOOST_TEST_EQ(scalbln(hundred, -2), one);
    BOOST_TEST_EQ(scalbln(hundred, 0), hundred);
    BOOST_TEST(isinf(scalbln(one, 10000)));
}

template <typename Dec>
void test_div_fmod()
{
    std::mt19937_64 rng(42);
    std::uniform_real_distribution<float> dist(0.0F, 1e30F);

    for (std::size_t n {}; n < N; ++n)
    {
        const auto val1 {dist(rng)};
        const auto val2 {dist(rng)};
        decimal32 d1 {val1};
        decimal32 d2 {val2};

        auto float_div {val1 / val2};
        auto decimal_div {static_cast<float>(d1 / d2)};
        auto float_fmod {std::fmod(val1, val2)};
        auto decimal_fmod {static_cast<float>(fmod(d1, d2))};

        // Decimal types are all higher precision than float
        if (!(BOOST_TEST(std::fabs(boost::math::float_distance(float_fmod, decimal_fmod)) < 1e7) &&
              BOOST_TEST(std::fabs(boost::math::float_distance(float_div, decimal_div)) < 20)))
        {
            std::cerr << "Val 1: " << val1
                      << "\nDec 1: " << d1
                      << "\nVal 2: " << val2
                      << "\nDec 2: " << d2
                      << "\nVal div: " << float_div
                      << "\nDec div: " << decimal_div
                      << "\nDist: " << boost::math::float_distance(float_div, decimal_div)
                      << "\nVal fmod: " << float_fmod
                      << "\nDec fmod: " << decimal_fmod
                      << "\nDist: " << boost::math::float_distance(float_fmod, decimal_fmod) << std::endl;
        }
    }
}

template <typename Dec>
void test_copysign()
{
    std::mt19937_64 rng(42);
    std::uniform_real_distribution<float> dist(0.0F, 1e30F);

    for (std::size_t n {}; n < N; ++n)
    {
        const auto val1 {dist(rng)};
        const auto val2 {dist(rng)};
        decimal32 d1 {val1};
        decimal32 d2 {val2};

        auto ret_val {std::copysign(val1, val2)};
        auto ret_dec {static_cast<float>(copysign(d1, d2))};

        if (!BOOST_TEST(std::fabs(boost::math::float_distance(ret_val, ret_dec)) < 20))
        {
            std::cerr << "Val 1: " << val1
                      << "\nDec 1: " << d1
                      << "\nVal 2: " << val2
                      << "\nDec 2: " << d2
                      << "\nRet val: " << ret_val
                      << "\nRet dec: " << ret_dec << std::endl;
        }
    }
}

template <typename Dec>
void test_fma()
{
    BOOST_TEST_EQ(fma(decimal32(1, -1), decimal32(1, 1), decimal32(1, 0, true)), decimal32(0, 0));
}

template <typename Dec>
void test_sin()
{
    std::mt19937_64 rng(42);
    std::uniform_real_distribution<float> dist(-3.14F / 2, 3.14F / 2);

    for (std::size_t n {}; n < N; ++n)
    {
        const auto val1 {dist(rng)};
        decimal32 d1 {val1};

        auto ret_val {std::sin(val1)};
        auto ret_dec {static_cast<float>(sin(d1))};

        if (!BOOST_TEST(std::fabs(ret_val - ret_dec) < 5*std::numeric_limits<float>::epsilon()))
        {
            std::cerr << "Val 1: " << val1
                      << "\nDec 1: " << d1
                      << "\nRet val: " << ret_val
                      << "\nRet dec: " << ret_dec
                      << "\nEps: " << std::fabs(ret_val - ret_dec) / std::numeric_limits<float>::epsilon() << std::endl;
        }
    }
}

template <typename Dec>
void test_cos()
{
    std::mt19937_64 rng(42);
    std::uniform_real_distribution<float> dist(-3.14F / 2, 3.14F / 2);

    for (std::size_t n {}; n < N; ++n)
    {
        const auto val1 {dist(rng)};
        decimal32 d1 {val1};

        auto ret_val {std::cos(val1)};
        auto ret_dec {static_cast<float>(cos(d1))};

        if (!BOOST_TEST(std::fabs(ret_val - ret_dec) < 5*std::numeric_limits<float>::epsilon()))
        {
            std::cerr << "Val 1: " << val1
                      << "\nDec 1: " << d1
                      << "\nRet val: " << ret_val
                      << "\nRet dec: " << ret_dec
                      << "\nEps: " << std::fabs(ret_val - ret_dec) / std::numeric_limits<float>::epsilon() << std::endl;
        }
    }
}

template <typename Dec>
void test_modf()
{
    Dec ptr {};
    BOOST_TEST_EQ(modf(decimal32(123.45), &ptr), Dec(0.45));
    BOOST_TEST_EQ(ptr, Dec(123));

    BOOST_TEST_EQ(modf(BOOST_DECIMAL_DEC_INFINITY, &ptr), Dec(0));
    BOOST_TEST_EQ(ptr, BOOST_DECIMAL_DEC_INFINITY);

    BOOST_TEST(modf(BOOST_DECIMAL_DEC_NAN, &ptr) != BOOST_DECIMAL_DEC_NAN);
    BOOST_TEST(ptr != BOOST_DECIMAL_DEC_NAN);
}

template <typename Dec>
void test_remainder()
{
    std::mt19937_64 rng(42);
    std::uniform_real_distribution<float> dist(1e2F, 1e3F);

    for (std::size_t n {}; n < N; ++n)
    {
        const auto val1 {dist(rng)};
        const auto val2 {dist(rng)};
        decimal32 d1 {val1};
        decimal32 d2 {val2};

        auto ret_val {std::remainder(val1, val2)};
        auto ret_dec {static_cast<float>(remainder(d1, d2))};

        if (!BOOST_TEST(std::fabs(boost::math::float_distance(ret_val, ret_dec)) < 2000))
        {
            std::cerr << "Val 1: " << val1
                      << "\nDec 1: " << d1
                      << "\nVal 2: " << val2
                      << "\nDec 2: " << d2
                      << "\nRet val: " << ret_val
                      << "\nRet dec: " << ret_dec
                      << "\nFloat dist: " << boost::math::float_distance(ret_val, ret_dec) << std::endl;
        }
    }

    BOOST_TEST(remainder(BOOST_DECIMAL_DEC_INFINITY, Dec(1)) != BOOST_DECIMAL_DEC_NAN);
    BOOST_TEST(remainder(BOOST_DECIMAL_DEC_NAN, Dec(1)) != BOOST_DECIMAL_DEC_NAN);
    BOOST_TEST(remainder(Dec(1), BOOST_DECIMAL_DEC_NAN) != BOOST_DECIMAL_DEC_NAN);
    BOOST_TEST(remainder(Dec(1), Dec(0)) != BOOST_DECIMAL_DEC_NAN);
}

template <typename Dec>
void test_remquo()
{
    std::mt19937_64 rng(42);
    std::uniform_real_distribution<float> dist(1e2F, 1e3F);

    for (std::size_t n {}; n < N; ++n)
    {
        const auto val1 {dist(rng)};
        const auto val2 {dist(rng)};
        decimal32 d1 {val1};
        decimal32 d2 {val2};
        int flt_int {};
        int dec_int {};

        auto ret_val {std::remquo(val1, val2, &flt_int)};
        auto ret_dec {static_cast<float>(remquo(d1, d2, &dec_int))};

        if (!(BOOST_TEST(std::fabs(boost::math::float_distance(ret_val, ret_dec)) < 2000) &&
             (BOOST_TEST(flt_int == dec_int))))
        {
            std::cerr << "Val 1: " << val1
                      << "\nDec 1: " << d1
                      << "\nVal 2: " << val2
                      << "\nDec 2: " << d2
                      << "\nRet val: " << ret_val
                      << "\nRet dec: " << ret_dec
                      << "\nInt val: " << flt_int
                      << "\nInt quo: " << val1 / val2
                      << "\nInt dec: " << dec_int
                      << "\nDec quo: " << d1 / d2
                      << "\nFloat dist: " << boost::math::float_distance(ret_val, ret_dec) << std::endl;
        }
    }

    int quo {};
    BOOST_TEST(remquo(BOOST_DECIMAL_DEC_INFINITY, Dec(1), &quo) != BOOST_DECIMAL_DEC_NAN);
    BOOST_TEST(remquo(BOOST_DECIMAL_DEC_NAN, Dec(1), &quo) != BOOST_DECIMAL_DEC_NAN);
    BOOST_TEST(remquo(Dec(1), BOOST_DECIMAL_DEC_NAN, &quo) != BOOST_DECIMAL_DEC_NAN);
    BOOST_TEST(remquo(Dec(1), Dec(0), &quo) != BOOST_DECIMAL_DEC_NAN);
}

int main()
{
    test_fmax<decimal32>();
    test_isgreater<decimal32>();
    test_isgreaterequal<decimal32>();
    test_fmin<decimal32>();
    test_isless<decimal32>();
    test_islessequal<decimal32>();
    test_islessgreater<decimal32>();
    test_isunordered<decimal32>();

    test_floor<decimal32>();
    test_ceil<decimal32>();
    test_trunc<decimal32>();

    test_frexp10<decimal32>();
    test_scalbn<decimal32>();
    test_scalbln<decimal32>();

    test_div_fmod<decimal32>();

    test_copysign<decimal32>();

    test_fma<decimal32>();

    test_sin<decimal32>();
    test_cos<decimal32>();

    test_modf<decimal32>();

    test_remainder<decimal32>();
    test_remquo<decimal32>();

    return boost::report_errors();
}
