/**
 * @brief Enecuum difficulty prediction neural network - Simulation setup
 * 
 * @file enecuum.cpp
 * @author Stefan Hamminga <s@stefanhamminga.com>
 * @date 2018-08-20
 * 
 * Basic concept:
 * There is a constant interval (each block cycle) data series for which a few
 * dimensions are given for a cycle t+1 (calculateable). Error rates are
 * deviations from a calculated optimum, the network is trained to predict the
 * next 'controls' (difficulty and reward) from the series.
 */

#include "../neural_network_tools/all.hpp"

#include <iostream>
#include <random>


int main() {
    using namespace neural_network_tools;

    std::default_random_engine e { 1u }; // Will result in the same 'random' generation each compile
    std::uniform_real_distribution<> rnd(-1.0f, 1.0f); // Easy to scale

    network<config<SUM_OF_SQUARE>,
            steer_to_ideal<composite<input<2>, // Target PoW and PoA time
                                     ratio<input<2>>>, // Target PoW and PoA ratio
                           composite<input<2>, // Realised PoW and PoA time
                                     ratio<input<2>>>>, // Realised PoW and PoA ratio
            gru<(2+2+2+2)*(2+2)*5, TANH>, // 5 times inputs * outputs. General rule of thumb is 2-8 times i*o.
            composite<output<2>, // PoW and PoA difficulty
                      ratio<output<2>>> // PoW and PoA reward %
            > net;

    // Pre-programmed targets for algorithm 1.0
    net.inputs[0] = 2.5 * 60; // 2.5 minutes in s target time
    net.inputs[1] = 2.5 * 60; // 2.5 minutes in s target time
    net.inputs[2] = 0.2; // 20% of marks to PoW
    net.inputs[3] = 0.8; // 80% of marks to PoA
    // For now fill the realisation inputs with random data. The last two should
    // internally always add up to 100%, hence the radio filter above
    net.inputs[4] = 2.5 * 60 + rnd(e)*10;
    net.inputs[5] = 2.5 * 60 + rnd(e)*10;
    net.inputs[6] = 0.2 + rnd(e)/10;
    net.inputs[7] = 0.8 + rnd(e)/20;

    for (size_t i = 0; i < net.inputs_size; ++i) {
        std::cout << "Input: " << net.inputs[i] << '\n';
    }

    for (int i = 0; i < 1000'000; ++i) {
        net.activate(); // Predict
        
        // **** Block finding phase ****
        
        // Update inputs
        net.inputs[4] = 2.5 * 60 + rnd(e)*10;
        net.inputs[5] = 2.5 * 60 + rnd(e)*10;
        net.inputs[6] = 0.2 + rnd(e)/10;
        net.inputs[7] = 0.8 + rnd(e)/20;

        net.check(); // Check prediction
        net.train(); // Retrain the network with current error
    }

    for (size_t i = 0; i < net.errors_size; ++i) {
        std::cout << "Error " << i << ": " << net.errors[i] << '\n';
    }
    std::cout << "Error total: " << net.error << '\n';

    for (size_t i = 0; i < net.outputs_size; ++i) {
        std::cout << "Output: " << net.outputs[i] << '\n';
    }
}
