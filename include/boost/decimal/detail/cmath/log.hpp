// Copyright 2023 - 2024 Matt Borland
// Copyright 2023 - 2024 Christopher Kormanyos
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_DECIMAL_DETAIL_CMATH_LOG_HPP
#define BOOST_DECIMAL_DETAIL_CMATH_LOG_HPP

#include <boost/decimal/fwd.hpp> // NOLINT(llvm-include-order)
#include <boost/decimal/detail/cmath/impl/log_impl.hpp>
#include <boost/decimal/detail/concepts.hpp>
#include <boost/decimal/detail/config.hpp>
#include <boost/decimal/detail/type_traits.hpp>
#include <boost/decimal/numbers.hpp>

#ifndef BOOST_DECIMAL_BUILD_MODULE
#include <cmath>
#include <type_traits>
#endif

namespace boost {
namespace decimal {

namespace detail {

template <typename T>
constexpr auto log_impl(T x) noexcept
    BOOST_DECIMAL_REQUIRES(detail::is_decimal_floating_point_v, T)
{
    constexpr T zero { 0, 0 };
    constexpr T one  { 1, 0 };

    T result { };

    if (isnan(x))
    {
        result = x;
    }
    else if (isinf(x))
    {
        result = (!signbit(x)) ? x: std::numeric_limits<T>::quiet_NaN();
    }
    else if (x < one)
    {
        // Handle reflection, the [+/-] zero-pole, and non-pole, negative x.
        if (x > zero)
        {
            result = -log(one / x);
        }
        else if ((x == zero) || (-x == zero))
        {
            // Actually, this should be equivalent to -HUGE_VAL.

            result = -std::numeric_limits<T>::infinity();
        }
        else
        {
            result = std::numeric_limits<T>::quiet_NaN();
        }
    }
    else if(x > one)
    {
        // The algorithm for logarithm is based on Chapter 5, pages 35-36
        // of Cody and Waite, Software Manual for the Elementary Functions,
        // Prentice Hall, 1980.

        int exp2val { };

        // TODO(ckormanyos) There is probably something more efficient than calling frexp here.
        auto g = frexp(x, &exp2val);

        if (g < numbers::inv_sqrt2_v<T>)
        {
            g += g;

            --exp2val;
        }

        const auto s   = (g - one) / (g + one);
        const auto z   = s + s;
        const auto zsq = z * z;

        result = z * fma(detail::log_series_expansion(zsq), zsq, one);

        if (exp2val > 0)
        {
            result += static_cast<T>(exp2val * numbers::ln2_v<T>);
        }
    }
    else
    {
        result = zero;
    }

    return result;
}

} //namespace detail

BOOST_DECIMAL_EXPORT template <typename T>
constexpr auto log(T x) noexcept
    BOOST_DECIMAL_REQUIRES(detail::is_decimal_floating_point_v, T)
{
    #if BOOST_DECIMAL_DEC_EVAL_METHOD == 0

    using evaluation_type = T;

    #elif BOOST_DECIMAL_DEC_EVAL_METHOD == 1

    using evaluation_type = detail::promote_args_t<T, decimal64>;

    #else // BOOST_DECIMAL_DEC_EVAL_METHOD == 2

    using evaluation_type = detail::promote_args_t<T, decimal128>;

    #endif

    return static_cast<T>(detail::log_impl(static_cast<evaluation_type>(x)));
}

} // namespace decimal
} // namespace boost

#endif // BOOST_DECIMAL_DETAIL_CMATH_LOG_HPP
