#pragma once

#include "DynamicMatrix.h"
#include "DynamicMatrixTypeList.h"
#include "Matrix.h"
#include "MatrixBase.h"
#include "Storage.h"
#include "TypeName.h"
#include "Utilities.h"
#include "SparseStorage.h"

namespace BlasBooster {

/**
 * \brief Column- or Row major stored sparse matrix.
 *
 * Storage:
 * Three arrays are needed, namely the Value-, Key- and OffsetArray.
 * The ValueArray stores the matrix elements consecutively ignoring non-significant elements.
 * The KeyArray stores the columnIndex for columnMajor and the rowIndex for rowMajor
 * matrices, respectively. The OffsetArray stores the position of the first element in a row
 * for columnMajor and in a column for rowMajor matrices, respectively.
 */
template <class T, class P>
class Matrix<Sparse,T,P>
 : public MatrixBase,
   public P::dimension,
   public SparseStorage<T,typename P::IndexType,P::dimension::fixed,P::dimension::size>//,
//    public NormPolicy<Matrix<Sparse,T,P>, typename P::NormType>,
//    public OccupationPolicy<Matrix<Sparse,T,P>>
{
public: // typedefs

    typedef Matrix<Sparse,T,P> self;
    typedef Sparse matrix_type;
    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef P parameter;
    typedef typename P::dimension dimension;
    typedef typename P::leadingDimension leadingDimension;
    typedef typename P::unblockedDimension unblockedDimension;
    typedef typename P::IndexType IndexType;
    typedef SparseStorage<T,typename P::IndexType,P::dimension::fixed,P::dimension::size> storage;
    typedef typename storage::iterator iterator;
    typedef typename storage::const_iterator const_iterator;
    typedef typename storage::index_iterator index_iterator;
    typedef typename storage::const_index_iterator const_index_iterator;

public:

    /// Default constructor
    Matrix();

    /// Parameter constructor
    /// If nbSignificantElements equal zero, storage of size nbRows*nbColumns will be allocated.
    Matrix(IndexType nbRows, IndexType nbColumns, IndexType nbSignificantElements = 0, auto const& filler = [](){});

    /// Conversion from SparseMatrix
    template <class T2, class P2>
    Matrix(Matrix<Sparse,T2,P2> const& other, auto const& checker);

    /// Conversion from DenseMatrix
    /// Elements with an absolute value in the range (lowerThreshold,upperThreshold] are significant.
    /// Significant elements will be counted for ideal storage size.
    template <class T2, class P2>
    Matrix(Matrix<Dense,T2,P2> const& other, auto const& checker);

    /// Conversion from DynamicMatrix
    Matrix(DynamicMatrix const& dynMatrix, double threshold = 0.0);

    // /// Construction by multiplication expression template
    // template <class Op1, class Op2>
    // Matrix(MatrixMultExp<Op1, Op2> const& expression);

    /// Default copy constructor
    Matrix(Matrix const& other) = default;

    /// Move constructor
    Matrix(Matrix&& rhs)
     : dimension(std::forward<dimension>(rhs)),
       storage(std::forward<storage>(rhs))
    {
        debug_print("SparseMatrix: Move constructor was called.");
    }

    /// Default destructor
    ~Matrix() = default;

    /// Default copy assignment operator
    Matrix& operator = (Matrix const& other) = default;

    /// Move assignment operator
    Matrix& operator = (Matrix&& rhs)
    {
        debug_print("SparseMatrix: Move assignment operator was called.");
        dimension::operator=(std::forward<dimension>(rhs));
        storage::operator=(std::forward<storage>(rhs));
        return *this;
    }

    //MatrixBase* clone() const { return new self(*this); }

    /// If nbSignificantElements equal zero, storage of size nbRows*nbColumns will be allocated.
    void resize(IndexType nbRows, IndexType nbColumns, IndexType nbSignificantElements = 0);

    bool operator == (Matrix const& rhs) const {
        return storage::operator==(rhs) and dimension::operator==(rhs);
    }

    const std::type_info& getTypeInfo() const { return typeid(*this); }

    size_t getTypeIndex() const { return typeIndex_; }

    friend void swap(Matrix& a, Matrix& b) noexcept {
        using std::swap; // bring in swap for built-in types
        swap(a.value_,b.value_);
        swap(a.key_,b.key_);
        swap(a.offset_,b.offset_);
        swap(a.nbRows_,b.nbRows_);
        swap(a.nbColumns_,b.nbColumns_);
    }

    static const size_t typeIndex_ = GetIndex<self, DynamicMatrixTypeList>::value;

    static const std::string name() { return "Matrix<Sparse," + TypeName<T>::value() + ">"; }

private:

    template <class M2, class T2, class P2>
    friend struct Matrix;

    template <class M1, class T1, class P1, class M2, class T2, class P2, class M3, class T3, class P3, class Interface>
    friend struct MultiplicationFunctor;

}; // class SparseMatrix

template <class T, class P>
Matrix<Sparse,T,P>::Matrix()
 : dimension(), storage()
{}

template <class T, class P>
template <class Filler>
Matrix<Sparse,T,P>::Matrix(typename P::IndexType nbRows, typename P::IndexType nbColumns,
    typename P::IndexType nbSignificantElements, Filler const& filler)
 : dimension(nbRows, nbColumns),
   storage(nbSignificantElements ? nbSignificantElements : nbRows*nbColumns, nbColumns + 1)
{
    filler(*this);
}

// Conversion from SparseMatrix
template <class T, class P>
template <class T2, class P2, class ValueChecker>
Matrix<Sparse,T,P>::Matrix(Matrix<Sparse,T2,P2> const& other, ValueChecker const& valueChecker)
 : dimension(other.getNbRows(), other.getNbColumns()),
   storage(std::is_same<typename P::orientation, typename P2::orientation>::value ? other : transpose(other))
{}

// // Conversion from DenseMatrix
// template <class T, class P>
// template <class T2, class P2, class ValueChecker>
// Matrix<Sparse,T,P>::Matrix(Matrix<Dense,T2,P2> const& other, ValueChecker const& valueChecker)
//  : dimension(other.getNbRows(), other.getNbColumns()),
//    storage(other.getNbOfSignificantElements(valueChecker), this->getMinorDimension() + 1)
// {
//     iterator iterValueCur(this->begin());
//     index_iterator iterKeyCur(this->beginKey());
//     index_iterator iterOffsetCur(this->beginOffset());

//     typename P::IndexType key, offset(0);

//     typedef typename std::conditional<
//         std::is_same<typename P::orientation, typename P2::orientation>::value,
//         Cursor<const Matrix<Dense,T2,P2>, Direction::Column>,
//         Cursor<const Matrix<Dense,T2,P2>, Direction::Row>
//     >::type OuterCursor;

//     typedef typename std::conditional<
//         std::is_same<typename OuterCursor::direction, Direction::Column>::value,
//         Cursor<OuterCursor, Direction::Row>,
//         Cursor<OuterCursor, Direction::Column>
//     >::type InnerCursor;

//     for (OuterCursor outerCur(other, 0), outerEnd(other, this->getMinorDimension());
//         outerCur != outerEnd; ++outerCur)
//     {
//         *iterOffsetCur++ = offset;
//         key = 0;
//         for (InnerCursor innerCur(other,outerCur.begin()), innerEnd(other,outerCur.end());
//             innerCur != innerEnd; ++innerCur, ++key)
//         {
//             if (valueChecker(*innerCur)) {
//                 *iterValueCur++ = *innerCur;
//                 *iterKeyCur++ = key;
//                 ++offset;
//             }
//         }
//     }
//     *iterOffsetCur++ = offset;
// }

template <class M>
struct ConvertToSparseMatrix
{
    typedef M result_type;

