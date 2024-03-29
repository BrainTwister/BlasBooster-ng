#pragma once

#include "Memory.h"
#include "StripedIterator.h"
#include "Utilities.h"
#include <algorithm>
#include <initializer_list>
#include <stdexcept>

namespace BlasBooster {

/// Fixed size storage class for dense arrays on stack.
template <class T, bool OnStack, bool isFixed, size_t Size, bool Strided = false>
class Storage
{
public:

    typedef Storage<T,OnStack,isFixed,Size,Strided> self;
    typedef T* pointer;
    typedef T const* const_pointer;
    typedef __gnu_cxx::__normal_iterator<pointer,self> iterator;
    typedef __gnu_cxx::__normal_iterator<const_pointer,self> const_iterator;

    Storage(size_t = 0)
     : data_()
    {
        debug_print("Storage (onStack): Default constructor is called.");
    }

    /// Copy constructor
    Storage(self const& other)
     : data_()
    {
        debug_print("Storage (onStack): Copy constructor is called.");
        std::copy(other.begin(), other.end(), begin());
    }

    /// Copy assignment operator
    Storage& operator = (self const& rhs)
    {
         if ( this != &rhs ) {
             std::copy_n(rhs.begin(), rhs.size(), this->begin());
         }
         return *this;
    }

    /// Initializer list constructor
    Storage(std::initializer_list<T> values)
     : data_()
    {
        debug_print("Storage (onStack): Initializer list constructor is called.");
        size_t i(0);
        for (auto val : values) data_[i++] = val;
    }

    bool operator == ( const self& rhs ) const {
        return data_ == rhs.data_ and size_ == rhs.size_;
    }

    template <class T2, bool onStack2, bool isFixed2, size_t Size2, bool Strided2>
    bool equal(Storage<T2,onStack2,isFixed2,Size2,Strided2> const& rhs) const
    {
        if ( size_ != rhs.size_ ) return false;
        for ( size_t i(0); i != size_; ++i ) {
            if ( !equalWithinNumericalAccuracy(data_[i],rhs.data_[i]) ) return false;
        }
        return true;
    }

    template <class T2, bool onStack2, bool isFixed2, size_t Size2, bool Strided2>
    bool notEqual(Storage<T2,onStack2,isFixed2,Size2,Strided2> const& rhs) const
    {
        return !equal(rhs);
    }

    iterator begin() {
        return iterator(data_);
    }

    const_iterator begin() const {
        return const_iterator(data_);
    }

    iterator end() {
        return iterator(data_ + size_);
    }

    const_iterator end() const {
        return const_iterator(data_ + size_);
    }

    size_t size() const {
        return size_;
    }

    pointer getDataPointer() {
        return data_;
    }

    const_pointer getDataPointer() const {
        return data_;
    }

private:

    template <class T2, bool onStack2, bool isFixed2, size_t Size2, bool Strided2>
    friend class Storage;

    T data_[Size];

    static const size_t size_ = Size;

};

/**
 * /brief Fixed size storage class for dense arrays on heap.
 */
template <class T, size_t Size>
class Storage<T,false,true,Size,false>
{
public:

    typedef Storage<T,false,true,Size,false> self;
    typedef T* pointer;
    typedef T const* const_pointer;
    typedef __gnu_cxx::__normal_iterator<pointer,self> iterator;
    typedef __gnu_cxx::__normal_iterator<const_pointer,self> const_iterator;

    /// Default constructor
    Storage( size_t = 0 )
     : data_(new T[size_]), ownMemory_(true)
    {
        debug_print("Storage (fixed,onHeap): Default constructor is called.");
    }

    /// Initializer list constructor
    Storage( std::initializer_list<T> values )
     : data_(new T[size_]), ownMemory_(true)
    {
        debug_print("Storage (fixed,onHeap): Initializer list constructor is called.");
        if ( values.size() != size_ ) throw std::runtime_error("Storage (fixed,onHeap): values.size() != size_");
        size_t i(0);
        for ( auto val : values ) data_[i++] = val;
    }

    /// Copy constructor
    Storage( const Storage& rhs )
     : data_(new T[size_]), ownMemory_(true)
    {
        debug_print("Storage (fixed,onHeap): Copy constructor is called.");
        std::copy(rhs.begin(),rhs.end(),begin());
    }

    /// Move constructor
    Storage( Storage&& rhs )
     : data_(rhs.data_), ownMemory_(rhs.ownMemory_)
    {
        debug_print("Storage (fixed,onHeap): Move constructor is called.");
        rhs.ownMemory_ = false;
    }

