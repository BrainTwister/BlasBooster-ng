#include <catch2/catch_test_macros.hpp>
#include "DenseMatrix.h"

TEST_CASE("DenseMatrix", "default constructor")
{
    using namespace BlasBooster;

    Matrix<Dense, double> A;

    REQUIRE( A.size() == 0 );
}