    ConvertToSparseMatrix(DynamicMatrix const& dynMatrix, double threshold)
     : dynMatrix_(dynMatrix), threshold_(threshold)
    {}

    template <class T>
    result_type operator()(T* = 0) const
    {
        return M(*std::static_pointer_cast<T>(dynMatrix_),threshold_);
    }

    DynamicMatrix dynMatrix_;
    double threshold_;
};

// template <class T, class P>
// Matrix<Sparse,T,P>::Matrix(DynamicMatrix const& dynMatrix, double threshold)
// : dimension(), storage()
// {
//     Matrix<Sparse,T,P> matrix = exec_if<TypeList>(TypeChecker(dynMatrix->getTypeIndex()),
//         ConvertToSparseMatrix<self>(dynMatrix,threshold));
//     swap(*this,matrix);
// }

// // Construction by multiplication expression template
// template <class T, class P>
// template <class Op1, class Op2>
// Matrix<Sparse,T,P>::Matrix(MatrixMultExp<Op1, Op2> const& expression)
// {
//     *this = expression.template execute<Native>();
// }

template <class T, class P>
void Matrix<Sparse,T,P>::resize(typename P::IndexType nbRows, typename P::IndexType nbColumns,
    typename P::IndexType nbSignificantElements)
{
    this->nbRows_ = nbRows;
    this->nbColumns_ = nbColumns;
    this->full_size_ = nbRows * nbColumns;
    static_cast<storage*>(this)->resize(nbSignificantElements ? nbSignificantElements : nbRows*nbColumns, nbColumns + 1);
}

} // namespace BlasBooster
