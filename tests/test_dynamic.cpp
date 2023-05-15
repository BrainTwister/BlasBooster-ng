#include <catch2/catch_test_macros.hpp>
#include "DenseMatrix.h"
#include "DynamicMatrix.h"

using namespace BlasBooster;

TEST_CASE("DynamicMatrix", "default constructor")
{
    auto dynA = make_dynamic<Matrix<Dense, double>>(3, 3);
    auto dynB = make_dynamic<Matrix<Dense, float>>(3, 3);

    CHECK(dynA.getTypeIndex() == 6);
    CHECK(dynB.getTypeIndex() == 2);
}
