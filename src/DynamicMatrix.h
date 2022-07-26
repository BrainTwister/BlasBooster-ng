#pragma once

#include "Matrix.h"
#include "MatrixBase.h"
#include <memory>

namespace BlasBooster {

struct DynamicMatrix
{
    template <typename M, typename T>
    DynamicMatrix(std::unique_ptr<Matrix<M,T>>&& ptr) : ptr(std::move(ptr)) {}
    std::unique_ptr<MatrixBase> ptr;
};

} // namespace BlasBooster
