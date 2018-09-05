#pragma once

#include "forward_declarations.hpp"
#include "neuron.hpp"

#include <numeric>

// #include <iostream>

namespace neural_network_tools {
    enum error_scaling_e {
        NO_ERROR_SCALING,
        DEVIATION_LIMIT,
        PCT100
    };

    template <size_t>
    struct error_scaling {
        template <typename A, typename B = error_t>
        static constexpr auto run(const A& value, const B& target __attribute__((unused)) = 0) {
            return value;
        }
    };

    template<>
    struct error_scaling<DEVIATION_LIMIT> {
        template <typename A, typename B = error_t>
        static constexpr auto run(const A& value, const B& target = 1, const size_t limit = 2) {
            if (target == 0) {
                if (value >= 1) {
                    return 1;
                } else if (value <= -1) {
                    return -1;
                } else {
                    return value;
                }
            } else {
                if (value > limit * target) {
                    return limit * target;
                } else if (value < target / limit) {
                    return target / limit;
                } else {
                    return value;
                }
            }
        }
    };

    template<>
    struct error_scaling<PCT100> {
        template <typename A, typename B = error_t>
        static constexpr auto run(const A& value, const B& target = 1) {
            if (target == 0) {
                if (value < 0) {
                    return static_cast<A>(-1);
                } else if (value > 0) {
                    return static_cast<A>(1);
                } else {
                    return static_cast<A>(0);
                }
            } else {
                if (value <= (target / 2)) {
                    return static_cast<A>(-1);
                } else if (value >= (target + target)) {
                    return static_cast<A>(1);
                } else {
                    return static_cast<A>(value / target - 1);
                }
            }
        }
    };

    enum error_aggregation_e {
        SUM,
        SUM_OF_SQUARE,
        EUCLIDEAN_DISTANCE,
        CROSS_ENTROPY,
        PSEUDO_HUBER /* https://en.wikipedia.org/wiki/Huber_loss */
    };

    template <error_aggregation_e>
    struct error_aggregation {
        template <size_t N>
        static constexpr auto run(const std::array<error_t, N>& errors) noexcept {
            // return std::reduce(errors.begin(), errors.end());
            return std::accumulate(errors.begin(), errors.end(), 0);
        }
    };

    template <>
    struct error_aggregation<SUM_OF_SQUARE> {
        template <size_t N>
        static constexpr auto run(const std::array<error_t, N>& errors) noexcept {
            error_t t = 0;
            for (const auto& e : errors) t += e * e;
            return t;
        }
    };

    template <>
    struct error_aggregation<EUCLIDEAN_DISTANCE> {
        template <size_t N>
        static constexpr auto run(const std::array<error_t, N>& errors) noexcept {
            error_t t = 0;
            for (const auto& e : errors) t += e * e;
            return sqrt(t);
        }
    };

    template <>
    struct error_aggregation<PSEUDO_HUBER> {
        template <size_t N>
        static constexpr auto run(const std::array<error_t, N>& errors, const error_t slope = 0.5) noexcept {
            error_t t = 1;
            for (const auto& e : errors) {
                const auto s = e / slope;
                t += s * s;
            }
            return slope * slope * (sqrt(t) - 1);
        }
    };



    /**
     * @brief Steer the realised inputs to the target given.
     * 
     * @tparam T Target inputs
     * @tparam U Realisation inputs
     */
    template <typename T, typename U, error_scaling_e ES = PCT100>
    struct steer_to_ideal : public base_cluster<T::size + U::size, T::bias || U::bias, T::weights_size + U::weights_size, U::size + T::errors_size + U::errors_size> {
        using base =               base_cluster<T::size + U::size, T::bias || U::bias, T::weights_size + U::weights_size, U::size + T::errors_size + U::errors_size>;
        constexpr operator base&() noexcept { return *static_cast<base *const>(this); }
        constexpr operator const base&() const noexcept { return *static_cast<const base *const>(this); }

        static constexpr void activate(accumulator_t *const a, state_t *const s, weight_t *const w __attribute__((unused))) noexcept {
            T::activate(a, s, w);
            U::activate(a + T::size, s + T::size, w + T::weights_size);
        }

        static constexpr void check(state_t *const s, error_t *const e) {
            if constexpr (T::size == 1) {
                for (size_t i = 0; i < T::size; ++i) {
                    e[i] = error_scaling<ES>::run((s + 1)[i], s[0]);
                }
            } else {
                // for (size_t i = 0; i < U::size; ++i) {
                //     e[i] = i;
                // }
                for (size_t i = 0; i < T::size; ++i) {
                    // std::cout << "Determining error (" << &s[i] << " <-- " << &(s + T::size)[i] << ") " << s[i] << " <-- " << (s + T::size)[i] << ": ";
                    e[i] = error_scaling<ES>::run((s + T::size)[i], s[i]);
                    // std::cout << e[i] << '\n';
                }
            }
            // T::check(s, e + U::size); // Since we can have 1:many and 1:1 relations, take the reference as offset for any errors underlying
            // U::check(s + T::size, e + U::size + T::errors_size);
        }
    };
}