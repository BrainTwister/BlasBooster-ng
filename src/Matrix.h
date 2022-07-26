#pragma once

#include "EmptyTypes.h"
#include "Parameter.h"
#include <cstddef>

namespace BlasBooster {

/**
 * \brief Primary template for all matrix types.
 *
 * All matrix types are specialized from this template class.
 * M: MatrixType, e.g. Dense, Sparse, Zero
 * T: DataType, which can be a simple build-in type (double, float, int) or again a matrix
 * P: Parameters, which describes the inner structure of the matrix
 */
template <class M, class T = NullType, class P = Parameter<>>
struct Matrix
{};

} // namespace BlasBooster
