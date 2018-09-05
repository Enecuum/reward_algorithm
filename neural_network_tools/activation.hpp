#pragma once

#include "forward_declarations.hpp"

namespace neural_network_tools {
    // Hack to make sure activation functions are easier to optimize for GCC
    enum activation_e {
        PASSTHROUGH,
        SIGMOID,
        FAST_SIGMOID,
        TANH,
        RELU,
        ELU
    };

    template <activation_e A>
    struct activation {
        template <typename T>
        static constexpr const auto& run(const T& val) noexcept { return val; }
        template <typename T>
        static constexpr auto& run(T& val) noexcept { return val; }
    };

    template <>
    struct activation<SIGMOID> {
        template <typename T>
        static constexpr const auto run(const T& val, const T& alpha = 1) noexcept { return 1 / (1 + exp(-val * alpha)); }
        template <typename T>
        static constexpr const auto derivative(const T& val) noexcept { const auto sig = run(val); return sig * (1 - sig); }
    };

    template <>
    struct activation<FAST_SIGMOID> {
        template <typename T>
        static constexpr const auto run(const T& val) noexcept { return 1 / (1 + abs(val)); }
        template <typename T>
        static constexpr const auto run(const T& val, const T& alpha) noexcept { return 0.5 * (val * alpha / (1 + abs(val * alpha))) + 0.5; }
    };

    template <>
    struct activation<TANH> {
        template <typename T>
        static constexpr const auto run(const T& val) noexcept { return tanh(val); }
        template <typename T>
        static constexpr const auto derivative(const T& val) noexcept {
            const auto th = tanh(val);
            return 1.0 - th * th;
        }
    };

    template <>
    struct activation<RELU> {
        template <typename T>
        static constexpr const auto run(const T& val) noexcept { return val > 0 ? val : 0; }
        template <typename T>
        static constexpr const auto derivative(const T& val) noexcept { return val > 0 ? 1 : 0; }
    };
}