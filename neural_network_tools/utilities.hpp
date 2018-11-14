#pragma once

#include "forward_declarations.hpp"
#include <tuple>

#ifndef likely
/// Wrap conditionals in likely to inform the compiler which branch path to optimise.
#define likely(x)   __builtin_expect(!!(x), true)
#endif
#ifndef unlikely
/// Wrap conditionals in unlikely to inform the compiler which branch path to optimise.
#define unlikely(x) __builtin_expect(!!(x), false)
#endif

namespace neural_network_tools {
    template <size_t I, typename... Tp>
    constexpr typename std::enable_if_t<(I == sizeof...(Tp)), void>
    tuple_forward(const tuple<Tp...>&) {}

    template <size_t I = 0, typename... Tp>
    constexpr typename std::enable_if_t<(I < sizeof...(Tp)), void>
    tuple_forward(tuple<Tp...>& a) {
        std::get<I>(a).forward();
        tuple_forward<I + 1, Tp...>(a);
    }

    template <size_t I, typename... Tp>
    constexpr typename std::enable_if_t<(I == (sizeof...(Tp) - 1)), void>
    tuple_linkup(const tuple<Tp...>&) {}

    template <size_t I = 0, typename... Tp>
    constexpr typename std::enable_if_t<(I < (sizeof...(Tp) - 1)), void>
    tuple_linkup(tuple<Tp...>& a) {
        std::get<I>(a).next_layer = &std::get<I+1>(a);
        tuple_linkup<I + 1, Tp...>(a);
    }

    template <typename T, typename = int>
    struct has_errors_size : std::false_type { };

    template <typename T>
    struct has_errors_size <T, decltype((void) T::errors_size, 0)> : std::true_type { };

    template <typename T, typename = int>
    struct has_weights_size : std::false_type { };

    template <typename T>
    struct has_weights_size <T, decltype((void) T::weights_size, 0)> : std::true_type { };


    template <typename T, typename...>
    static constexpr size_t input_count = T::size;

    template <typename T, typename... Ts>
    static constexpr size_t output_count = output_count<Ts...>;

    template <typename T>
    static constexpr size_t output_count<T> = T::size;
}