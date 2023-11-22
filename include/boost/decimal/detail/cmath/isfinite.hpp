// Copyright 2023 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_DECIMAL_DETAIL_CMATH_ISFINITE_HPP
#define BOOST_DECIMAL_DETAIL_CMATH_ISFINITE_HPP

#include <boost/decimal/fwd.hpp>
#include <boost/decimal/detail/config.hpp>
#include <boost/decimal/detail/type_traits.hpp>
#include <boost/decimal/detail/concepts.hpp>
#include <type_traits>
#include <cmath>

namespace boost {
namespace decimal {

template <BOOST_DECIMAL_DECIMAL_FLOATING_TYPE T>
constexpr auto isfinite BOOST_DECIMAL_PREVENT_MACRO_SUBSTITUTION (T rhs) noexcept
    -> std::enable_if_t<detail::is_decimal_floating_point_v<T>, bool>
{
    return !isinf(rhs) && !isnan(rhs);
}

} // namespace decimal
} // namespace boost

#endif //BOOST_DECIMAL_DETAIL_CMATH_ISFINITE_HPP
