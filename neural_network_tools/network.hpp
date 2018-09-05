/**
 * @brief Template based neural network tools
 * 
 * @file network.hpp
 * @author Stefan Hamminga <s@stefanhamminga.com>
 * @date 2016-03-21
 * 
 * @NOTE: Making GCC unroll all loops will reduce performance, selectively
 * unrolling can improve it. Probably need PGO or manual testing.
 */

#pragma once

#include "forward_declarations.hpp"
#include "error_model.hpp"

#include <random>


namespace neural_network_tools {
    /**
     * @brief Aggregation of configuration parameters
     * 
     * @tparam EA Error aggregation method 
     */
    template <error_aggregation_e EA = SUM_OF_SQUARE>
    struct config {
        static constexpr const error_aggregation_e ea {EA};
    };

    /**
     * @brief Can be used to auto determine a hidden layer size, grow the
     * network along with the training dataset.
     * 
     * @tparam Inputs 
     * @tparam Outputs 
     * @tparam Training_samples 
     * @tparam 2 
     */
    template <size_t Inputs, size_t Outputs, size_t Training_samples, size_t Alpha = 2>
    static constexpr size_t hidden_layer_size = Training_samples / (Alpha * (Inputs + Outputs));
    
    template <size_t SI, size_t SO, size_t SS = 1024>
    constexpr auto simple_gru() {
        return network<config<SUM_OF_SQUARE>, input<SI>, gru<hidden_layer_size<SI, SO, SS>>, output<SO>>{};
    }

    /**
     * @brief Neural network
     * 
     * @tparam EA       Error aggregation model
     * @tparam T_layers Layer structure - Each type here represents a layer (group).
     */
    template <typename CFG, typename... T_layers>
    class network {
    private:
        using layers_t = tuple<T_layers...>; // The layers only store meta information and are never instantiated
        using inputs_t = std::tuple_element_t<0, layers_t>;
        using outputs_t = std::tuple_element_t<sizeof...(T_layers) - 1, layers_t>;

        template <size_t c, typename T, typename U, typename... Ts>
        static constexpr size_t count_weights() {
            if constexpr (sizeof...(Ts)) {
                return count_weights<c + (T::size + T::bias) * U::size, U, Ts...>();
            } else {
                return c + (T::size + T::bias) * U::size;
            }
        }

        template <size_t L, size_t N = 0>
        struct size_offset {
            static constexpr const size_t value {
                size_offset<L-1, N + std::tuple_element_t<L-1, layers_t>::size>::value
            };
        };

        template <size_t N>
        struct size_offset<0, N> {
            static constexpr const size_t value { N };
        };

        template <size_t L, size_t N = 0>
        struct external_weight_offset {
            static constexpr const size_t value {
                external_weight_offset<L-1, 
                    N +
                    (std::tuple_element_t<L-1, layers_t>::size +
                     std::tuple_element_t<L-1, layers_t>::bias) *
                    std::tuple_element_t<L, layers_t>::size +
                    std::tuple_element_t<L, layers_t>::weights_size
                >::value
            };
        };

        template <size_t N>
        struct external_weight_offset<0, N> {
            static constexpr const size_t value {
                N + std::tuple_element_t<0, layers_t>::weights_size
            };
        };

        template <size_t L, size_t N = 0>
        struct internal_weight_offset {
            static constexpr const size_t value {
                internal_weight_offset<L-1,
                    N +
                    (std::tuple_element_t<L-1, layers_t>::size +
                     std::tuple_element_t<L-1, layers_t>::bias) *
                    std::tuple_element_t<L, layers_t>::size +
                    std::tuple_element_t<L-1, layers_t>::weights_size
                >::value
            };
        };

        template <size_t N>
        struct internal_weight_offset<0, N> {
            static constexpr const size_t value { N };
        };


        template <size_t L, size_t N = 0>
        struct errors_offset {
            static constexpr const size_t value {
                errors_offset<L-1,
                              N + std::tuple_element_t<L-1, layers_t>::errors_size
                >::value
            };
        };

        template <size_t N>
        struct errors_offset<0, N> {
            static constexpr const size_t value { N };
        };




        template <size_t I, typename... Tp>
        constexpr std::enable_if_t<(I == sizeof...(T_layers)), void>
        activate_next() {}

        template <size_t I = 0, typename... Tp>
        constexpr std::enable_if_t<(I < sizeof...(T_layers)), void>
        activate_next() {
            constexpr const auto so = size_offset<I>::value;
            constexpr const auto iwo = internal_weight_offset<I>::value;
            // std::cout << "Activating layer " << I << ", so=" << so << ", iwo=" << iwo << '\n';
            std::tuple_element_t<I, layers_t>::activate(&accumulators[so],
                                                        &states[so],
                                                        &weights[iwo]);

            if constexpr (I < (sizeof...(T_layers) - 1)) {
                constexpr const auto sol = so + std::tuple_element_t<I, layers_t>::size;
                constexpr const auto ewo = external_weight_offset<I>::value;
                constexpr const auto nso = size_offset<I+1>::value;
                constexpr const auto nsol = nso + std::tuple_element_t<I+1, layers_t>::size;

                size_t k = ewo;
                for (auto i = so; i < sol; ++i) {
                    for (auto j = nso; j < nsol; ++j) {
                        accumulators[j] += states[i] * weights[k++];
                    }
                }
                if (std::tuple_element_t<I, layers_t>::bias) {
                    for (auto j = nso; j < nsol; ++j) {
                        accumulators[j] += weights[k++];
                    }
                }
            }
            activate_next<I + 1>();
        }

