// Copyright 2023 Matt Borland
// Copyright 2023 Christopher Kormanyos
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#if defined(__clang__)
  #if defined __has_feature
  #if __has_feature(thread_sanitizer)
  #define BOOST_DECIMAL_REDUCE_TEST_DEPTH
  #endif
  #endif
#elif defined(__GNUC__)
  #if defined(__SANITIZE_THREAD__)
  #define BOOST_DECIMAL_REDUCE_TEST_DEPTH
  #endif
#elif defined(_MSC_VER)
  #if defined(_DEBUG)
  #define BOOST_DECIMAL_REDUCE_TEST_DEPTH
  #endif
#endif

#include <chrono>
#include <limits>
#include <random>

#include <boost/decimal.hpp>
#include <boost/core/lightweight_test.hpp>

namespace local
{
  template<typename IntegralTimePointType,
           typename ClockType = std::chrono::high_resolution_clock>
  auto time_point() -> IntegralTimePointType
  {
    using local_integral_time_point_type = IntegralTimePointType;
    using local_clock_type               = ClockType;

    const auto current_now =
      static_cast<std::uintmax_t>
      (
        std::chrono::duration_cast<std::chrono::nanoseconds>
        (
          local_clock_type::now().time_since_epoch()
        ).count()
      );

    return static_cast<local_integral_time_point_type>(current_now);
  }

  template<typename NumericType>
  auto is_close_fraction(const NumericType& a,
                         const NumericType& b,
                         const NumericType& tol) -> bool
  {
    using std::fabs;

    auto result_is_ok = bool { };

    if(b == static_cast<NumericType>(0))
    {
      result_is_ok = (fabs(a - b) < tol);
    }
    else
    {
      const auto delta = fabs(1 - fabs(a / b));

      result_is_ok = (delta < tol);
    }

    return result_is_ok;
  }

  typedef struct test_frexp_ldexp_ctrl
  {
    float         float_value_lo;
    float         float_value_hi;
    bool          b_neg;
    std::uint32_t count;
  }
  test_frexp_ldexp_ctrl;

  auto test_frexp_ldexp_impl(const test_frexp_ldexp_ctrl& ctrl, const int eps_tol_factor = 16) -> bool
  {
    using decimal_type = boost::decimal::decimal32;

    std::random_device rd;
    std::mt19937_64 gen(rd());

    gen.seed(time_point<typename std::mt19937_64::result_type>());

    std::uniform_real_distribution<float> dis(ctrl.float_value_lo, ctrl.float_value_hi);

    auto result_is_ok = true;

    auto trials = static_cast<std::uint32_t>(UINT8_C(0));

    for( ; trials < ctrl.count; ++trials)
    {
      auto flt_start = float { };

      do
      {
        flt_start = dis(gen);
      }
      while (flt_start == static_cast<float>(0.0L));

      if(ctrl.b_neg) { flt_start = -flt_start; }

      const auto dec = static_cast<decimal_type>(flt_start);
      const auto flt = static_cast<float>(dec);

      using std::frexp;

      int n_flt;
      const auto frexp_flt = frexp(flt, &n_flt);

      int n_dec;
      const auto frexp_dec = frexp(dec, &n_dec);

      using std::ldexp;

      const auto ldexp_flt = ldexp(frexp_flt, n_flt);
      const auto ldexp_dec = ldexp(frexp_dec, n_dec);

      const auto ldexp_dec_as_float = static_cast<float>(ldexp_dec);

      const auto tol_n =
        static_cast<float>
        (
            static_cast<float>(std::numeric_limits<decimal_type>::epsilon())
          * static_cast<float>(eps_tol_factor)
        );

      const auto result_frexp_ldexp_is_ok = is_close_fraction(ldexp_flt, ldexp_dec_as_float, tol_n);

      if(!result_frexp_ldexp_is_ok)
      {
        std::cout << "Error: frexp/ldexp, for flt: " << flt << std::endl;

        break;
      }

      result_is_ok = (result_frexp_ldexp_is_ok && result_is_ok);
    }

    result_is_ok = ((trials == ctrl.count) && result_is_ok);

    return result_is_ok;
  }

  auto test_frexp_ldexp() -> bool
  {
    #if !defined(BOOST_DECIMAL_REDUCE_TEST_DEPTH)
    constexpr auto test_frexp_ldexp_depth = static_cast<std::uint32_t>(UINT32_C(0x2000));
    #else
    constexpr auto test_frexp_ldexp_depth = static_cast<std::uint32_t>(UINT32_C(0x400));
    #endif

    const local::test_frexp_ldexp_ctrl flt_ctrl[static_cast<std::size_t>(UINT8_C(7))] =
    {
      { 8388606.5F, 8388607.5F, false, static_cast<std::uint32_t>(UINT32_C(0x100)) },
      { -1.0E7F   , +1.0E7F,    false, test_frexp_ldexp_depth },
      { +1.0E-20F , +1.0E-1F,   false, test_frexp_ldexp_depth },
      { +1.0E-20F , +1.0E-1F,   true,  test_frexp_ldexp_depth },
      { +1.0E-28F , +1.0E-26F,  false, static_cast<std::uint32_t>(UINT32_C(0x100)) },
      { +10.0F    , +1.0E12F,   false, test_frexp_ldexp_depth },
      { +10.0F    , +1.0E12F,   true , test_frexp_ldexp_depth },
    };

    auto result_is_ok = true;

    for(const auto& ctrl : flt_ctrl)
    {
      const auto result_test_frexp_ldexp_is_ok = local::test_frexp_ldexp_impl(ctrl);

      BOOST_TEST(result_test_frexp_ldexp_is_ok);

      result_is_ok = (result_test_frexp_ldexp_is_ok && result_is_ok);
    }

    return result_is_ok;
  }

