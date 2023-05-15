#pragma once

#include "Matrix.h"
#include "MatrixBase.h"
#include <memory>

namespace BlasBooster {

struct DynamicMatrix
{
    template <typename M, typename T>
    DynamicMatrix(Matrix<M,T>* ptr) : ptr(ptr) {}

    auto getTypeIndex() const { return ptr->getTypeIndex(); }

private:

    std::unique_ptr<MatrixBase> ptr;
};

template <typename T, typename... Args>
auto make_dynamic(Args&&... args)
{
    return DynamicMatrix(new T(std::forward<Args>(args)...));
}

} // namespace BlasBooster