        template <size_t I, typename... Tp>
        constexpr std::enable_if_t<(I == sizeof...(T_layers)), void>
        check_next() {}

        template <size_t I = 0, typename... Tp>
        constexpr std::enable_if_t<(I < sizeof...(T_layers)), void>
        check_next() {
            constexpr const auto so = size_offset<I>::value;
            constexpr const auto eo = errors_offset<I>::value;
            std::tuple_element_t<I, layers_t>::check(&states[so],
                                                     &errors[eo]);

            check_next<I + 1>();
        }

    public:
        constexpr network() noexcept {
            set_weights();
        }
        
        static constexpr const size_t inputs_size { inputs_t::size };
        static constexpr const size_t outputs_size { outputs_t::size };

        static constexpr const size_t accumulators_size { (T_layers::size + ...) };
        static constexpr const size_t states_size { (T_layers::size + ...) };
        static constexpr const size_t errors_size { ((has_errors_size<T_layers>::value ? T_layers::errors_size : 0) + ...) };
        static constexpr const size_t external_weights_size { count_weights<0, T_layers...>() };
        static constexpr const size_t internal_weights_size { (T_layers::weights_size + ... ) };
        static constexpr const size_t weights_size { external_weights_size + internal_weights_size };

        std::array<accumulator_t,   accumulators_size>  accumulators {};
        std::array<state_t,         states_size>        states {};
        std::array<error_t,         errors_size>        errors {};
        std::array<weight_t,        weights_size>       weights {};
        error_t error {};
        size_t step { 0 };
        size_t last_checked { 0 };
        size_t last_learned { 0 };

        //TODO: Convert these to use `span`s with proper iterator support
        accumulator_t *const inputs = accumulators.data();
        state_t *const outputs = &states[states_size - outputs_size];

        static constexpr const size_t states_bytes { states_size * sizeof(state_t) };
        static constexpr const size_t weights_bytes { weights_size * sizeof(weight_t) };
        static constexpr const size_t step_bytes { sizeof(decltype(step)) };
        static constexpr const size_t last_checked_bytes { sizeof(decltype(last_checked)) };
        static constexpr const size_t last_learned_bytes { sizeof(decltype(last_learned)) };
        static constexpr const size_t save_bytes { states_bytes + weights_bytes + step_bytes + last_checked_bytes + last_learned_bytes };


        /**
         * @brief Predict the next output values
         */
        constexpr void activate() {
            activate_next();
            ++step;
        }

        /**
         * @brief Determine errors and aggregated error based on current network state.
         * 
         */
        constexpr void check() {
            check_next();
            error = error_aggregation<CFG::ea>::run(errors);
            last_checked = step;
        }

        /**
         * @brief Optimize weights based on current error.
         * 
         */
        constexpr void train() {
            if (step <= last_learned) return; // Don't repeat a learning step
            if (last_checked < step) check(); // Make sure our error data is as actual as possible.
            
            //TODO: Training library port entry point

            last_learned = step;
        }

        constexpr void set_weights() noexcept {
            for (size_t i = 0; i < weights_size; ++i) {
                weights[i] = -1 + i * (2.0 / weights_size);
            }
            // std::default_random_engine e { 1u }; // Will result in the same 'random' generation each compile
            // std::uniform_real_distribution<> rnd(-1.0f, 1.0f);
            // for (auto& w : weights) w = rnd(e);
        }

        constexpr void set_weights(const std::array<weight_t, weights_size>& w) noexcept {
            std::copy(w.begin(), w.end(), weights.begin());
        }

        /**
         * @brief Save the current network state (minus inputs) to a byte buffer.
         * 
         * @param dst   Destination buffer
         * @param free  Buffer free size for check
         * @return constexpr int    bytes written
         */
        constexpr int save(void *const dst, const size_t free = save_bytes) noexcept {
            if (free < save_bytes) return -1;

            std::memcpy(dst, states.data(), states_bytes);
            std::memcpy(dst + states_bytes, weights.data(), weights_bytes);
            std::memcpy(dst + states_bytes + weights_bytes, &step, step_bytes);
            std::memcpy(dst + states_bytes + weights_bytes + step_bytes, &last_checked, last_checked_bytes);
            std::memcpy(dst + states_bytes + weights_bytes + step_bytes + last_checked_bytes, &last_learned, last_learned_bytes);
            return save_bytes;
        }

        /**
         * @brief Restore the network state from a plain buffer.
         * 
         * @param src Source buffer
         */
        constexpr void restore(const void *const src) {
            std::memcpy(states.data(), src, states_bytes);
            std::memcpy(weights.data(), src + states_bytes, weights_bytes);
            std::memcpy(&step, src + states_bytes + weights_bytes, step_bytes);
            std::memcpy(&last_checked, src + states_bytes + weights_bytes + step_bytes, last_checked_bytes);
            std::memcpy(&last_learned, src + states_bytes + weights_bytes + step_bytes + last_checked_bytes, last_learned_bytes);
        }
    };
}