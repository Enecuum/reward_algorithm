// Header guards used to allow overwriting configuration by doing an earlier
// include with the same guard name.
#ifndef NEURAL_NETWORK_TOOLS_CONFIG_h
#define NEURAL_NETWORK_TOOLS_CONFIG_h

// #include <extra_math/extra_math.hpp>
// #include <cnl/all.h>
// #include <cnl/cmath.h>
// #include <cnl/fixed_point.h>

#include "../extra_math/extra_math.hpp"

#include <array>
#include <cstdint>
#include <cstddef>

namespace neural_network_tools {
    // NOTE: Investigate Intel hardware half float instructions:
    // https://software.intel.com/en-us/articles/performance-benefits-of-half-precision-floats

//     using q6_t  = cnl::fixed_point<int8_t,   -6>;
// #ifndef q7_t
//     using q7_t  = cnl::fixed_point<int8_t,   -7>;
// #endif
//     using q14_t = cnl::fixed_point<int16_t, -14>;
// #ifndef q15_t
//     using q15_t = cnl::fixed_point<int16_t, -15>;
// #endif
//     using q30_t = cnl::fixed_point<int32_t, -30>;
// #ifndef q31_t
//     using q31_t = cnl::fixed_point<int32_t, -31>;
// #endif

    using flp_t = float;
    using accumulator_t = flp_t;
    using state_t = flp_t;
    using weight_t = flp_t;
    using error_t = flp_t;
    // using fip_t = q30_t;
    // using fip_t =  cnl::elastic_number<32, -6>;
    // using accumulator_t = fip_t;
    // using state_t = fip_t;
    // using weight_t = fip_t;
    // using error_t = fip_t;

    using size_t = std::size_t;
    using std::tuple;
    using std::array; // default array type
    // using std::vector;
    using std::tanh;
    using std::sqrt;
    using extra_math::abs;
    using extra_math::min;

    // using cnl::abs;
    // using cnl::sqrt;
    // using cnl::multiply;
    // using cnl::quotient;

    // template <typename T>
    // constexpr auto tanh(const T& val) {
    //     return val;
    //     // const auto _exp = cnl::exp(val * -2);
    //     // return cnl::quotient(1 - _exp, 1 + _exp);
    //     // return (cnl::make_elastic_number(1) - _exp) / (cnl::make_elastic_number(1) + _exp);
    // }
}

#endif
