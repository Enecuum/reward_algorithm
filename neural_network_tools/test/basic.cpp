#include "../all.hpp"

#include <iostream>
#include <random>


int main() {
    using namespace neural_network_tools;

    std::default_random_engine e { 1u }; // Will result in the same 'random' generation each compile
    std::uniform_real_distribution<> rnd(-1.0f, 1.0f);

    network<config<SUM_OF_SQUARE>,
            input<3>, // Target PoW and PoA time
            gru<5, TANH>,
            output<2> // PoW, PoA difficulty + PoW, PoA reward %
            > net;

    // Pre-programmed targets for algorithm 1.0
    net.inputs[0] = 2.5 * 60; // 2.5 minutes in s target time
    net.inputs[1] = 2.5 * 60; // 2.5 minutes in s target time
    net.inputs[2] = 0.2; // 20% of marks to PoW

    for (size_t i = 0; i < net.inputs_size; ++i) {
        std::cout << "Input: " << net.inputs[i] << '\n';
    }

    // for (int i = 0; i < 1000'000; ++i) {
    //     net.activate(); // Predict
    //     // **** Block finding phase ****
    //     // Update inputs
    //     net.inputs[4] = 2.5 * 60 + rnd(e)*10;
    //     net.inputs[5] = 2.5 * 60 + rnd(e)*10;
    //     net.inputs[6] = 0.2 + rnd(e)/10;
    //     net.inputs[7] = 0.8 + rnd(e)/20;
    //     net.check(); // Check prediction
    //     net.train(); // Retrain the network with current error
    // }

    net.activate();
    // net.check();

    // for (size_t i = 0; i < net.errors_size; ++i) {
    //     std::cout << "Error " << i << ": " << net.errors[i] << '\n';
    // }
    //     std::cout << "Error total: " << net.error << '\n';

    for (size_t i = 0; i < net.outputs_size; ++i) {
        std::cout << "Output: " << net.outputs[i] << '\n';
    }
}