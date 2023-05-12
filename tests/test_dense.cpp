#include <catch2/catch_test_macros.hpp>
#include "DenseMatrix.h"

namespace bb = BlasBooster;

TEST_CASE("DenseMatrix default", "[double]")
{
    bb::Matrix<bb::Dense, double> A;
    CHECK(A.getSize() == 0);
    // CHECK(bb::size(A) == 0);
}

TEST_CASE("DenseMatrix 2x3", "[double]")
{
    bb::Matrix<bb::Dense, double> A(2, 3, [](){});
    CHECK(A.getSize() == 6);
    // CHECK(bb::size(A) == 6);
}
