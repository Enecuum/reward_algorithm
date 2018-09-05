#pragma once

#include "forward_declarations.hpp"

namespace neural_network_tools {

    template <typename... Ts>
    struct composite : base_cluster<(Ts::size + ...), (Ts::bias || ...), (Ts::weights_size + ...), (Ts::errors_size + ...)> {
        using base =   base_cluster<(Ts::size + ...), (Ts::bias || ...), (Ts::weights_size + ...), (Ts::errors_size + ...)>;
        constexpr operator base&() noexcept { return *static_cast<base *const>(this); }
        constexpr operator const base&() const noexcept { return *static_cast<const base *const>(this); }

        static constexpr void activate(accumulator_t *const a, state_t *const s, weight_t *const __attribute__((unused)) w) noexcept {
            // This accounts for offsetting the weight pointer for each
            // activation. Should be CT.
            constexpr size_t ss[] { 0, Ts::size... };
            size_t ws[] { 0, Ts::weights_size... };
            for (size_t i = 2; i < (sizeof...(Ts) - 1); ++i) {
                ws[i] += ws[i-1];
            }
            size_t i = 0;
            ((Ts::activate(a + ss[i], s + ss[i], w + ws[i]), ++i), ...);
        }

        static constexpr void check(state_t *const s, error_t *const e) {
            constexpr size_t ss[] { 0, Ts::size... };
            constexpr size_t es[] { 0, Ts::errors_size... };
            size_t i = 0;
            ((Ts::check(s + ss[i], e + es[i]), ++i), ...);
        }
    };

    template <typename T>
    struct shift_normalise : public T {
        constexpr operator T&() noexcept { return *static_cast<T *const>(this); }
        constexpr operator const T&() const noexcept { return *static_cast<const T *const>(this); }

        constexpr void forward() noexcept {
            T& base = *static_cast<T *const>(this);

            base.forward();

            state_t avg = 0;

            for (size_t i = 0; i < T::size; ++i) {
                avg += base[i] / T::size;
            }

            for (size_t i = 0; i < T::size; ++i) {
                base[i] -= avg;
            }
        }
    };

    template <typename T>
    struct ratio : public T {
        constexpr operator T&() noexcept { return *static_cast<T *const>(this); }
        constexpr operator const T&() const noexcept { return *static_cast<const T *const>(this); }
        
        static constexpr void activate(accumulator_t *const a, state_t *const s, weight_t *const w) noexcept {
            T::activate(a, s, w);

            state_t _min = std::numeric_limits<state_t>::max();
            state_t _sum = 0;

            for (size_t i = 0; i < T::size; ++i) {
                if (unlikely(s[i] < _min)) {
                    _min = s[i];
                }
                _sum += s[i];
            }


            for (size_t i = 0; i < T::size; ++i) {
                s[i] = (s[i] - _min) / (_sum - (T::size * _min));
            }
        }
    };

    // TODO: Verify this implementation
    template <typename T>
    struct softmax : public T {
        constexpr operator T&() noexcept { return *static_cast<T *const>(this); }
        constexpr operator const T&() const noexcept { return *static_cast<const T *const>(this); }
        
        static constexpr void activate(accumulator_t *const a, state_t *const s, weight_t *const w) noexcept {
            T::activate(a, s, w);

            state_t max = std::numeric_limits<state_t>::lowest();

            for (size_t i = 0; i < T::size; ++i) {
                if (unlikely(s[i] > max)) {
                    max = s[i];
                }
            }

            state_t sum = 0;

            for (size_t i = 0; i < T::size; ++i) {
                s[i] = exp(s[i] - max);
                sum += s[i];
            }

            for (size_t i = 0; i < T::size; ++i) {
                s[i] = s[i] / sum;
            }
        }
    };

    // template <typename T, size_t Tag = __COUNTER__>
    // struct temporal_softmax : public T {
    //     static inline neuron_state_t max = std::numeric_limits<neuron_state_t>::lowest();
    //     static inline neuron_state_t sum = 0;

    //     constexpr void forward() {
    //         static_cast<T *const>(this)->forward();

    //         for (auto& n : static_cast<T *const>(this)->neurons) {
    //             if (n.state > max) {
    //                 max = n.state;
    //             }
    //         }

    //         for (auto& n : static_cast<T *const>(this)->neurons) {
    //             n.state = exp(n.state - max);
    //         }

            
    //         for (auto& n : static_cast<T *const>(this)->neurons) {
    //             sum += n.state;
    //         }

    //         for (auto& n : static_cast<T *const>(this)->neurons) {
    //             n.state = n.state / sum;
    //         }
    //     }
    // };
}