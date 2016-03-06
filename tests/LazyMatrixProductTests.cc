#include "lazy_matrix_test_utils.hh"
#include "rapidcheck_utils.hh"
#include <rapidcheck.h>
#include <catch.hpp>
#include <LazyMatrixProduct.hh>
#include "generators.hh"

namespace linalgwrap {
namespace tests {
using namespace rc;

TEST_CASE("LazyMatrixProduct", "[LazyMatrixProduct]") {
    // Make sure that the program does not get aborted
    linalgwrap::exceptions::assert_dbg_effect =
          linalgwrap::exceptions::ExceptionEffect::THROW;

    // TODO  Test swap function
    // TODO  Test constructors

    typedef double scalar_type;
    typedef SmallMatrix<scalar_type> stored_matrix_type;
    typedef stored_matrix_type model_matrix_type;

    SECTION("Random function test") {
        using namespace matrix_test_utils;

        auto random_test = [] {
            model_matrix_type in(2, 3);
            in(0, 0) = 3;
            in(1, 1) = 2;
            in(0, 1) = -1;
            in(1, 0) = -4;
            in(0, 2) = 1;
            in(1, 2) = -1;

            //
            // The actual test
            //
            // The test library we use
            typedef lazy_matrix_test_utils::TestLibrary<
                  model_matrix_type, LazyMatrixProduct<stored_matrix_type>>
                  test_library;

            // The commands we check
            auto genCommands =
                  state::gen::execOneOf<typename test_library::op_MultiplyLazy,
                                        typename test_library::op_UnaryMinus,
                                        typename test_library::op_MultScalar,
                                        typename test_library::op_DivideScalar>;

            // Run the check:
            test_library().run_check(in, genCommands, 0.25);
        };

        REQUIRE(rc::check("Random function test of LazyMatrixProduct.",
                          random_test));
    }  // Random function test
}

}  // namespace tests
}  // namescpace linalgwrap
