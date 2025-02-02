// Copyright 2023 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_DECIMAL_NUMBERS_HPP
#define BOOST_DECIMAL_NUMBERS_HPP

#include <boost/decimal/fwd.hpp>
#include <boost/decimal/literals.hpp>
#include <boost/decimal/detail/type_traits.hpp>
#include <boost/decimal/detail/concepts.hpp>
#include <boost/decimal/detail/emulated128.hpp>

#ifndef BOOST_DECIMAL_BUILD_MODULE
#include <cstdint>
#endif

namespace boost { namespace decimal { namespace numbers {

BOOST_DECIMAL_EXPORT template <BOOST_DECIMAL_DECIMAL_FLOATING_TYPE Dec, std::enable_if_t<detail::is_decimal_floating_point_v<Dec>, bool> = true>
BOOST_DECIMAL_CONSTEXPR_VARIABLE Dec e_v = Dec{UINT64_C(2718281828459045235), -18};

BOOST_DECIMAL_EXPORT template <>
BOOST_DECIMAL_CONSTEXPR_VARIABLE_SPECIALIZATION decimal128 e_v<decimal128> = decimal128{detail::uint128{UINT64_C(147358353192158),
                                                                                                        UINT64_C(5661142159003925334)}, -33};

BOOST_DECIMAL_EXPORT template <BOOST_DECIMAL_DECIMAL_FLOATING_TYPE Dec, std::enable_if_t<detail::is_decimal_floating_point_v<Dec>, bool> = true>
BOOST_DECIMAL_CONSTEXPR_VARIABLE Dec log2e_v = Dec{UINT64_C(1442695040888963407), -18};

BOOST_DECIMAL_EXPORT template <>
BOOST_DECIMAL_CONSTEXPR_VARIABLE_SPECIALIZATION decimal128 log2e_v<decimal128> = decimal128{detail::uint128{UINT64_C(78208654878293),
                                                                                                            UINT64_C(16395798456599530402)}, -33};

BOOST_DECIMAL_EXPORT template <BOOST_DECIMAL_DECIMAL_FLOATING_TYPE Dec, std::enable_if_t<detail::is_decimal_floating_point_v<Dec>, bool> = true>
BOOST_DECIMAL_CONSTEXPR_VARIABLE Dec log10e_v = Dec{UINT64_C(4342944819032518277), -19};

BOOST_DECIMAL_EXPORT template <>
BOOST_DECIMAL_CONSTEXPR_VARIABLE_SPECIALIZATION decimal128 log10e_v<decimal128> = decimal128{detail::uint128{UINT64_C(235431510388986),
                                                                                                             UINT64_C(2047877485384264674)}, -34};

BOOST_DECIMAL_EXPORT template <BOOST_DECIMAL_DECIMAL_FLOATING_TYPE Dec, std::enable_if_t<detail::is_decimal_floating_point_v<Dec>, bool> = true>
BOOST_DECIMAL_CONSTEXPR_VARIABLE Dec log10_2_v = Dec{UINT64_C(3010299956639811952), -19};

BOOST_DECIMAL_EXPORT template <>
BOOST_DECIMAL_CONSTEXPR_VARIABLE_SPECIALIZATION decimal128 log10_2_v<decimal128> = decimal128{detail::uint128{UINT64_C(163188687641095),
                                                                                                              UINT64_C(3612628795761985410)}, -34};

BOOST_DECIMAL_EXPORT template <BOOST_DECIMAL_DECIMAL_FLOATING_TYPE Dec, std::enable_if_t<detail::is_decimal_floating_point_v<Dec>, bool> = true>
BOOST_DECIMAL_CONSTEXPR_VARIABLE Dec pi_v = Dec{UINT64_C(3141592653589793238), -18};

BOOST_DECIMAL_EXPORT template <>
BOOST_DECIMAL_CONSTEXPR_VARIABLE_SPECIALIZATION decimal128 pi_v<decimal128> = decimal128{detail::uint128{UINT64_C(170306079004327),
                                                                                                         UINT64_C(13456286628489437068)}, -33};

BOOST_DECIMAL_EXPORT template <BOOST_DECIMAL_DECIMAL_FLOATING_TYPE Dec, std::enable_if_t<detail::is_decimal_floating_point_v<Dec>, bool> = true>
BOOST_DECIMAL_CONSTEXPR_VARIABLE Dec pi_over_four_v = Dec{UINT64_C(7853981633974483096), -19};

// For extraction of the 128-bit approximate value of pi/4, see also: https://godbolt.org/z/fcjjGP7bY
// See also:
//   N[Pi/4, 37]
//   0.7853981633974483096156608458198757210
BOOST_DECIMAL_EXPORT template <>
BOOST_DECIMAL_CONSTEXPR_VARIABLE_SPECIALIZATION decimal128 pi_over_four_v<decimal128> = decimal128{detail::uint128{UINT64_C(42576519751081932),
                                                                                                                   UINT64_C(6764235707220873609)}, -38};

BOOST_DECIMAL_EXPORT template <BOOST_DECIMAL_DECIMAL_FLOATING_TYPE Dec, std::enable_if_t<detail::is_decimal_floating_point_v<Dec>, bool> = true>
BOOST_DECIMAL_CONSTEXPR_VARIABLE Dec inv_pi_v = Dec{UINT64_C(3183098861837906715), -19};

BOOST_DECIMAL_EXPORT template <>
BOOST_DECIMAL_CONSTEXPR_VARIABLE_SPECIALIZATION decimal128 inv_pi_v<decimal128> = decimal128{detail::uint128{UINT64_C(172556135062039),
                                                                                                             UINT64_C(13820348844234745256)}, -34};

BOOST_DECIMAL_EXPORT template <BOOST_DECIMAL_DECIMAL_FLOATING_TYPE Dec, std::enable_if_t<detail::is_decimal_floating_point_v<Dec>, bool> = true>
BOOST_DECIMAL_CONSTEXPR_VARIABLE Dec inv_sqrtpi_v = Dec{UINT64_C(5641895835477562869), -19};

BOOST_DECIMAL_EXPORT template <>
BOOST_DECIMAL_CONSTEXPR_VARIABLE_SPECIALIZATION decimal128 inv_sqrtpi_v<decimal128> = decimal128{detail::uint128{UINT64_C(305847786088084),
                                                                                                                 UINT64_C(12695685840195063976)}, -34};

BOOST_DECIMAL_EXPORT template <BOOST_DECIMAL_DECIMAL_FLOATING_TYPE Dec, std::enable_if_t<detail::is_decimal_floating_point_v<Dec>, bool> = true>
BOOST_DECIMAL_CONSTEXPR_VARIABLE Dec ln2_v = Dec{UINT64_C(6931471805599453094), -19};

BOOST_DECIMAL_EXPORT template <>
BOOST_DECIMAL_CONSTEXPR_VARIABLE_SPECIALIZATION decimal128 ln2_v<decimal128> = decimal128{detail::uint128{UINT64_C(375755839507647),
                                                                                                          UINT64_C(8395602002641374208)}, -34};

BOOST_DECIMAL_EXPORT template <BOOST_DECIMAL_DECIMAL_FLOATING_TYPE Dec, std::enable_if_t<detail::is_decimal_floating_point_v<Dec>, bool> = true>
BOOST_DECIMAL_CONSTEXPR_VARIABLE Dec ln10_v = Dec{UINT64_C(2302585092994045684), -18};

BOOST_DECIMAL_EXPORT template <>
BOOST_DECIMAL_CONSTEXPR_VARIABLE_SPECIALIZATION decimal128 ln10_v<decimal128> = decimal128{detail::uint128{UINT64_C(124823388007844),
                                                                                                           UINT64_C(1462833818723808456)}, -33};

BOOST_DECIMAL_EXPORT template <BOOST_DECIMAL_DECIMAL_FLOATING_TYPE Dec, std::enable_if_t<detail::is_decimal_floating_point_v<Dec>, bool> = true>
BOOST_DECIMAL_CONSTEXPR_VARIABLE Dec sqrt2_v = Dec{UINT64_C(1414213562373095049), -18};

BOOST_DECIMAL_EXPORT template <>
BOOST_DECIMAL_CONSTEXPR_VARIABLE_SPECIALIZATION decimal128 sqrt2_v<decimal128> = decimal128{detail::uint128{UINT64_C(76664670834168),
                                                                                                            UINT64_C(12987834932751794202)}, -33};

BOOST_DECIMAL_EXPORT template <BOOST_DECIMAL_DECIMAL_FLOATING_TYPE Dec, std::enable_if_t<detail::is_decimal_floating_point_v<Dec>, bool> = true>
BOOST_DECIMAL_CONSTEXPR_VARIABLE Dec sqrt3_v = Dec{UINT64_C(1732050807568877294), -18};

BOOST_DECIMAL_EXPORT template <>
BOOST_DECIMAL_CONSTEXPR_VARIABLE_SPECIALIZATION decimal128 sqrt3_v<decimal128> = decimal128{detail::uint128{UINT64_C(93894662421072),
                                                                                                            UINT64_C(8437766544231453518)}, -33};

BOOST_DECIMAL_EXPORT template <BOOST_DECIMAL_DECIMAL_FLOATING_TYPE Dec, std::enable_if_t<detail::is_decimal_floating_point_v<Dec>, bool> = true>
BOOST_DECIMAL_CONSTEXPR_VARIABLE Dec sqrt10_v = Dec{UINT64_C(3162277660168379332), -18};

BOOST_DECIMAL_EXPORT template <>
BOOST_DECIMAL_CONSTEXPR_VARIABLE_SPECIALIZATION decimal128 sqrt10_v<decimal128> = decimal128{detail::uint128{UINT64_C(171427415457846),
                                                                                                             UINT64_C(13450487317535253574)}, -33};

BOOST_DECIMAL_EXPORT template <BOOST_DECIMAL_DECIMAL_FLOATING_TYPE Dec, std::enable_if_t<detail::is_decimal_floating_point_v<Dec>, bool> = true>
BOOST_DECIMAL_CONSTEXPR_VARIABLE Dec cbrt2_v = Dec{UINT64_C(1259921049894873165), -18};

BOOST_DECIMAL_EXPORT template <>
BOOST_DECIMAL_CONSTEXPR_VARIABLE_SPECIALIZATION decimal128 cbrt2_v<decimal128> = decimal128{detail::uint128{UINT64_C(68300456972811),
                                                                                                            UINT64_C(17628749411094165652)}, -33};

BOOST_DECIMAL_EXPORT template <BOOST_DECIMAL_DECIMAL_FLOATING_TYPE Dec, std::enable_if_t<detail::is_decimal_floating_point_v<Dec>, bool> = true>
BOOST_DECIMAL_CONSTEXPR_VARIABLE Dec cbrt10_v = Dec{UINT64_C(2154434690031883722), -18};

BOOST_DECIMAL_EXPORT template <>
BOOST_DECIMAL_CONSTEXPR_VARIABLE_SPECIALIZATION decimal128 cbrt10_v<decimal128> = decimal128{detail::uint128{UINT64_C(116792138570535),
                                                                                                             UINT64_C(2467411419527284790)}, -33};

BOOST_DECIMAL_EXPORT template <BOOST_DECIMAL_DECIMAL_FLOATING_TYPE Dec, std::enable_if_t<detail::is_decimal_floating_point_v<Dec>, bool> = true>
BOOST_DECIMAL_CONSTEXPR_VARIABLE Dec inv_sqrt2_v = Dec{UINT64_C(7071067811865475244), -19};

BOOST_DECIMAL_EXPORT template <>
BOOST_DECIMAL_CONSTEXPR_VARIABLE_SPECIALIZATION decimal128 inv_sqrt2_v<decimal128> = decimal128{detail::uint128{UINT64_C(383323354170843),
                                                                                                                UINT64_C(9598942442630316202)}, -34};

BOOST_DECIMAL_EXPORT template <BOOST_DECIMAL_DECIMAL_FLOATING_TYPE Dec, std::enable_if_t<detail::is_decimal_floating_point_v<Dec>, bool> = true>
BOOST_DECIMAL_CONSTEXPR_VARIABLE Dec inv_sqrt3_v = Dec{UINT64_C(5773502691896257645), -19};

BOOST_DECIMAL_EXPORT template <>
BOOST_DECIMAL_CONSTEXPR_VARIABLE_SPECIALIZATION decimal128 inv_sqrt3_v<decimal128> = decimal128{detail::uint128{UINT64_C(312982208070241),
                                                                                                                UINT64_C(9679144407061960114)}, -34};

BOOST_DECIMAL_EXPORT template <BOOST_DECIMAL_DECIMAL_FLOATING_TYPE Dec, std::enable_if_t<detail::is_decimal_floating_point_v<Dec>, bool> = true>
BOOST_DECIMAL_CONSTEXPR_VARIABLE Dec egamma_v = Dec{UINT64_C(5772156649015328606), -19};

BOOST_DECIMAL_EXPORT template <>
BOOST_DECIMAL_CONSTEXPR_VARIABLE_SPECIALIZATION decimal128 egamma_v<decimal128> = decimal128{detail::uint128{UINT64_C(312909238939453),
                                                                                                             UINT64_C(7916302232898517972)}, -34};

BOOST_DECIMAL_EXPORT template <BOOST_DECIMAL_DECIMAL_FLOATING_TYPE Dec, std::enable_if_t<detail::is_decimal_floating_point_v<Dec>, bool> = true>
BOOST_DECIMAL_CONSTEXPR_VARIABLE Dec phi_v = Dec{UINT64_C(1618033988749894848), -18};

BOOST_DECIMAL_EXPORT template <>
BOOST_DECIMAL_CONSTEXPR_VARIABLE_SPECIALIZATION decimal128 phi_v<decimal128> = decimal128{detail::uint128{UINT64_C(87713798287901),
                                                                                                          UINT64_C(2061523135646567614)}, -33};

BOOST_DECIMAL_EXPORT BOOST_DECIMAL_CONSTEXPR_VARIABLE auto e {e_v<decimal64>};
BOOST_DECIMAL_EXPORT BOOST_DECIMAL_CONSTEXPR_VARIABLE auto log10_2 {log10_2_v<decimal64>};
BOOST_DECIMAL_EXPORT BOOST_DECIMAL_CONSTEXPR_VARIABLE auto log10e {log10e_v<decimal64>};
BOOST_DECIMAL_EXPORT BOOST_DECIMAL_CONSTEXPR_VARIABLE auto log2e {log2e_v<decimal64>};
BOOST_DECIMAL_EXPORT BOOST_DECIMAL_CONSTEXPR_VARIABLE auto pi {pi_v<decimal64>};
BOOST_DECIMAL_EXPORT BOOST_DECIMAL_CONSTEXPR_VARIABLE auto inv_pi {inv_pi_v<decimal64>};
BOOST_DECIMAL_EXPORT BOOST_DECIMAL_CONSTEXPR_VARIABLE auto inv_sqrtpi {inv_sqrtpi_v<decimal64>};
BOOST_DECIMAL_EXPORT BOOST_DECIMAL_CONSTEXPR_VARIABLE auto ln2 {ln2_v<decimal64>};
BOOST_DECIMAL_EXPORT BOOST_DECIMAL_CONSTEXPR_VARIABLE auto ln10 {ln10_v<decimal64>};
BOOST_DECIMAL_EXPORT BOOST_DECIMAL_CONSTEXPR_VARIABLE auto sqrt2 {sqrt2_v<decimal64>};
BOOST_DECIMAL_EXPORT BOOST_DECIMAL_CONSTEXPR_VARIABLE auto sqrt3 {sqrt3_v<decimal64>};
BOOST_DECIMAL_EXPORT BOOST_DECIMAL_CONSTEXPR_VARIABLE auto sqrt10 {sqrt10_v<decimal64>};
BOOST_DECIMAL_EXPORT BOOST_DECIMAL_CONSTEXPR_VARIABLE auto cbrt2 {cbrt2_v<decimal64>};
BOOST_DECIMAL_EXPORT BOOST_DECIMAL_CONSTEXPR_VARIABLE auto cbrt10 {cbrt10_v<decimal64>};
BOOST_DECIMAL_EXPORT BOOST_DECIMAL_CONSTEXPR_VARIABLE auto inv_sqrt2 {inv_sqrt2_v<decimal64>};
BOOST_DECIMAL_EXPORT BOOST_DECIMAL_CONSTEXPR_VARIABLE auto inv_sqrt3 {inv_sqrt3_v<decimal64>};
BOOST_DECIMAL_EXPORT BOOST_DECIMAL_CONSTEXPR_VARIABLE auto egamma {egamma_v<decimal64>};
BOOST_DECIMAL_EXPORT BOOST_DECIMAL_CONSTEXPR_VARIABLE auto phi {phi_v<decimal64>};

} // namespace numbers
} // namespace decimal
} // namespace boost

#endif //BOOST_DECIMAL_NUMBERS_HPP
