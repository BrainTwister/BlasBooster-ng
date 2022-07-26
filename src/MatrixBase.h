#pragma once

/**
 * \brief Dense and sparse linear algebra operations.
 *
 * BlasBooster search the optimal way for linear algebra operations
 * by dividing the hole matrix in sub-blocks and decide at runtime which
 * algorithm is the optimal one in respect to accuracy and sparsity.
 */
namespace BlasBooster {

/**
 * \brief Abstract base class for all matrix types.
 *
 * Using dynamic polymorphism to decide matrix type at runtime.
 */
struct MatrixBase
{
    virtual ~MatrixBase() {}
};

} // namespace BlasBooster