    /// Conversion constructor
    template <class T2, bool onStack2, bool isFixed2, size_t Size2, bool Strided2>
    Storage( const Storage<T2,onStack2,isFixed2,Size2,Strided2>& rhs )
     : data_(new T[size_]), ownMemory_(true)
    {
        debug_print("Storage (fixed,onHeap): Conversion constructor is called.");
        if ( rhs.size_ != size_ ) throw std::runtime_error("Storage (fixed,onHeap): rhs.size_ != size_");
        std::copy(rhs.begin(),rhs.end(),begin());
    }

    /// Proxy constructor using external memory
    Storage( T* ptrExternalData, size_t size )
     : data_(ptrExternalData), ownMemory_(false)
    {}

    ~Storage() {
        if (ownMemory_) delete [] data_;
    }

    /// Copy assignment
    Storage& operator = ( const self& rhs ) {
        debug_print("Storage (fixed,onHeap): Copy assignment is called.");
        if ( this != &rhs ) {
            if ( rhs.size_ != size_ ) throw std::runtime_error("Storage (fixed,onHeap): rhs.size_ != size_");
            std::copy_n(rhs.begin(),rhs.size_,begin());
            ownMemory_ = true;
        }
        return *this;
    }

    /// Move assignment
    Storage& operator = ( self&& rhs ) noexcept {
        debug_print("Storage (fixed,onHeap): Move assignment is called.");
        swap(*this,rhs);
        return *this;
    }

    friend void swap( self& a, self& b ) noexcept {
        using std::swap; // bring in swap for built-in types
        swap(a.data_,b.data_);
        swap(a.ownMemory_,b.ownMemory_);
    }

    bool operator == ( const Storage& rhs ) const {
        return data_ == rhs.data_ and ownMemory_ == rhs.ownMemory_;
    }

    template < class T2, bool onStack2, bool isFixed2, size_t Size2 >
    bool equal( const Storage<T2,onStack2,isFixed2,Size2>& rhs ) const {
        if ( size_ != rhs.size_ ) return false;
        for ( size_t i(0); i != size_; ++i ) {
            if ( !equalWithinNumericalAccuracy(data_[i],rhs.data_[i]) ) return false;
        }
        return true;
    }

    template < class T2, bool onStack2, bool isFixed2, size_t Size2 >
    bool notEqual( const Storage<T2,onStack2,isFixed2,Size2>& rhs ) const {
        return !equal(rhs);
    }

    iterator begin() {
        return iterator(data_);
    }

    const_iterator begin() const {
        return const_iterator(data_);
    }

    iterator end() {
        return iterator(data_ + size_);
    }

    const_iterator end() const {
        return const_iterator(data_ + size_);
    }

    size_t size() const {
        return size_;
    }

    pointer getDataPointer() {
        return data_;
    }

    const_pointer getDataPointer() const {
        return data_;
    }

protected:

    template <class T2, bool onStack2, bool isFixed2, size_t Size2, bool Strided2>
    friend class Storage;

    T* data_;

    static const size_t size_ = Size;

    /// If true memory is administrated by own class. Otherwise external memory is used.
    bool ownMemory_;

};

/**
 * /brief Flexible size class for dense arrays on heap.
 */
template <class T>
class Storage<T,false,false,0,false>
{
public:

    typedef Storage<T,false,false,0,false> self;
    typedef T* pointer;
    typedef T const* const_pointer;
    typedef __gnu_cxx::__normal_iterator<pointer,self> iterator;
    typedef __gnu_cxx::__normal_iterator<const_pointer,self> const_iterator;

    /// Default constructor
    Storage()
     : data_(nullptr), size_(0), ownMemory_(true)
    {
        debug_print("Storage (onHeap): Default constructor is called.");
    }

    /// Parameter constructor
    Storage(size_t size)
     : data_(allocate<T>(size)), size_(size), ownMemory_(true)
    {
        debug_print("Storage (onHeap): Parameter constructor is called.");
    }

    /// Initializer list constructor
    Storage(std::initializer_list<T> values)
     : data_(allocate<T>(values.size())), size_(values.size()), ownMemory_(true)
    {
        debug_print("Storage (onHeap): Initializer list constructor is called.");
        size_t i(0);
        for ( auto val : values ) data_[i++] = val;
    }

    /// Copy constructor
    Storage(Storage const& rhs)
     : data_(allocate<T>(rhs.size_)), size_(rhs.size_), ownMemory_(rhs.ownMemory_)
    {
        debug_print("Storage (onHeap): Copy constructor is called.");
        std::copy(rhs.begin(), rhs.end(), begin());
    }