  template<typename FloatingPointType>
  auto test_frexp_ldexp_exact_impl(long double f_in, long double fr_ctrl, int nr_ctrl) -> bool
  {
    using decimal_type = boost::decimal::decimal32;

    using local_float_type = FloatingPointType;

    const auto dec = static_cast<decimal_type>(static_cast<local_float_type>(f_in));

    int n_dec;
    const auto frexp_dec = frexp(dec, &n_dec);

    const auto result_frexp_is_ok =
      (
           (frexp_dec == static_cast<decimal_type>(static_cast<local_float_type>(fr_ctrl)))
        && (n_dec == nr_ctrl)
      );

    auto result_is_ok = result_frexp_is_ok;

    const auto ldexp_dec = ldexp(frexp_dec, n_dec);

    const auto result_ldexp_is_ok = (ldexp_dec == static_cast<decimal_type>(static_cast<local_float_type>(f_in)));

    result_is_ok = (result_ldexp_is_ok && result_is_ok);

    BOOST_TEST(result_is_ok);

    return result_is_ok;
  }

  auto test_frexp_ldexp_exact() -> bool
  {
    // 7.625L, 0.953125L, 3
    auto result_frexp_ldexp_exact_is_ok = true;

    result_frexp_ldexp_exact_is_ok = (test_frexp_ldexp_exact_impl<float>(+7.625L, +0.953125L,  3) && result_frexp_ldexp_exact_is_ok);
    result_frexp_ldexp_exact_is_ok = (test_frexp_ldexp_exact_impl<float>(+0.125L, +0.5L,      -2) && result_frexp_ldexp_exact_is_ok);
    result_frexp_ldexp_exact_is_ok = (test_frexp_ldexp_exact_impl<float>(-0.125L, -0.5L,      -2) && result_frexp_ldexp_exact_is_ok);

    return result_frexp_ldexp_exact_is_ok;
  }

  auto test_frexp_edge() -> bool
  {
    using decimal_type = boost::decimal::decimal32;

    constexpr decimal_type zero {0};

    auto n_dec = int { };
    auto frexp_dec = zero;

    auto result_is_ok = true;

    {
      frexp_dec = frexp(zero, &n_dec);

      const auto result_zero_is_ok = ((frexp_dec == 0) && (n_dec == 0));
      result_is_ok = (result_zero_is_ok && result_is_ok);
      BOOST_TEST(result_is_ok);
    }

    {
      frexp_dec = frexp(std::numeric_limits<decimal_type>::infinity(), &n_dec);

      const auto result_inf_is_ok = (isinf(frexp_dec) && (n_dec == 0));
      result_is_ok = (result_inf_is_ok && result_is_ok);
      BOOST_TEST(result_is_ok);
    }

    {
      frexp_dec = frexp(std::numeric_limits<decimal_type>::quiet_NaN(), &n_dec);

      const auto result_nan_is_ok = (isnan(frexp_dec) && (n_dec == 0));
      result_is_ok = (result_nan_is_ok && result_is_ok);
      BOOST_TEST(result_is_ok);
    }

    return result_is_ok;
  }

  auto test_ldexp_edge() -> bool
  {
    using decimal_type = boost::decimal::decimal32;

    auto result_is_ok = true;

    {
      auto ldexp_dec = ldexp(static_cast<decimal_type>(0.0L), 0);
      auto result_zero_is_ok = (ldexp_dec == 0);

      ldexp_dec = ldexp(static_cast<decimal_type>(0.0L), 3);
      result_zero_is_ok = ((ldexp_dec == 0) && result_zero_is_ok);

      result_is_ok = (result_zero_is_ok && result_is_ok);
      BOOST_TEST(result_is_ok);
    }

    {
      auto ldexp_dec = ldexp(std::numeric_limits<decimal_type>::infinity(), 0);
      auto result_inf_is_ok = isinf(ldexp_dec);

      ldexp_dec = ldexp(std::numeric_limits<decimal_type>::infinity(), 3);
      result_inf_is_ok = (isinf(ldexp_dec) && result_inf_is_ok);

      result_is_ok = (result_inf_is_ok && result_is_ok);
      BOOST_TEST(result_is_ok);
    }

    {
      auto ldexp_dec = ldexp(std::numeric_limits<decimal_type>::quiet_NaN(), 0);
      auto result_nan_is_ok = isnan(ldexp_dec);

      ldexp_dec = ldexp(std::numeric_limits<decimal_type>::quiet_NaN(), 3);
      result_nan_is_ok = (isnan(ldexp_dec) && result_nan_is_ok);

      result_is_ok = (result_nan_is_ok && result_is_ok);
      BOOST_TEST(result_is_ok);
    }

    return result_is_ok;
  }
}

auto main() -> int
{
  auto result_is_ok =
  (
       local::test_frexp_ldexp()
    && local::test_frexp_ldexp_exact()
    && local::test_frexp_edge()
    && local::test_ldexp_edge()
  );

  result_is_ok = ((boost::report_errors() == 0) && result_is_ok);

  return (result_is_ok ? 0 : -1);
}
