#include <catch2/catch_test_macros.hpp>
#include "DenseMatrix.h"

using namespace BlasBooster;

TEST_CASE("DenseMatrix default", "[double]")
{
    Matrix<Dense, double> A;
    CHECK(A.getSize() == 0);
    // CHECK(bb::size(A) == 0);
}

TEST_CASE("DenseMatrix 2x3", "[double]")
{
    Matrix<Dense, double> A(2, 3);
    CHECK(A.getSize() == 6);
    // CHECK(bb::size(A) == 6);
}

TEST_CASE("DenseMatrix initializer_list", "[double]")
{
    Matrix<Dense, double> A
    {
        { 2.0,  3.0},
        {-4.0,  1.0},
        { 7.0, -1.0}
    };
    CHECK(A.getSize() == 6);
    // CHECK(A(0, 0) == 2.0);
}