    /// Move constructor
    Storage(Storage&& rhs)
     : data_(rhs.data_), size_(rhs.size_), ownMemory_(rhs.ownMemory_)
    {
        debug_print("Storage (onHeap): Move constructor is called.");
        rhs.data_ = nullptr;
        rhs.size_ = 0;
        rhs.ownMemory_ = false;
    }

    /// Conversion constructor
    template <class T2, bool onStack2, bool isFixed2, size_t Size2, bool Strided2>
    Storage(Storage<T2,onStack2,isFixed2,Size2,Strided2> const& rhs)
     : data_(allocate<T>(rhs.size_)), size_(rhs.size_), ownMemory_(true)
    {
        debug_print("Storage (onHeap): Conversion constructor is called.");
        std::copy(rhs.begin(),rhs.end(),begin());
    }

    /// Proxy constructor using external memory
    Storage(T* ptrExternalData, size_t size)
     : data_(ptrExternalData), size_(size), ownMemory_(false)
    {}

    ~Storage() {
        if (ownMemory_ and data_) deallocate<T>(data_);
    }

    void resize(size_t size) {
        if (size_ == size) return;
        if (!ownMemory_) throw std::runtime_error("resize storage with not own memory.");
        if (data_) deallocate<T>(data_);
        data_ = allocate<T>(size);
        size_ = size;
    }

    /// Copy assignment
    Storage& operator = ( const self& rhs ) {
        debug_print("Storage (onHeap): Copy assignment is called.");
        if ( this != &rhs ) {
            if (ownMemory_) deallocate<T>(data_);
            if (rhs.ownMemory_) {
                data_ = allocate<T>(rhs.size_);
                std::copy(rhs.begin(),rhs.end(),begin());
            } else {
                data_ = rhs.data_;
            }
            size_ = rhs.size_;
            ownMemory_ = rhs.ownMemory_;
        }
        return *this;
    }

    /// Move assignment
    Storage& operator = ( self&& rhs ) noexcept {
        debug_print("Storage (onHeap): Move assignment is called.");
        // clear own resources
        if (ownMemory_) deallocate<T>(data_);
        // steal other resources
        data_ = rhs.data_;
        size_ = rhs.size_;
        ownMemory_ = rhs.ownMemory_;
        // clean other resources
        rhs.data_ = nullptr;
        rhs.size_ = 0;
        rhs.ownMemory_ = false;
        return *this;
    }

    friend void swap( self& a, self& b ) noexcept
    {
        using std::swap; // bring in swap for built-in types
        swap(a.data_,b.data_);
        swap(a.size_,b.size_);
        swap(a.ownMemory_,b.ownMemory_);
    }

    bool operator == (Storage const& rhs) const
    {
        return data_ == rhs.data_
            and size_ == rhs.size_
            and ownMemory_ == rhs.ownMemory_;
    }

    template <class T2, bool onStack2, bool isFixed2, size_t Size2>
    bool equal(Storage<T2,onStack2,isFixed2,Size2> const& rhs) const
    {
        if (size_ != rhs.size_) return false;
        for (size_t i(0); i != size_; ++i) {
            if (!equalWithinNumericalAccuracy(data_[i],rhs.data_[i])) return false;
        }
        return true;
    }

    template <class T2, bool onStack2, bool isFixed2, size_t Size2>
    bool notEqual(Storage<T2,onStack2,isFixed2,Size2> const& rhs) const
    {
        return !equal(rhs);
    }

    iterator begin() {
        return iterator(data_);
    }

    const_iterator begin() const {
        return const_iterator(data_);
    }

    iterator end() {
        return iterator(data_ + size_);
    }

    const_iterator end() const {
        return const_iterator(data_ + size_);
    }

    size_t size() const {
        return size_;
    }

    pointer getDataPointer() {
        return data_;
    }

    const_pointer getDataPointer() const {
        return data_;
    }

    void fill(T const& value) {
        std::fill(data_, data_ + size_, value);
    }

protected:

    template <class T2, bool onStack2, bool isFixed2, size_t Size2, bool Strided2>
    friend class Storage;

    T* data_;

    size_t size_;

    /// If true memory is administrated by own class. Otherwise external memory is used.
    bool ownMemory_;

};

/// Flexible size class for striped arrays on heap
/// Only available as proxy storage using external memory.
template <class T>
class Storage<T,false,false,0,true>
{
public:

    typedef Storage<T,false,false,0,true> self;
    typedef T* pointer;
    typedef T const* const_pointer;
    typedef StripedIterator<T> iterator;
    typedef StripedIterator<const T> const_iterator;

    /// Default constructor
    Storage()
     : data_(nullptr), size_(0),
       continuousSize_(0), nbBlocks_(0), separatorSize_(0)
    {
        debug_print("Storage<T,false,false,0,true>: Default constructor is called.");
    }

