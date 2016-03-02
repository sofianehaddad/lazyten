#pragma once

#include "Constants.hh"
#include <rapidcheck.h>
#include "../TestConstants.hh"
#include <limits>

namespace rc {
template <typename Scalar>
struct MatrixElement {
    typedef Scalar scalar_type;

    static Gen<scalar_type> matrix_element() {
        using ::linalgwrap::tests::TestConstants;

        // TODO better do this in terms of 2-based numbers
        //      and convert them to decimal later.

        // Define a lambda that returns a new matrix element:
        auto gen_element = [=] {
            typedef long gen_type;

            // Generate an arbitrary value
            const gen_type gen_value = *gen::arbitrary<gen_type>();

            // Bring it to scale between -1 and 1:
            const scalar_type ratio =
                  scalar_type(gen_value) / std::numeric_limits<gen_type>::max();

            // We want a flat growth of exponent around 0, but still we want to
            // reach all smaller and larger exponents eventually
            // => use a polynomial for transformation
            auto transform = [](double x) { return std::pow(x, 5.); };

            // Apply transformation => get modified value between -1 and 1,
            // then multiply with the max entry value
            const scalar_type value =
                  transform(ratio) * TestConstants::max_matrix_entry;

            // If too small set zero
            if (std::fabs(value) < TestConstants::min_matrix_entry) {
                return scalar_type(0.);
            }

            // else return it
            return value;
        };

        return gen::exec(gen_element);
    }
};
}
