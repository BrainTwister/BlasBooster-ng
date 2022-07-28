#pragma once

namespace BlasBooster {

template <typename T>
T* allocate(size_t size)
{
    return new T[size];
}

template <typename T>
void deallocate(T* memory) noexcept
{
    delete [] memory;
}

} // namespce BlasBooster
