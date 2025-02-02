// Copyright 2024 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_DECIMAL_CHARCONV_HPP
#define BOOST_DECIMAL_CHARCONV_HPP

#include <boost/decimal/fwd.hpp>
#include <boost/decimal/detail/config.hpp>
#include <boost/decimal/detail/parser.hpp>
#include <boost/decimal/detail/utilities.hpp>
#include <boost/decimal/detail/emulated128.hpp>
#include <boost/decimal/detail/from_chars_result.hpp>
#include <boost/decimal/detail/chars_format.hpp>
#include <boost/decimal/detail/concepts.hpp>
#include <boost/decimal/detail/to_chars_result.hpp>
#include <boost/decimal/detail/to_chars_integer_impl.hpp>
#include <boost/decimal/detail/buffer_sizing.hpp>
#include <boost/decimal/detail/cmath/frexp10.hpp>
#include <boost/decimal/detail/attributes.hpp>
#include <boost/decimal/detail/countl.hpp>
#include <boost/decimal/detail/remove_trailing_zeros.hpp>

#ifndef BOOST_DECIMAL_BUILD_MODULE
#include <cstdint>
#endif

#if !defined(BOOST_DECIMAL_DISABLE_CLIB)

namespace boost {
namespace decimal {

// ---------------------------------------------------------------------------------------------------------------------
// from_chars and implementation
// ---------------------------------------------------------------------------------------------------------------------

namespace detail {

template <BOOST_DECIMAL_DECIMAL_FLOATING_TYPE TargetDecimalType>
constexpr auto from_chars_general_impl(const char* first, const char* last, TargetDecimalType& value, chars_format fmt) noexcept -> from_chars_result
{
    using significand_type = std::conditional_t<std::is_same<TargetDecimalType, decimal128>::value, detail::uint128, std::uint64_t>;

    if (first >= last)
    {
        return {first, std::errc::invalid_argument};
    }

    bool sign {};
    significand_type significand {};
    std::int32_t expval {};

    auto r {detail::parser(first, last, sign, significand, expval, fmt)};

    if (!r)
    {
        if (r.ec == std::errc::not_supported)
        {
            if (significand)
            {
                value = std::numeric_limits<TargetDecimalType>::signaling_NaN();
            }
            else
            {
                value = std::numeric_limits<TargetDecimalType>::quiet_NaN();
            }

            r.ec = std::errc();
        }
        else if (r.ec == std::errc::value_too_large)
        {
            value = std::numeric_limits<TargetDecimalType>::infinity();
            r.ec = std::errc::result_out_of_range;
        }
        else
        {
            value = std::numeric_limits<TargetDecimalType>::signaling_NaN();
            errno = static_cast<int>(r.ec);
        }
    }
    else
    {
        value = TargetDecimalType(significand, expval, sign);
    }

    return r;
}

} //namespace detail

BOOST_DECIMAL_EXPORT constexpr auto from_chars(const char* first, const char* last, decimal32& value, chars_format fmt = chars_format::general) noexcept
{
    return detail::from_chars_general_impl(first, last, value, fmt);
}

BOOST_DECIMAL_EXPORT constexpr auto from_chars(const char* first, const char* last, decimal32_fast& value, chars_format fmt = chars_format::general) noexcept
{
    return detail::from_chars_general_impl(first, last, value, fmt);
}

BOOST_DECIMAL_EXPORT constexpr auto from_chars(const char* first, const char* last, decimal64& value, chars_format fmt = chars_format::general) noexcept
{
    return detail::from_chars_general_impl(first, last, value, fmt);
}

BOOST_DECIMAL_EXPORT constexpr auto from_chars(const char* first, const char* last, decimal128& value, chars_format fmt = chars_format::general) noexcept
{
    return detail::from_chars_general_impl(first, last, value, fmt);
}

// ---------------------------------------------------------------------------------------------------------------------
// to_chars and implementation
// ---------------------------------------------------------------------------------------------------------------------

namespace detail {

template <BOOST_DECIMAL_DECIMAL_FLOATING_TYPE TargetDecimalType>
BOOST_DECIMAL_CONSTEXPR auto to_chars_nonfinite(char* first, char* last, const TargetDecimalType& value, int fp, chars_format fmt, int precision) noexcept -> to_chars_result
{
    const auto buffer_len = last - first;

    switch (fp)
    {
        case FP_INFINITE:
            if (buffer_len >= 3)
            {
                boost::decimal::detail::memcpy(first, "inf", 3U);
                return {first + 3U, std::errc()};
            }

            return {last, std::errc::value_too_large};
        case FP_ZERO:
            if (fmt == chars_format::general)
            {
                if (buffer_len >= 7)
                {
                    boost::decimal::detail::memcpy(first, "0.0e+00", 7U);
                    return {first + 7U, std::errc()};
                }
            }
            else if (fmt == chars_format::hex || fmt == chars_format::scientific)
            {
                if (buffer_len >= 7 + precision + 1)
                {
                    if (precision == 0)
                    {
                        *first++ = '0';
                    }
                    else
                    {
                        boost::decimal::detail::memcpy(first, "0.0", 3U);
                        first += 3U;

                        if (precision != -1 && precision != 1)
                        {
                            boost::decimal::detail::memset(first, '0', static_cast<std::size_t>(precision - 1));
                            first += precision - 1;
                        }
                    }

                    if (fmt == chars_format::hex)
                    {
                        *first++ = 'p';
                    }
                    else
                    {
                        *first++ = 'e';
                    }

                    boost::decimal::detail::memcpy(first, "+00", 3U);
                    return {first + 3U, std::errc()};
                }
            }
            else
            {
                if (precision == -1 || precision == 0)
                {
                    *first++ = '0';
                    return {first, std::errc()};
                }
                else if (buffer_len > 2 + precision)
                {
                    boost::decimal::detail::memcpy(first, "0.0", 3U);
                    first += 3U;

                    if (precision > 1)
                    {
                        boost::decimal::detail::memset(first, '0', static_cast<std::size_t>(precision - 1));
                        first += precision - 1;
                    }

                    return {first, std::errc()};
                }
            }

            return {last, std::errc::value_too_large};
        case FP_NAN:
            if (issignaling(value) && buffer_len >= 9)
            {
                boost::decimal::detail::memcpy(first, "nan(snan)", 9U);
                return {first + 9U, std::errc()};
            }
            else if (signbit(value) && buffer_len >= 9)
            {
                boost::decimal::detail::memcpy(first, "nan(ind)", 8U);
                return {first + 8U, std::errc()};
            }
            else if (buffer_len >= 3)
            {
                boost::decimal::detail::memcpy(first, "nan", 3U);
                return {first + 3U, std::errc()};
            }

            return {last, std::errc::value_too_large};
        default:
            // LCOV_EXCL_START
            BOOST_DECIMAL_ASSERT_MSG(fp != 0, "Unreachable return");
            return {first, std::errc::not_supported};
            // LCOV_EXCL_STOP
    }
}

template <BOOST_DECIMAL_DECIMAL_FLOATING_TYPE TargetDecimalType>
BOOST_DECIMAL_CONSTEXPR auto to_chars_scientific_impl(char* first, char* last, const TargetDecimalType& value, chars_format fmt = chars_format::general, int precision = -1) noexcept -> to_chars_result
{
    if (signbit(value))
    {
        *first++ = '-';
    }

    const auto fp = fpclassify(value);
    if (fp != FP_NORMAL)
    {
        return to_chars_nonfinite(first, last, value, fp, fmt, precision);
    }

    int exp {};
    auto significand {frexp10(value, &exp)};

    using uint_type = std::conditional_t<std::is_same<TargetDecimalType, decimal128>::value, uint128, std::uint64_t>;
    auto significand_digits = num_digits(significand);
    exp += significand_digits - 1;
    bool append_zeros = false;

    if (precision != -1)
    {
        if (significand_digits > precision)
        {
            // If the precision is specified we need to make sure the result is rounded correctly
            // using the current fenv rounding mode

            while (significand_digits > precision + 2)
            {
                significand /= 10;
                --significand_digits;
            }

            if (significand_digits > precision + 1)
            {
                fenv_round(significand);
            }
        }
        else if (significand_digits < precision && fmt != chars_format::general)
        {
            append_zeros = true;
        }
    }

    // Offset the value of first by 1 so that we can copy the leading digit and insert a decimal point
    auto r = to_chars_integer_impl<uint_type, uint_type>(first + 1, last, significand, 10);

    // Only real reason we will hit this is a buffer overflow
    if (BOOST_DECIMAL_UNLIKELY(!r))
    {
        return r; // LCOV_EXCL_LINE
    }

    const auto current_digits = r.ptr - (first + 1) - 1;

    if (current_digits < precision && fmt != chars_format::general)
    {
        append_zeros = true;
    }

    if (append_zeros)
    {
        const auto zeros_inserted {static_cast<std::size_t>(precision - current_digits)};

        if (r.ptr + zeros_inserted > last)
        {
            return {last, std::errc::value_too_large};
        }

        boost::decimal::detail::memset(r.ptr, '0', zeros_inserted);
        r.ptr += zeros_inserted;
    }

    // Insert our decimal point
    *first = *(first + 1);
    *(first + 1) = '.';
    first = r.ptr;

    if (precision == 0)
    {
        --first;
    }

    // Strip trailing zeros in general mode
    if (fmt == chars_format::general)
    {
        --first;
        while (*first == '0')
        {
            --first;
        }

        // Remove decimal point if not significant digits
        if (*first != '.')
        {
            ++first;
        }
    }

    // Insert the exponent character
    *first++ = 'e';

    const int abs_exp { (exp < 0) ? -exp : exp };

    if (exp < 0)
    {
        *first++ = '-';
    }
    else
    {
        *first++ = '+';
    }

    // Always give 2 digits in the exp (ex. 2.0e+09)
    if (abs_exp <= 9)
    {
        *first++ = '0';
    }

    r = to_chars_integer_impl<int, unsigned>(first, last, abs_exp, 10);
    if (BOOST_DECIMAL_UNLIKELY(!r))
    {
        return r; // LCOV_EXCL_LINE
    }

    return {r.ptr, std::errc()};
}

template <BOOST_DECIMAL_DECIMAL_FLOATING_TYPE TargetDecimalType>
BOOST_DECIMAL_CONSTEXPR auto to_chars_fixed_impl(char* first, char* last, const TargetDecimalType& value, chars_format fmt = chars_format::general, int precision = -1) noexcept -> to_chars_result
{
    auto buffer_size = last - first;
    auto real_precision = get_real_precision<TargetDecimalType>(precision);

    // Dummy check the bounds
    if (buffer_size < real_precision)
    {
        return {last, std::errc::value_too_large};
    }

    const bool is_neg = signbit(value);
    if (is_neg)
    {
        *first++ = '-';
        --buffer_size;
    }

    const auto fp = fpclassify(value);
    if (fp != FP_NORMAL)
    {
        return to_chars_nonfinite(first, last, value, fp, fmt, precision);
    }

    auto abs_value = abs(value);
    int exponent {};
    auto significand = frexp10(abs_value, &exponent);

    const char* output_start = first;

    int num_dig = num_digits(significand);
    bool append_trailing_zeros = false;
    bool append_leading_zeros = false;
    int num_leading_zeros = 0;
    int integer_digits = num_dig + exponent;
    num_dig -= integer_digits;

    if (integer_digits < 0)
    {
        const int abs_integer_digits { (integer_digits < 0) ? -integer_digits : integer_digits };

        num_leading_zeros = abs_integer_digits;
        integer_digits = 0;
        append_leading_zeros = true;
    }

    if (precision != -1)
    {
        if (num_dig >= precision + 1)
        {
            while (num_dig > precision + 1)
            {
                significand /= 10;
                ++exponent;
                --num_dig;
            }

            if (num_dig == precision + 1)
            {
                --num_dig;
                exponent += fenv_round(significand);
            }
        }
        else if (num_dig < precision && fmt != chars_format::general)
        {
            append_trailing_zeros = true;
        }

        // In general formatting we remove trailing 0s
        if (fmt == chars_format::general)
        {

            const auto zeros_removal {remove_trailing_zeros(significand)};
            significand = zeros_removal.trimmed_number;
            exponent += static_cast<int>(zeros_removal.number_of_removed_zeros);
            num_dig -= static_cast<int>(zeros_removal.number_of_removed_zeros);
        }
    }

    // Make sure the result will fit in the buffer
    const std::ptrdiff_t total_length = total_buffer_length(num_dig, exponent, is_neg) + num_leading_zeros;
    if (total_length > buffer_size)
    {
        return {last, std::errc::value_too_large};
    }

    // Insert the leading zeros and return if the answer is ~0 for current precision
    if (append_leading_zeros)
    {
        if (precision == 0)
        {
            *first++ = '0';
            return {first, std::errc()};
        }
        else if (num_leading_zeros > precision)
        {
            *first++ = '0';
            *first++ = '.';
            std::memset(first, '0', static_cast<std::size_t>(precision));
            return {first + precision, std::errc()};
        }
        else
        {
            *first++ = '0';
            *first++ = '.';
            std::memset(first, '0', static_cast<std::size_t>(num_leading_zeros));
            first += num_leading_zeros;

            // We can skip the rest if there's nothing more to do for the required precision
            if (significand == 0)
            {
                std::memset(first, '0', static_cast<std::size_t>(precision - num_leading_zeros));
                return {first + precision, std::errc()};
            }
        }
    }

    using uint_type = std::conditional_t<std::is_same<TargetDecimalType, decimal128>::value, uint128, std::uint64_t>;
    auto r = to_chars_integer_impl<uint_type, uint_type>(first, last, significand, 10);

    if (BOOST_DECIMAL_UNLIKELY(!r))
    {
        return r;
    }

    // Bounds check again
    if (precision == 0)
    {
        return {r.ptr, std::errc()};
    }
    else if (abs_value >= 1)
    {
        if (exponent < 0 && -exponent < buffer_size)
        {
            // Bounds check our move
            if (r.ptr + 2 > last)
            {
                return {last, std::errc::value_too_large};
            }

            boost::decimal::detail::memmove(r.ptr + exponent + 1, r.ptr + exponent,
                                            static_cast<std::size_t>(-exponent));
            boost::decimal::detail::memset(r.ptr + exponent, '.', 1U);
            ++r.ptr;
        }
        else if (exponent >= 1)
        {
            // Bounds check the length of the memset before doing so
            if (r.ptr + exponent + 1 > last)
            {
                return {last, std::errc::value_too_large};
            }

            boost::decimal::detail::memset(r.ptr, '0', static_cast<std::size_t>(exponent));
            r.ptr += exponent;

            if (append_trailing_zeros)
            {
                *r.ptr++ = '.';
            }
        }
        else if (append_trailing_zeros)
        {
            *r.ptr++ = '.';
        }
    }
    else if (!append_leading_zeros)
    {
        #ifdef BOOST_DECIMAL_DEBUG_FIXED
        std::cerr << std::setprecision(std::numeric_limits<Real>::digits10) << "Value: " << value
                  << "\n  Buf: " << first
                  << "\n  sig: " << significand
                  << "\n  exp: " << exponent << std::endl;
        #endif

        const auto offset_bytes = static_cast<std::size_t>(integer_digits);

        // Bounds check memmove followed by insertion of 0.
        if (first + 2 + offset_bytes + (static_cast<std::size_t>(-exponent) - offset_bytes) + 2 > last)
        {
            return {last, std::errc::value_too_large};
        }

        boost::decimal::detail::memmove(first + 2 + offset_bytes,
                                        first,
                                        static_cast<std::size_t>(-exponent) - offset_bytes);

        boost::decimal::detail::memcpy(first, "0.", 2U);
        first += 2;
        r.ptr += 2;
    }

    // The leading 0 is an integer digit now that we need to account for
    if (integer_digits == 0)
    {
        ++integer_digits;
    }

    const auto current_fractional_digits = r.ptr - output_start - integer_digits - 1;
    if (current_fractional_digits < precision && fmt != chars_format::general)
    {
        append_trailing_zeros = true;
    }

    if (append_trailing_zeros)
    {
        const auto zeros_inserted = static_cast<std::size_t>(precision - current_fractional_digits);

        if (r.ptr + zeros_inserted > last)
        {
            return {last, std::errc::value_too_large};
        }

        boost::decimal::detail::memset(r.ptr, '0', zeros_inserted);
        r.ptr += zeros_inserted;
    }

    return {r.ptr, std::errc()};
}

template <BOOST_DECIMAL_DECIMAL_FLOATING_TYPE TargetDecimalType>
BOOST_DECIMAL_CONSTEXPR auto to_chars_hex_impl(char* first, char* last, const TargetDecimalType& value, int precision = -1) noexcept -> to_chars_result
{
    using Unsigned_Integer = std::conditional_t<!std::is_same<TargetDecimalType, decimal128>::value, std::uint64_t, uint128>;

    if (signbit(value))
    {
        *first++ = '-';
    }

    const auto fp = fpclassify(value);
    if (fp != FP_NORMAL)
    {
        return to_chars_nonfinite(first, last, value, fp, chars_format::hex, precision);
    }

    const std::ptrdiff_t buffer_size = last - first;
    auto real_precision = get_real_precision<TargetDecimalType>(precision);

    if (precision != -1)
    {
        real_precision = precision;
    }

    if (buffer_size < real_precision)
    {
        return {last, std::errc::value_too_large};
    }

    int exp {};
    Unsigned_Integer significand = frexp10(value, &exp);

    // Strip zeros of the significand since frexp10 normalizes it
    while (significand % 10U == 0)
    {
        significand /= 10U;
        ++exp;
    }

    // Calculate the number of bytes
    constexpr auto significand_bits = std::is_same<Unsigned_Integer, std::uint64_t>::value ? 64 : 128;
    auto significand_digits = static_cast<int>(std::ceil(static_cast<double>(significand_bits - countl_zero(significand)) / 4));
    bool append_zeros = false;

    if (precision != -1)
    {
        if (significand_digits > precision)
        {
            // If the precision is specified we need to make sure the result is rounded correctly
            // using the current fenv rounding mode

            while (significand_digits > precision + 2)
            {
                significand /= 16;
                --significand_digits;
            }

            if (significand_digits > precision + 1)
            {
                const auto trailing_digit = significand % 16;
                significand /= 16;
                ++exp;
                if (trailing_digit >= 8)
                {
                    ++significand;
                }
            }
        }
        else if (significand_digits < precision)
        {
            append_zeros = true;
        }
    }

    auto r = to_chars_integer_impl<Unsigned_Integer, Unsigned_Integer>(first + 1, last, significand, 16);
    if (BOOST_DECIMAL_UNLIKELY(!r))
    {
        return r; // LCOV_EXCL_LINE
    }

    const auto current_digits = r.ptr - (first + 1) - 1;
    exp += static_cast<int>(current_digits);

    if (current_digits < precision)
    {
        append_zeros = true;
    }

    if (append_zeros)
    {
        const auto zeros_inserted {static_cast<std::size_t>(precision - current_digits)};

        if (r.ptr + zeros_inserted > last)
        {
            return {last, std::errc::value_too_large};
        }

        boost::decimal::detail::memset(r.ptr, '0', zeros_inserted);
        r.ptr += zeros_inserted;
    }

    // Insert our decimal point
    *first = *(first + 1);
    *(first + 1) = '.';
    first = r.ptr;

    if (precision == 0)
    {
        --first;
    }

    *first++ = 'p';
    if (exp < 0)
    {
        *first++ = '-';
    }
    else
    {
        *first++ = '+';
    }

    const int abs_exp { (exp < 0) ? -exp : exp };

    if (abs_exp < 10)
    {
        *first++ = '0';
    }

    return to_chars_integer_impl<std::uint32_t, std::uint32_t>(first, last, static_cast<std::uint32_t>(abs_exp), 10);
}

template <BOOST_DECIMAL_DECIMAL_FLOATING_TYPE TargetDecimalType>
BOOST_DECIMAL_CONSTEXPR auto to_chars_impl(char* first, char* last, TargetDecimalType value, chars_format fmt = chars_format::general, int precision = -1) noexcept -> to_chars_result
{
    // Sanity check our bounds
    if (first >= last)
    {
        return {last, std::errc::value_too_large};
    }

    auto abs_value = abs(value);
    constexpr auto max_fractional_value = std::is_same<TargetDecimalType, decimal32>::value ? TargetDecimalType{1, 7} :
                                          std::is_same<TargetDecimalType, decimal64>::value ? TargetDecimalType{1, 16} :
                                                                                              TargetDecimalType{1, 34};

    constexpr auto min_fractional_value = TargetDecimalType{1, -4};

    if (fmt == chars_format::hex)
    {
        return to_chars_hex_impl(first, last, value, precision);
    }

    // Unspecified precision so we always go with the shortest representation
    if (precision == -1)
    {
        if (fmt == chars_format::general || fmt == chars_format::fixed)
        {
            if (abs_value >= 1 && abs_value < max_fractional_value)
            {
                return to_chars_fixed_impl(first, last, value, fmt, precision);
            }
            else
            {
                return to_chars_scientific_impl(first, last, value, fmt, precision);
            }
        }
        else
        {
            return to_chars_scientific_impl(first, last, value, fmt, precision);
        }
    }
    else
    {
        // In this range with general formatting, fixed formatting is the shortest
        if (fmt == chars_format::general && abs_value >= min_fractional_value && abs_value < max_fractional_value)
        {
            return to_chars_fixed_impl(first, last, value, fmt, precision);
        }

        if (fmt == chars_format::fixed)
        {
            return to_chars_fixed_impl(first, last, value, fmt, precision);
        }
        else
        {
            return to_chars_scientific_impl(first, last, value, fmt, precision);
        }
    }
}

} //namespace detail

BOOST_DECIMAL_EXPORT BOOST_DECIMAL_CONSTEXPR auto to_chars(char* first, char* last, decimal32 value) noexcept -> to_chars_result
{
    return detail::to_chars_impl(first, last, value);
}

BOOST_DECIMAL_EXPORT BOOST_DECIMAL_CONSTEXPR auto to_chars(char* first, char* last, decimal32 value, chars_format fmt) noexcept -> to_chars_result
{
    return detail::to_chars_impl(first, last, value, fmt);
}

BOOST_DECIMAL_EXPORT BOOST_DECIMAL_CONSTEXPR auto to_chars(char* first, char* last, decimal32 value, chars_format fmt, int precision) noexcept -> to_chars_result
{
    if (precision < 0)
    {
        precision = 6;
    }

    return detail::to_chars_impl(first, last, value, fmt, precision);
}

BOOST_DECIMAL_EXPORT BOOST_DECIMAL_CONSTEXPR auto to_chars(char* first, char* last, decimal32_fast value) noexcept -> to_chars_result
{
    return detail::to_chars_impl(first, last, value);
}

BOOST_DECIMAL_EXPORT BOOST_DECIMAL_CONSTEXPR auto to_chars(char* first, char* last, decimal32_fast value, chars_format fmt) noexcept -> to_chars_result
{
    return detail::to_chars_impl(first, last, value, fmt);
}

BOOST_DECIMAL_EXPORT BOOST_DECIMAL_CONSTEXPR auto to_chars(char* first, char* last, decimal32_fast value, chars_format fmt, int precision) noexcept -> to_chars_result
{
    if (precision < 0)
    {
        precision = 6;
    }

    return detail::to_chars_impl(first, last, value, fmt, precision);
}

BOOST_DECIMAL_EXPORT BOOST_DECIMAL_CONSTEXPR auto to_chars(char* first, char* last, decimal64 value) noexcept -> to_chars_result
{
    return detail::to_chars_impl(first, last, value);
}

BOOST_DECIMAL_EXPORT BOOST_DECIMAL_CONSTEXPR auto to_chars(char* first, char* last, decimal64 value, chars_format fmt) noexcept -> to_chars_result
{
    return detail::to_chars_impl(first, last, value, fmt);
}

BOOST_DECIMAL_EXPORT BOOST_DECIMAL_CONSTEXPR auto to_chars(char* first, char* last, decimal64 value, chars_format fmt, int precision) noexcept -> to_chars_result
{
    if (precision < 0)
    {
        precision = 6;
    }

    return detail::to_chars_impl(first, last, value, fmt, precision);
}

BOOST_DECIMAL_EXPORT BOOST_DECIMAL_CONSTEXPR auto to_chars(char* first, char* last, decimal128 value) noexcept -> to_chars_result
{
    return detail::to_chars_impl(first, last, value);
}

BOOST_DECIMAL_EXPORT BOOST_DECIMAL_CONSTEXPR auto to_chars(char* first, char* last, decimal128 value, chars_format fmt) noexcept -> to_chars_result
{
    return detail::to_chars_impl(first, last, value, fmt);
}

BOOST_DECIMAL_EXPORT BOOST_DECIMAL_CONSTEXPR auto to_chars(char* first, char* last, decimal128 value, chars_format fmt, int precision) noexcept -> to_chars_result
{
    if (precision < 0)
    {
        precision = 6;
    }

    return detail::to_chars_impl(first, last, value, fmt, precision);
}

template <typename T>
struct limits
{
    BOOST_DECIMAL_ATTRIBUTE_UNUSED static constexpr int max_chars = boost::decimal::detail::max_string_length_v<T>;
};

#if !(defined(__cpp_inline_variables) && __cpp_inline_variables >= 201606L) && (!defined(_MSC_VER) || _MSC_VER != 1900)

template <typename T> BOOST_DECIMAL_ATTRIBUTE_UNUSED constexpr int limits<T>::max_chars;

#endif

} //namespace decimal
} //namespace boost

#endif

#endif //BOOST_DECIMAL_CHARCONV_HPP
