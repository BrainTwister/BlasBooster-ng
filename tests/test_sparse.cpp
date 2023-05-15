#include <catch2/catch_test_macros.hpp>
#include "SparseMatrix.h"

using namespace BlasBooster;

TEST_CASE("SparseMatrix", "default constructor")
{
    Matrix<Sparse, double> A;
    CHECK(A.getSize() == 0);
}
