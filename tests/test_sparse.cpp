#include <catch2/catch_test_macros.hpp>
#include "SparseMatrix.h"

using namespace BlasBooster;

TEST_CASE("SparseMatrix", "default constructor")
{
    Matrix<Sparse, double> A;
    CHECK(A.getSize() == 0);
}

TEST_CASE("DynamicMatrix", "default constructor")
{
    DynamicMatrix dynA(new Matrix<Dense, double>());
    DynamicMatrix dynB(new Matrix<Dense, float>());

    CHECK(dynA.ptr->getTypeIndex() == 6);
    CHECK(dynB.ptr->getTypeIndex() == 2);
}
