#pragma once

#include <iterator>

template <typename T>
struct StripedIterator
{
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using value_type        = T;
    using pointer           = T*;
    using reference         = T&;

    StripedIterator()
     : ptr_(nullptr), position_(0), block_(0), continuousSizeMinus1_(0), separatorSizePlus1_(0), nbBlocks_(0)
    {}

    StripedIterator(T* iter, size_t position, size_t block, size_t continuousSize, size_t separatorSize, size_t nbBlocks)
     : ptr_(iter), position_(position), block_(block),
       continuousSizeMinus1_(continuousSize-1), separatorSizePlus1_(separatorSize+1), nbBlocks_(nbBlocks)
    {}

    template <class T2>
    StripedIterator(StripedIterator<T2> const& other)
     : ptr_(other.ptr_), position_(other.position_), block_(other.block_),
       continuousSizeMinus1_(other.continuousSizeMinus1_), separatorSizePlus1_(other.separatorSizePlus1_), nbBlocks_(other.nbBlocks_)
    {}

    reference operator*() const { return *ptr_; }
    pointer operator->() { return ptr_; }

    // Prefix increment
    StripedIterator& operator++()
    {
        if (position_ == continuousSizeMinus1_) {
            if (static_cast<size_t>(++block_) == nbBlocks_) ++ptr_;
            else ptr_ += separatorSizePlus1_;
            position_ = 0;
        } else {
            ++ptr_;
            ++position_;
        }
        return *this;
    }

    StripedIterator& operator--()
    {
        if (position_ == 0) {
            if (--block_ == -1) --ptr_;
            else ptr_ -= separatorSizePlus1_;
            position_ = continuousSizeMinus1_;
        } else {
            --ptr_;
            --position_;
        }
    }

    StripedIterator operator++(T) { StripedIterator tmp = *this; ++(*this); return tmp; }
    StripedIterator operator--(T) { StripedIterator tmp = *this; --(*this); return tmp; }

    friend bool operator== (const StripedIterator& a, const StripedIterator& b) { return a.ptr_ == b.ptr_; }
    friend bool operator!= (const StripedIterator& a, const StripedIterator& b) { return a.ptr_ != b.ptr_; } 

private:

    pointer ptr_;

    // Current position within the continuous memory space
    size_t position_;

    // Current continuous memory space block
    int block_;

    size_t continuousSizeMinus1_;

    size_t separatorSizePlus1_;

    size_t nbBlocks_;

};
