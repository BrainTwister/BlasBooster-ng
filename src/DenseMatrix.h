#pragma once

#include "DynamicMatrix.h"
#include "DynamicMatrixTypeList.h"
#include "Matrix.h"
#include "MatrixBase.h"
#include "Storage.h"
#include "TypeName.h"
#include "Utilities.h"
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <fstream>
#include <initializer_list>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <typeinfo>

namespace BlasBooster {

/**
 * \brief Column- or row major stored dense matrix.
 */
template <class T, class P>
class Matrix<Dense,T,P>
 : public MatrixBase,
   public P::dimension,
   public P::leadingDimension,
   public P::unblockedDimension,
   public Storage<T,P::onStack,P::isFixed,P::dimension::size,P::isSubMatrix>//,
   //public NormPolicy<Matrix<Dense,T,P>, typename P::NormType>,
   //public OccupationPolicy<Matrix<Dense,T,P>>
{
public: // typedefs

    typedef Matrix<Dense,T,P> self;
    typedef Dense matrix_type;
    typedef T value_type;
    typedef const T const_value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef P parameter;
    typedef typename P::dimension dimension;
    typedef typename P::orientation orientation;
    typedef typename P::leadingDimension leadingDimension;
    typedef typename P::unblockedDimension unblockedDimension;
    typedef typename P::IndexType IndexType;
    typedef Storage<T,P::onStack,P::isFixed,P::dimension::size,P::isSubMatrix> storage;
    typedef typename storage::iterator iterator;
    typedef typename storage::const_iterator const_iterator;

public: // member functions

    /// Default constructor
    Matrix();

    /// Parameter constructor
    Matrix(IndexType nbRows, IndexType nbColumns);

    /// Parameter constructor for BlockedMatrix
    template <class U = T>
    Matrix(IndexType nbRows = 0, IndexType nbColumns = 0,
        IndexType nbUnblockedRows = 0, IndexType nbUnblockedColumns = 0,
        typename std::enable_if<std::is_same<U, DynamicMatrix>::value>::type* = 0);

    /// Parameter constructor using external memory
    Matrix(IndexType nbRows, IndexType nbColumns, T* ptrExternalMemory);

    /// Initializer list constructor
    Matrix(std::initializer_list<std::initializer_list<T>> values);

    /// Initializer constructor for BlockedMatrix
    // template <class U = T>
    // Matrix(std::initializer_list< std::initializer_list<T> > values,
    //     typename std::enable_if<std::is_same<U, DynamicMatrix>::value>::type* = 0);

    /// Submatrix constructor, use storage of reference matrix, ColumnMajor
    template <class T2, class P2, class U = P>
    Matrix(Matrix<Dense,T2,P2> const& other, IndexType nbRows, IndexType nbColumns,
        IndexType beginRow = 0, IndexType beginColumn = 0,
        typename std::enable_if<
            std::is_same<typename P::orientation, ColumnMajor>::value and
            std::is_same<typename P2::orientation, ColumnMajor>::value and
            U::isSubMatrix
        >::type* = 0
    );

    /// Submatrix constructor, use storage of reference matrix, RowMajor
    template <class T2, class P2, class U = P>
    Matrix(Matrix<Dense,T2,P2> const& other, IndexType nbRows, IndexType nbColumns,
        IndexType beginRow = 0, IndexType beginColumn = 0,
        typename std::enable_if<
            std::is_same<typename P::orientation, RowMajor>::value and
            std::is_same<typename P2::orientation, RowMajor>::value and
            U::isSubMatrix
        >::type* = 0
    );

    /// Conversion from DenseMatrix with same orientation
    template <class T2, class P2>
    Matrix(Matrix<Dense,T2,P2> const& other, auto const& checker,
        typename std::enable_if<
            std::is_same<typename P::orientation, typename P2::orientation>::value and
            !P2::isSubMatrix and !P2::isBlockedMatrix
        >::type* = 0
    );

    /// Conversion from DenseMatrix with different orientation
    template <class T2, class P2>
    Matrix(Matrix<Dense,T2,P2> const& other, auto const& checker,
        typename std::enable_if<
            !std::is_same<typename P::orientation, typename P2::orientation>::value and
            !P2::isSubMatrix and !P2::isBlockedMatrix
        >::type* = 0
    );

    /// Conversion from SubDenseMatrix
    template <class T2, class P2>
    Matrix(Matrix<Dense,T2,P2> const& other, auto const& checker,
        typename std::enable_if<
            std::is_same<typename P::orientation, ColumnMajor>::value and
            std::is_same<typename P2::orientation, ColumnMajor>::value and
            P2::isSubMatrix
        >::type* = 0
    );

    /// Conversion from SubDenseMatrix
    template <class T2, class P2>
    Matrix(Matrix<Dense,T2,P2> const& other, auto const& checker,
        typename std::enable_if<
            std::is_same<typename P::orientation, RowMajor>::value and
            std::is_same<typename P2::orientation, RowMajor>::value and
            P2::isSubMatrix
        >::type* = 0
    );

    /// Conversion from blocked DenseMatrix
    template <class T2, class P2>
    Matrix(Matrix<Dense,T2,P2> const& other,
        typename std::enable_if<
            std::is_same<typename P::orientation, ColumnMajor>::value and
            std::is_same<typename P2::orientation, ColumnMajor>::value and
            !P::isSubMatrix and
            P2::isBlockedMatrix
        >::type* = 0
    );

    /// Conversion from DynamicMatrix
    Matrix(DynamicMatrix const& other);

    /// Conversion from SparseMatrix with same orientation
    template <class T2, class P2>
    Matrix(Matrix<Sparse,T2,P2> const& other,
        typename std::enable_if<std::is_same<typename P::orientation, typename P2::orientation>::value>::type* = 0
    );

    /// Conversion from SparseMatrix with different orientation
    template <class T2, class P2>
    Matrix(Matrix<Sparse,T2,P2> const& other,
        typename std::enable_if<!std::is_same<typename P::orientation, typename P2::orientation>::value>::type* = 0
    );

    /// Conversion from ZeroMatrix
    template <class P2>
    Matrix(Matrix<Zero,P2> const& other);

    // /// Conversion from MultipleMatrix
    // template <class X1, class X2>
    // Matrix(MultipleMatrix<X1,X2> const& other);

    // /// Construction by multiplication expression template
    // template <class Op1, class Op2>
    // Matrix(MatrixMultExp<Op1, Op2> const& expression);

    /// Default copy constructor
    Matrix(self const& other) = default;

    /// Move constructor
    Matrix(self&& other)
     : dimension(std::move(other)),
       storage(std::forward<storage>(other))
    {
        debug_print("DenseMatrix: Move constructor was called.");
    }

    /// Default destructor
    ~Matrix() {
        debug_print("DenseMatrix: Destructor was called.");
    }

    /// Default copy assignment operator
    Matrix& operator = (self const& other) = default;

    /// Move assignment operator
    Matrix& operator = (self&& rhs) noexcept {
        debug_print("DenseMatrix: Move assignment operator was called.");
        dimension::operator=(std::forward<dimension>(rhs));
        leadingDimension::operator=(std::forward<leadingDimension>(rhs));
        unblockedDimension::operator=(std::forward<unblockedDimension>(rhs));
        storage::operator=(std::forward<storage>(rhs));
        return *this;
    }

    /// Set all elements to value
    Matrix& operator = (T value);

    //MatrixBase* clone() const { return new self(*this); }

    /// Resize function for non-blocked matrix
    void resize(IndexType nbRows, IndexType nbColumns, auto const& filler);

    /// Resize function for BlockedMatrix
    template <class U = T>
    void resize(IndexType nbRows, IndexType nbColumns,
        IndexType nbUnblockedRows, IndexType nbUnblockedColumns,
        typename std::enable_if<std::is_same<U, DynamicMatrix>::value>::type* = 0);

    bool operator == (self const& rhs) const {
        return storage::operator==(rhs) and dimension::operator==(rhs);
    }

    bool operator != (self const& rhs) const {
        return !operator==(rhs);
    }

    template <class T2, class P2>
    bool equal(Matrix<Dense,T2,P2> const& rhs) const {
        return storage::equal(rhs)
            and this->getNbRows() == rhs.getNbRows()
            and this->getNbColumns() == rhs.getNbColumns();
    }

    template <class T2, class P2>
    bool notEqual(Matrix<Dense,T2,P2> const& rhs) const {
        return !equal(rhs);
    }

    T& operator () (IndexType row, IndexType column);
    const T& operator () (IndexType row, IndexType column) const;

    // TODO: move to dimension
    template <class U = P>
    IndexType getLdColumns(typename std::enable_if<!U::isSubMatrix>::type* = 0) const {
        return this->getNbColumns();
    }

    template <class U = P>
    IndexType getLdColumns(typename std::enable_if<U::isSubMatrix>::type* = 0) const {
        return leadingDimension::getLdColumns();
    }

    template <class U = P>
    IndexType getLdRows(typename std::enable_if<!U::isSubMatrix>::type* = 0) const {
        return this->getNbRows();
    }

    template <class U = P>
    IndexType getLdRows(typename std::enable_if<U::isSubMatrix>::type* = 0) const {
        return leadingDimension::getLdRows();
    }

    const std::type_info& getTypeInfo() const { return typeid(*this); }

    size_t getTypeIndex() const { return typeIndex_; }

    friend void swap( self& a, self& b ) noexcept {
        using std::swap; // bring in swap for built-in types
        swap(a.data_,b.data_);
        swap(a.size_,b.size_);
        swap(a.ownMemory_,b.ownMemory_);
        swap(a.nbRows_,b.nbRows_);
        swap(a.nbColumns_,b.nbColumns_);
        //swap(a.storage,b.storage);
    }

    static const size_t typeIndex_ = GetIndex<self, DynamicMatrixTypeList>::value;

    static const std::string name() { return "Matrix<Dense," + TypeName<T>::value() + ">"; }

private:

    template <class M2, class T2, class P2>
    friend struct Matrix;

    template <class M1, class T1, class P1, class M2, class T2, class P2, class M3, class T3, class P3, class Interface>
    friend struct MultiplicationFunctor;

    template <class U = P>
    size_t getPosition(IndexType row, typename P::IndexType column,
        typename std::enable_if<std::is_same<typename U::orientation, RowMajor>::value and !U::isSubMatrix>::type* = 0) const;

    template <class U = P>
    size_t getPosition(IndexType row, typename P::IndexType column,
        typename std::enable_if<std::is_same<typename U::orientation, RowMajor>::value and U::isSubMatrix>::type* = 0) const;

    template <class U = P>
    size_t getPosition(IndexType row, typename P::IndexType column,
        typename std::enable_if<std::is_same<typename U::orientation, ColumnMajor>::value and !U::isSubMatrix>::type* = 0) const;

    template <class U = P>
    size_t getPosition(IndexType row, typename P::IndexType column,
        typename std::enable_if<std::is_same<typename U::orientation, ColumnMajor>::value and U::isSubMatrix>::type* = 0) const;

}; // class DenseMatrix

template <class T, class P>
Matrix<Dense,T,P>::Matrix()
 : dimension(), storage()
{}

template <class T, class P>
Matrix<Dense,T,P>::Matrix(typename P::IndexType nbRows, typename P::IndexType nbColumns)
 : dimension(nbRows,nbColumns), storage(nbRows*nbColumns)
{}

template <class T, class P>
template <class U>
Matrix<Dense,T,P>::Matrix(typename P::IndexType nbRows, typename P::IndexType nbColumns,
    typename P::IndexType nbUnblockedRows, typename P::IndexType nbUnblockedColumns,
    typename std::enable_if<std::is_same<U, DynamicMatrix>::value>::type*)
 : dimension(nbRows,nbColumns),
   unblockedDimension(nbUnblockedRows,nbUnblockedColumns),
   storage(nbRows*nbColumns)
{}

template <class T, class P>
Matrix<Dense,T,P>::Matrix(typename P::IndexType nbRows, typename P::IndexType nbColumns,
    T* ptrExternalMemory)
 : dimension(nbRows,nbColumns),
   storage(ptrExternalMemory,nbRows*nbColumns)
{}

template <class T, class P>
Matrix<Dense,T,P>::Matrix(std::initializer_list<std::initializer_list<T>> values)
 : dimension(values.size(), values.begin()->size()),
   storage(values.size() * values.begin()->size())
{
    for (auto const& inner : values)
    {
        std::memcpy(this->data_, inner.begin(), inner.size());
    }
}

// template <class T, class P>
// template <class U>
// Matrix<Dense,T,P>::Matrix(std::initializer_list< std::initializer_list<T> > values,
//     typename std::enable_if<std::is_same<U, DynamicMatrix>::value>::type*)
//  : dimension(values.size(),values.begin()->size()),
//    storage(values.size() * values.begin()->size())
// {
//     // typedef Cursor<Matrix<Dense,T,P>, Direction::Row> RowCursor;
//     // typedef Cursor<RowCursor, Direction::Column> ColumnCursor;

//     // auto val1 = values.begin();
//     // typename P::IndexType ubRowsTmp, ubColumnsTmp;
//     // for (RowCursor rowCur(*this,0), rowEnd(*this,this->getNbRows()); rowCur != rowEnd; ++rowCur, ++val1)
//     // {
//     //     auto val2 = val1->begin();
//     //     ubRowsTmp = getNbRows(*val2);
//     //     this->ubRows_ += ubRowsTmp;
//     //     ubColumnsTmp = 0;
//     //     for (ColumnCursor columnCur(*this,rowCur.begin()),
//     //         columnEnd(*this,rowCur.end()); columnCur != columnEnd; ++columnCur, ++val2)
//     //     {
//     //         *columnCur = *val2;
//     //         if (ubRowsTmp != getNbRows(*val2)) throw std::runtime_error("wrong sub-dimension");
//     //         ubColumnsTmp += getNbColumns(*val2);
//     //     }
//     //     this->ubColumns_ = ubColumnsTmp;
//     // }
// }

// Submatrix constructor, use storage of reference matrix, ColumnMajor
template <class T, class P>
template <class T2, class P2, class U>
Matrix<Dense,T,P>::Matrix(Matrix<Dense,T2,P2> const& other, typename P::IndexType nbRows,
    typename P::IndexType nbColumns, typename P::IndexType beginRow, typename P::IndexType beginColumn,
    typename std::enable_if<
        std::is_same<typename P::orientation, ColumnMajor>::value and
        std::is_same<typename P2::orientation, ColumnMajor>::value and
        U::isSubMatrix
    >::type*)
 : dimension(nbRows,nbColumns),
   leadingDimension(other.getNbRows(),other.getNbColumns()),
   // TODO: avoid const_cast
   storage(const_cast<T2*>(other.getDataPointer() + beginRow + beginColumn * other.getNbRows()),
       nbRows, nbColumns, other.getNbRows() - nbRows)
{}

template <class T, class P>
template <class T2, class P2, class U>
Matrix<Dense,T,P>::Matrix(Matrix<Dense,T2,P2> const& other, typename P::IndexType nbRows,
    typename P::IndexType nbColumns, typename P::IndexType beginRow, typename P::IndexType beginColumn,
    typename std::enable_if<
        std::is_same<typename P::orientation, RowMajor>::value and
        std::is_same<typename P2::orientation, RowMajor>::value and
        U::isSubMatrix
    >::type*)
 : dimension(nbRows,nbColumns),
   leadingDimension(other.getNbRows(),other.getNbColumns()),
   storage(
       const_cast<T2*>(other.getDataPointer()) + beginRow * other.getNbColumns() + beginColumn,
       other.getSize() - beginRow * other.getNbColumns() - beginColumn
   )
{}

// Conversion from DenseMatrix with same orientation
template <class T, class P>
template <class T2, class P2, class ValueChecker>
Matrix<Dense,T,P>::Matrix(Matrix<Dense,T2,P2> const& other, ValueChecker const& valueChecker,
    typename std::enable_if<
        std::is_same<typename P::orientation, typename P2::orientation>::value and
        !P2::isSubMatrix and !P2::isBlockedMatrix
    >::type*)
  : dimension(other.getNbRows(), other.getNbColumns()),
    storage(other)
{}

// Conversion from DenseMatrix with different orientation
template <class T, class P>
template <class T2, class P2, class ValueChecker>
Matrix<Dense,T,P>::Matrix(Matrix<Dense,T2,P2> const& other, ValueChecker const& valueChecker,
    typename std::enable_if<
        !std::is_same<typename P::orientation, typename P2::orientation>::value and
        !P2::isSubMatrix and !P2::isBlockedMatrix
    >::type*)
  : dimension(other.getNbRows(), other.getNbColumns()),
    storage(transpose(other))
{}

/// Conversion from SubDenseMatrix
template <class T, class P>
template <class T2, class P2, class ValueChecker>
Matrix<Dense,T,P>::Matrix(Matrix<Dense,T2,P2> const& other, ValueChecker const& valueChecker,
    typename std::enable_if<
        std::is_same<typename P::orientation, ColumnMajor>::value and
        std::is_same<typename P2::orientation, ColumnMajor>::value and
        P2::isSubMatrix
    >::type*)
  : dimension(other.getNbRows(),other.getNbColumns()),
    storage(other.getSize())
{
    typename storage::iterator iterCur(this->begin());
    typename Matrix<Dense,T2,P2>::const_iterator otherIterCur(other.begin()), otherIterEnd(other.end());

    for ( ; otherIterCur != otherIterEnd; ++otherIterCur, ++iterCur) {
        *iterCur = valueChecker(*otherIterCur) ? *otherIterCur : 0.0;
    }
}

/// Conversion from SubDenseMatrix
template <class T, class P>
template <class T2, class P2, class ValueChecker>
Matrix<Dense,T,P>::Matrix(Matrix<Dense,T2,P2> const& other, ValueChecker const& valueChecker,
    typename std::enable_if<
        std::is_same<typename P::orientation, RowMajor>::value and
        std::is_same<typename P2::orientation, RowMajor>::value and
        P2::isSubMatrix
    >::type*)
  : dimension(other.getNbRows(),other.getNbColumns()),
    storage(other.getSize())
{
    typename storage::iterator iterCur(this->begin());
    typename Matrix<Dense,T2,P2>::const_iterator otherIterCur(other.begin());

    typename P2::IndexType leadingDifference(other.getLdColumns() - other.getNbColumns());

    for ( size_t rowCur(0), rowEnd(other.getNbRows()); rowCur != rowEnd; ++rowCur ) {
        for ( size_t columnCur(0), columnEnd(other.getNbColumns()); columnCur != columnEnd; ++columnCur ) {
            *iterCur++ = *otherIterCur++;
        }
        otherIterCur += leadingDifference;
    }
}

// /// Conversion from blocked DenseMatrix
// template <class T, class P>
// template <class T2, class P2>
// Matrix<Dense,T,P>::Matrix(Matrix<Dense,T2,P2> const& other,
//     typename std::enable_if<
//         std::is_same<typename P::orientation, ColumnMajor>::value and
//         std::is_same<typename P2::orientation, ColumnMajor>::value and
//         !P::isSubMatrix and
//         P2::isBlockedMatrix
//     >::type*
// ) : dimension(other.getUnblockedRows(), other.getUnblockedColumns()),
//     storage(other.getUnblockedRows() * other.getUnblockedColumns())
// {
//     typename P::IndexType subRowOffset(0), subRowOffsetTmp(0), subColumnOffset(0);

//     typedef Cursor<const Matrix<Dense,T2,P2>, Direction::Row> RowCursor;
//     typedef Cursor<RowCursor, Direction::Column> ColumnCursor;
//     for (RowCursor rowCur(other,0), rowEnd(other,other.getNbRows()); rowCur != rowEnd; ++rowCur)
//     {
//         for (ColumnCursor columnCur(other,rowCur.begin()),
//             columnEnd(other,rowCur.end()); columnCur != columnEnd; ++columnCur)
//         {
//             typedef Matrix<Dense, T, Parameter<typename P::IndexType, typename P::orientation, typename P::SizeType, LeadingDimension>> SubMatrix;

//             SubMatrix subMatrixOut(*this,getNbRows(*columnCur),getNbColumns(*columnCur),subRowOffset,subColumnOffset);

//             self subMatrixIn(*columnCur);
//             copy(subMatrixIn,subMatrixOut);

//             // TODO: rowOffset is ideally determined one time (all submatrices in a column have the same row size).
//             // But I don't know yet how to determine the offset in the outer loop.
//             subRowOffsetTmp = getNbRows(*columnCur);
//             subColumnOffset += getNbColumns(*columnCur);
//         }
//         subRowOffset += subRowOffsetTmp;
//         subColumnOffset = 0;
//     }
// }

// template <class M>
// struct ConvertToDenseMatrix
// {
//     typedef M result_type;

//     ConvertToDenseMatrix(DynamicMatrix const& dynMatrix)
//      : dynMatrix_(dynMatrix)
//     {}

//     template <class T>
//     result_type operator () (T* = 0) const
//     {
//         M matrix(*std::static_pointer_cast<T>(dynMatrix_));
//         return matrix;
//     }

//     DynamicMatrix dynMatrix_;
// };

// template <class T, class P>
// Matrix<Dense,T,P>::Matrix(DynamicMatrix const& dynMatrix)
//  : dimension(), storage()
// {
//     self matrix = exec_if<DynamicMatrixTypeList>(TypeChecker(dynMatrix->getTypeIndex()),
//         ConvertToDenseMatrix<self>(dynMatrix));
//     swap(*this,matrix);
// }

// Conversion from SparseMatrix with same orientation
template <class T, class P>
template <class T2, class P2>
Matrix<Dense,T,P>::Matrix(Matrix<Sparse,T2,P2> const& other,
    typename std::enable_if<std::is_same<typename P::orientation, typename P2::orientation>::value>::type*)
 : dimension(other.getNbRows(), other.getNbColumns()), storage(other.getSize())
{
    typedef typename Matrix<Sparse,T2,P2>::const_index_iterator ConstIndexIterator;
    typedef typename Matrix<Sparse,T2,P2>::const_iterator ConstIterator;

    this->fill(0.0);

    iterator iterDense = this->begin();
    for (ConstIndexIterator iterOffsetCur(other.beginOffset()),
        iterOffsetEnd(other.endOffset() - 1); iterOffsetCur != iterOffsetEnd; ++iterOffsetCur)
    {
        ConstIterator iterValueCur(other.begin() + *iterOffsetCur);
        for (ConstIndexIterator iterKeyCur(other.beginKey() + *iterOffsetCur),
            iterKeyEnd(other.beginKey() + *(iterOffsetCur + 1)); iterKeyCur != iterKeyEnd; ++iterKeyCur, ++iterValueCur)
        {
            iterDense[*iterKeyCur] = *iterValueCur;
        }
        iterDense += this->getMajorDimension();
    }
}

// Conversion from SparseMatrix with different orientation
template <class T, class P>
template <class T2, class P2>
Matrix<Dense,T,P>::Matrix(Matrix<Sparse,T2,P2> const& other,
    typename std::enable_if<!std::is_same<typename P::orientation, typename P2::orientation>::value>::type*)
 : dimension(other.getNbRows(), other.getNbColumns()), storage(other.getSize())
{
    typedef typename Matrix<Sparse,T2,P2>::const_index_iterator ConstIndexIterator;
    typedef typename Matrix<Sparse,T2,P2>::const_iterator ConstIterator;

    this->fill(0.0);

    iterator iterDense = this->begin();
    for (ConstIndexIterator iterOffsetCur(other.beginOffset()),
        iterOffsetEnd(other.endOffset() - 1); iterOffsetCur != iterOffsetEnd; ++iterOffsetCur)
    {
        ConstIterator iterValueCur(other.begin() + *iterOffsetCur);
        for (ConstIndexIterator iterKeyCur(other.beginKey() + *iterOffsetCur),
            iterKeyEnd(other.beginKey() + *(iterOffsetCur + 1)); iterKeyCur != iterKeyEnd; ++iterKeyCur, ++iterValueCur)
        {
            iterDense[*iterKeyCur * this->getMajorDimension()] = *iterValueCur;
        }
        ++iterDense;
    }
}

// Conversion from ZeroMatrix
template <class T, class P>
template <class P2>
Matrix<Dense,T,P>::Matrix(Matrix<Zero,P2> const& other)
 : dimension(other.getNbRows(), other.getNbColumns()), storage(other.getNbRows() * other.getNbColumns())
{
   AllFiller<T>(0.0)(*this);
}

// // Conversion from MultipleMatrix
// template <class T, class P>
// template <class X1, class X2>
// Matrix<Dense,T,P>::Matrix(MultipleMatrix<X1,X2> const& other)
//  : dimension(other.getNbRows(), other.getNbColumns()), storage(other.getSize())
// {
//     *this = other.getMatrix1();
//     *this += other.getMatrix2();
// }

// // Construction by multiplication expression template
// template <class T, class P>
// template <class Op1, class Op2>
// Matrix<Dense,T,P>::Matrix(MatrixMultExp<Op1, Op2> const& expression)
// {
//     *this = expression.template execute<Native>();
// }

template <class T, class P>
Matrix<Dense,T,P>& Matrix<Dense,T,P>::operator = (T value)
{
    for (iterator iterCur(this->begin()), iterEnd(this->end()); iterCur != iterEnd; ++iterCur)
    {
        *iterCur = value;
    }
    return *this;
}

template <class T, class P>
void Matrix<Dense,T,P>::resize(typename P::IndexType nbRows, typename P::IndexType nbColumns, auto const& filler)
{
    static_cast<dimension*>(this)->resize(nbRows, nbColumns);
    static_cast<storage*>(this)->resize(nbRows * nbColumns);
    filler();
}

template <class T, class P>
template <class U>
void Matrix<Dense,T,P>::resize(typename P::IndexType nbRows, typename P::IndexType nbColumns,
    typename P::IndexType nbUnblockedRows, typename P::IndexType nbUnblockedColumns,
    typename std::enable_if<std::is_same<U, DynamicMatrix>::value>::type*)
{
    this->nbRows_ = nbRows;
    this->nbColumns_ = nbColumns;
    this->ubRows_ = nbUnblockedRows;
    this->ubColumns_ = nbUnblockedColumns;
    static_cast<storage*>(this)->resize(nbRows*nbColumns);
}

template <class T, class P>
T& Matrix<Dense,T,P>::operator () (typename P::IndexType row, typename P::IndexType column)
{
    return this->getDataPointer()[getPosition(row, column)];
}

template <class T, class P>
const T& Matrix<Dense,T,P>::operator () (typename P::IndexType row, typename P::IndexType column) const
{
    return this->getDataPointer()[getPosition(row, column)];
}

template <class T, class P>
template <class U>
size_t Matrix<Dense,T,P>::getPosition(typename P::IndexType row, typename P::IndexType column,
    typename std::enable_if<std::is_same<typename U::orientation, ColumnMajor>::value and !U::isSubMatrix>::type*) const
{
    assert(row < this->getNbRows());
    assert(column < this->getNbColumns());
    return row + column * this->getNbRows();
}

template <class T, class P>
template <class U>
size_t Matrix<Dense,T,P>::getPosition(typename P::IndexType row, typename P::IndexType column,
    typename std::enable_if<std::is_same<typename U::orientation, ColumnMajor>::value and U::isSubMatrix>::type*) const
{
    assert(row < this->getNbRows());
    assert(column < this->getNbColumns());
    return row + column * this->getLdRows();
}

template <class T, class P>
template <class U>
size_t Matrix<Dense,T,P>::getPosition(typename P::IndexType row, typename P::IndexType column,
    typename std::enable_if<std::is_same<typename U::orientation, RowMajor>::value and !U::isSubMatrix>::type*) const
{
    assert(row < this->getNbRows());
    assert(column < this->getNbColumns());
    return row * this->getNbColumns() + column;
}


template <class T, class P>
template <class U>
size_t Matrix<Dense,T,P>::getPosition(typename P::IndexType row, typename P::IndexType column,
    typename std::enable_if<std::is_same<typename U::orientation, RowMajor>::value and U::isSubMatrix>::type*) const
{
    assert(row < this->getNbRows());
    assert(column < this->getNbColumns());
    return row * this->getLdColumns() + column;
}

} // namespace BlasBooster

/// Pretty printing
template <class T, class P>
struct fmt::formatter<BlasBooster::Matrix<BlasBooster::Dense,T,P>>
{
    char presentation = 'f';

    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        auto it = ctx.begin(), end = ctx.end();
        if (it != end && (*it == 'f' || *it == 's')) presentation = *it++;
        if (it != end && *it != '}') throw format_error("invalid format");
        return it;
    }

    template <typename FormatContext>
    auto format(BlasBooster::Matrix<BlasBooster::Dense,T,P> const& matrix, FormatContext& ctx)
    {
        //format_to(ctx.out(), "{}\n", matrix.getNbRows());
        return ctx.out();
    }
};
