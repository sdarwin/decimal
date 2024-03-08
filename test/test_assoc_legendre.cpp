// Copyright 2024 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// Propogates up from boost.math
#define _SILENCE_CXX23_DENORM_DEPRECATION_WARNING

#include <boost/decimal.hpp>
#include <boost/core/lightweight_test.hpp>
#include <boost/math/special_functions/next.hpp>
#include <boost/math/special_functions/legendre.hpp>
#include <iostream>
#include <random>
#include <cmath>

#if !defined(BOOST_DECIMAL_REDUCE_TEST_DEPTH)
static constexpr auto N = static_cast<std::size_t>(128U); // Number of trials
#else
static constexpr auto N = static_cast<std::size_t>(128U >> 4U); // Number of trials
#endif

static std::mt19937_64 rng(42);

using namespace boost::decimal;

template <typename Dec>
void test()
{
    std::uniform_real_distribution<float> dist(-1, 1);

    constexpr auto max_iter {std::is_same<Dec, decimal128>::value ? N / 4 : N};
    for (std::size_t i {}; i < max_iter / 4; ++i)
    {
        for (unsigned n {}; n < 4; ++n)
        {
            for (unsigned m {}; m < 4; ++m)
            {
                const auto val1 {dist(rng)};
                Dec d1 {val1};

                auto ret_val {boost::math::legendre_p(static_cast<int>(n), static_cast<int>(m), val1)};
                auto ret_dec {static_cast<float>(assoc_legendre(n, m, d1))};

                if (!BOOST_TEST(std::fabs(ret_val - ret_dec) < 20*std::numeric_limits<float>::epsilon()))
                {
                    // LCOV_EXCL_START
                    std::cerr << "Val 1: " << val1
                              << "\nDec 1: " << d1
                              << "\nRet val: " << ret_val
                              << "\nRet dec: " << ret_dec
                              << "\nEps: " << std::fabs(ret_val - ret_dec) / std::numeric_limits<float>::epsilon() << std::endl;
                    // LCOV_EXCL_STOP
                }
            }
        }
    }
}

int main()
{
    test<decimal32>();
    test<decimal64>();

    #if !defined(BOOST_DECIMAL_REDUCE_TEST_DEPTH)
    test<decimal128>();
    #endif

    return boost::report_errors();
}
