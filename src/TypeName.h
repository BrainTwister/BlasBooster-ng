#pragma once

#include <string>

namespace BlasBooster {

template <class T>
struct TypeName
{};

template <>
struct TypeName<double>
{
    static const std::string value() { return "double"; }
};

template <>
struct TypeName<float>
{
    static const std::string value() { return "float"; }
};

template <>
struct TypeName<size_t>
{
    static const std::string value() { return "size_t"; }
};

template <>
struct TypeName<int>
{
    static const std::string value() { return "int"; }
};

} // namespace BlasBooster
