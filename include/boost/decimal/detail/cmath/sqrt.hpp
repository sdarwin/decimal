// Copyright 2023 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_DECIMAL_DETAIL_CMATH_SQRT_HPP
#define BOOST_DECIMAL_DETAIL_CMATH_SQRT_HPP

#include <boost/decimal/fwd.hpp>
#include <boost/decimal/detail/type_traits.hpp>
#include <boost/decimal/detail/cmath/abs.hpp>
#include <type_traits>
#include <cstdint>
#include <cmath>

namespace boost { namespace decimal {

// Bakhsali Approximation
template <typename T>
constexpr auto sqrt(T val) noexcept -> std::enable_if_t<detail::is_decimal_floating_point_v<T>, T>
{
    constexpr T zero {0, 0};

    T result { };

    if (isnan(val) || abs(val) == zero)
    {
        result = val;
    }
    else if (isinf(val))
    {
        if (signbit(val))
        {
            result = std::numeric_limits<T>::quiet_NaN();
        }
        else
        {
            result = val;
        }
    }
    else if (val < zero)
    {
        result = std::numeric_limits<T>::quiet_NaN();
    }
    else
    {
        constexpr T one { 1, 0 };

        if (val < one)
        {
          return one / sqrt(one / val);
        }
        else if (val > one)
        {
            // TODO(ckormanyos)
            // TODO(mborland)
            // This implementation of square root, although it works, needs optimization.
            // Also the number of Newton steps is only valid for decimal32 at the moment.

            int exp2val { };

            const auto man = frexp(val, &exp2val);

            const auto corrected = (static_cast<unsigned>(exp2val) & 1U) != 0U;

            if(!corrected)
            {
                result = ldexp(man / 2, exp2val / 2);
            }
            else
            {
                val *= T { 2, 0 };

                result = ldexp(man, --exp2val / 2);
            }

            for(auto i = 0U; i < 5U; ++i)
            {
                result = (result + val / result) / 2;
            }

            if(corrected) { result /= numbers::sqrt2_v<T>; }
        }
        else
        {
            result = one;
        }
    }

    return result;
}

} //namespace decimal
} //namespace boost

#endif //BOOST_DECIMAL_DETAIL_CMATH_SQRT_HPP

