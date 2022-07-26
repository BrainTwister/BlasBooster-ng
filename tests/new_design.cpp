#include <concepts>
#include <iostream>
#include <memory>

template <typename T>
concept arithmetic = std::floating_point<T> or std::integral<T>;

struct Dense {};
struct MatrixBase
{};

template <typename M, arithmetic T>
struct Matrix
{};

template <arithmetic T>
struct Matrix<Dense, T> : MatrixBase
{
    auto info() const { return std::string("Dense ") + typeid(T).name();}
};

struct DynamicMatrix
{
    template <typename M, typename T>
    DynamicMatrix(std::unique_ptr<Matrix<M,T>>&& ptr) : ptr(std::move(ptr)) {}
    std::unique_ptr<MatrixBase> ptr;
};

template <typename M, arithmetic T1, arithmetic T2>
auto operator * (Matrix<M, T1> const& A, Matrix<M, T2> const& B)
{
    return A;
}

int main()
{
    Matrix<Dense, double> A;
    Matrix<Dense, int> B;
    auto C = A * B;

    std::cout << "A: " << A.info() << std::endl;
    std::cout << "B: " << B.info() << std::endl;
    std::cout << "C: " << C.info() << std::endl;

    DynamicMatrix D{std::make_unique<Matrix<Dense, double>>()};
}