    /// Parameter constructor
    Storage(T* ptrExternalData, size_t continuousSize, size_t nbBlocks, size_t separatorSize)
     : data_(ptrExternalData), size_(continuousSize*nbBlocks),
       continuousSize_(continuousSize), nbBlocks_(nbBlocks), separatorSize_(separatorSize)
    {
        debug_print("Storage<T,false,false,0,true>: Parameter constructor is called.");
    }

    /// Copy constructor
    Storage(Storage const& rhs)
     : data_(rhs.data_), size_(rhs.size_),
       continuousSize_(rhs.continuousSize_), nbBlocks_(rhs.nbBlocks_), separatorSize_(rhs.separatorSize_)
    {
        debug_print("Storage<T,false,false,0,true>: Copy constructor is called.");
    }

    ~Storage() {}

    /// Copy assignment
    Storage& operator = (Storage const& rhs)
    {
        debug_print("Storage<T,false,false,0,true>: Copy assignment is called.");
        if ( this != &rhs ) {
            data_ = rhs.data_;
            size_ = rhs.size_;
            continuousSize_ = rhs.continuousSize_;
            nbBlocks_ = rhs.nbBlocks_;
            separatorSize_ = rhs.separatorSize_;
        }
        return *this;
    }

    friend void swap(self& a, self& b) noexcept {
        std::swap(a.data_,b.data_);
        std::swap(a.size_,b.size_);
        std::swap(a.continuousSize_,b.continuousSize_);
        std::swap(a.nbBlocks_,b.nbBlocks_);
        std::swap(a.separatorSize_,b.separatorSize_);
    }

    bool operator == (Storage const& rhs) const
    {
        return data_ == rhs.data_
            and size_ == rhs.size_
            and continuousSize_ == rhs.continuousSize_
            and nbBlocks_ == rhs.nbBlocks_
            and separatorSize_ == rhs.separatorSize_;
    }

//    template <class T2, bool onStack2, bool isFixed2, size_t Size2>
//    bool equal(Storage<T2,onStack2,isFixed2,Size2> const& rhs) const {
//        if ( size_ != rhs.size_ ) return false;
//        typename Storage<T2,onStack2,isFixed2,Size2>::const_iterator iter2(rhs.begin());
//        for ( size_t i(0); i != size_; ++i ) {
//            if ( !equalWithinNumericalAccuracy(data_[i],rhs.data_[i]) ) return false;
//        }
//        return true;
//    }
//
//    template <class T2, bool onStack2, bool isFixed2, size_t Size2>
//    bool notEqual(Storage<T2,onStack2,isFixed2,Size2> const& rhs) const {
//        return !equal(rhs);
//    }

    iterator begin() {
        return iterator(data_, 0, 0, continuousSize_, separatorSize_, nbBlocks_);
    }

    const_iterator begin() const {
        return const_iterator(data_, 0, 0, continuousSize_, separatorSize_, nbBlocks_);
    }

    iterator rbegin() {
        return iterator(data_ + size_ + (nbBlocks_-1) * separatorSize_ - 1, continuousSize_-1, nbBlocks_-1, continuousSize_, separatorSize_, nbBlocks_);
    }

    const_iterator rbegin() const {
        return iterator(data_ + size_ + (nbBlocks_-1) * separatorSize_ - 1, continuousSize_-1, nbBlocks_-1, continuousSize_, separatorSize_, nbBlocks_);
    }

    iterator end() {
        return iterator(data_ + size_ + (nbBlocks_-1) * separatorSize_, continuousSize_, nbBlocks_, continuousSize_, separatorSize_, nbBlocks_);
    }

    const_iterator end() const {
        return const_iterator(data_ + size_ + (nbBlocks_-1) * separatorSize_, continuousSize_, nbBlocks_, continuousSize_, separatorSize_, nbBlocks_);
    }

    iterator rend() {
        return iterator(data_ - 1, continuousSize_, -1, continuousSize_, separatorSize_, nbBlocks_);
    }

    const_iterator rend() const {
        return iterator(data_ - 1, continuousSize_, -1, continuousSize_, separatorSize_, nbBlocks_);
    }

    size_t size() const {
        return size_;
    }

    pointer getDataPointer() {
        return data_;
    }

    const_pointer getDataPointer() const {
        return data_;
    }

protected:

    template <class T2, bool onStack2, bool isFixed2, size_t Size2, bool Strided2>
    friend class Storage;

    template <class T2>
    friend class StripedIterator;

    T* data_;

    size_t size_;

    size_t continuousSize_;

    size_t nbBlocks_;

    size_t separatorSize_;

};

} // namespace BlasBooster
