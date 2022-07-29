#pragma once

#include "Matrix.h"
#include "MatrixBase.h"
#include <memory>

namespace BlasBooster {

struct DynamicMatrix
{
    template <typename M, typename T>
    DynamicMatrix(Matrix<M,T>* ptr) : ptr(ptr) {}
    std::unique_ptr<MatrixBase> ptr;
};

} // namespace BlasBooster
