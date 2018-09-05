#pragma once

#include "forward_declarations.hpp"
#include "activation.hpp"

// #include <iostream>

namespace neural_network_tools {

    template <size_t S = 1, bool B = false, size_t W = 0, size_t E = 0>
    struct base_cluster {
        static constexpr const size_t size { S };
        static constexpr const size_t weights_size { W };
        static constexpr const size_t errors_size { E };
        static constexpr const bool bias { B };

        static constexpr void check(state_t *const s __attribute__((unused)), error_t *const e __attribute__((unused))) {}
    };

    /**
     * @brief A simple non-recursive neuron cluster model.
     * 
     * Multiple (heterogenous) clusters can exist in one layer, a cluster can
     * never span multiple layers.
     * 
     * @tparam S    Size
     * @tparam TA   Activation function ID
     * @tparam B    Add 1 bias neuron with fixed value `1` to the layer?
     * @tparam C    Clear the accumulator (input) on every forward pass?
     */
    template <size_t S, activation_e TA = PASSTHROUGH, bool B = true, bool C = true>
    struct simple : public base_cluster<S, B> {
        using base = base_cluster<S, B>;
        constexpr operator base&() noexcept { return *static_cast<base *const>(this); }
        constexpr operator const base&() const noexcept { return *static_cast<const base *const>(this); }

        static constexpr void activate(accumulator_t *const a, state_t *const s, weight_t *const w __attribute__((unused))) noexcept {
            for (size_t i = 0; i < S; ++i) {
                // std::cout << "Activating input  (" << &s[i] << " <-- " << &a[i] << ") " << s[i] << " <-- " << a[i] << ": ";
                s[i] = activation<TA>::run(a[i]);
                // std::cout << s[i] << '\n';
            }
            if constexpr (C) {
                // Doing the wipe loop separately helps the compiler optimize.
                // Can't use memset, as it might clear status bits of custom
                // floating / fixed point types!
                for (size_t i = 0; i < S; ++i) {
                    a[i] = 0;
                }
            }
        }
    };

    /**
     * @brief Simple neuron cluster with defaults suitable for input neurons.
     * 
     * @tparam S  Size
     * @tparam TA Activation function ID
     */
    template <size_t S, activation_e TA = PASSTHROUGH>
    using input = simple<S, TA, true, false>; // Inputs don't clear their accumulators by default

    /**
     * @brief Simple neuron cluster with defaults suitable for output neurons.
     * 
     * @tparam S  Size
     * @tparam TA Activation function ID
     */
    template <size_t S, activation_e TA = PASSTHROUGH>
    using output = simple<S, TA, false, true>; // Outputs don't need bias (it's ignored anyway, but adds dead weights)

    /**
     * @brief Recurrent neuron cluster, based on the GRU model.
     * 
     * @tparam S    Size
     * @tparam TA   Activation function ID
     * @tparam TRA  Activation function ID for reset gate
     * @tparam TUA  Activation function ID for update gate
     * @tparam GB   Add bias weights to each gate?
     * @tparam B    Add 1 bias neuron with fixed value `1` to the layer?
     * @tparam C    Clear the accumulator (input) on every forward pass?
     */
    template <size_t S,
              activation_e TA = TANH,
              activation_e TRA = FAST_SIGMOID,
              activation_e TUA = FAST_SIGMOID,
              bool GB = false,
              bool B = true,
              bool C = true>
    struct gru : public base_cluster<S, B, (GB ? 9 : 6) * S> { // GRU, but in SoA layout. Is about 20% faster.
        using base =    base_cluster<S, B, (GB ? 9 : 6) * S>;
        constexpr operator base&() noexcept { return *static_cast<base *const>(this); }
        constexpr operator const base&() const noexcept { return *static_cast<const base *const>(this); }

        static constexpr void activate(accumulator_t *const a, state_t *const s, weight_t *const w) noexcept {
            auto *_w = w;
            for (size_t i = 0; i < S; ++i) {
                // std::cout << "Activating GRU    (" << &s[i] << " <-- " << &a[i] << ") " << s[i] << " <-- " << a[i] << ": ";
                if constexpr (GB) {
                    const auto reset_gate  = activation<TRA>::run(_w[0] * a[i] + _w[1] * s[i]);
                    const auto update_gate = activation<TUA>::run(_w[2] * a[i] + _w[3] * s[i]);
                    const auto new_state   = activation<TA >::run(_w[4] * a[i] + _w[5] * (reset_gate * s[i]));
                    _w += 6;
                    s[i] = (1 - update_gate) * s[i] + update_gate * new_state;
                } else {
                    const auto reset_gate  = activation<TRA>::run(_w[0] * a[i] + _w[1] * s[i] + _w[2]);
                    const auto update_gate = activation<TUA>::run(_w[3] * a[i] + _w[4] * s[i] + _w[5]);
                    const auto new_state   = activation<TA>::run(_w[6] * a[i] + _w[7] * (reset_gate * s[i]) + _w[8]);
                    _w += 9;
                    s[i] = (1 - update_gate) * s[i] + update_gate * new_state;
                }
                // std::cout << s[i] << '\n';
            }
            if constexpr (C) {
                for (size_t i = 0; i < S; ++i) {
                    a[i] = 0;
                }
            }
        }
    };

    // TODO: Add a few GRU variants with less internal weights: https://arxiv.org/pdf/1701.05923.pdf
}