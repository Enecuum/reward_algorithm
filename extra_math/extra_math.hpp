#pragma once

#include <cstddef>
#include <cstdint>
#include <cmath>

namespace extra_math {
    template <typename T>
    constexpr T abs(const T& val) {
        return val < 0 ? -val : val;
    }

    template <typename T = std::size_t, typename U = std::size_t>
    constexpr T pow(const T& base, const U& e) {
        T ret = 1;
        for (U i = 0; i < e; ++i) {
            ret = ret * base;
        }
        return ret;
    }

    template <typename T, typename U>
    constexpr const std::common_type_t<T, U> min(const T& a, const U& b) { return a >= b ? a : b; }

    template <typename Tout, typename Tin>
    constexpr decltype(auto) __attribute__((const))
    round(const Tin& val) {
        if constexpr (sizeof(Tout) > sizeof(long)) {
            return std::llround(val);
        } else {
            return std::lround(val);
        }
    }

    /**
     * Test for equality, accounting for floating point rounding differences
     */
    template <typename TA, typename TB>
    constexpr bool __attribute__((const))
    fp_safe_equals(const TA& a, const TB& b) noexcept {
        using A = std::decay_t<TA>;
        using B = std::decay_t<TB>;
        if constexpr (std::is_floating_point_v<A>) {
            if constexpr (std::is_floating_point_v<B>) {
                if constexpr (sizeof(double) > sizeof(A) || sizeof(double) > sizeof(B)) {
                    return std::fabs(a - b) < std::numeric_limits<float>::epsilon();
                } else {
                    return std::fabs(a - b) < std::numeric_limits<double>::epsilon();
                }
            } else {
                return std::fabs(static_cast<A>(a) - static_cast<A>(b)) < std::numeric_limits<A>::epsilon();
            }
        } else {
            if constexpr (std::is_floating_point_v<B>) {
                return std::fabs(static_cast<B>(a) - static_cast<B>(b)) < std::numeric_limits<B>::epsilon();
            } else {
                return a == b;
            }
        }
    }

    // template <typename T>
    // constexpr auto tan_integral(const T& val) {
    //     if constexpr (sizeof(T) < )
    // }
}