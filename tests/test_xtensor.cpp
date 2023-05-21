#include <catch2/catch_test_macros.hpp>
#include <xtensor/xarray.hpp>
#include <xtensor/xio.hpp>
#include <xtensor/xview.hpp>

TEST_CASE("xtensor", "[double]")
{
    xt::xarray<double> a1
      {{1.0, 2.0, 3.0},
       {2.0, 5.0, 7.0},
       {2.0, 5.0, 7.0}};

    xt::xarray<double> a2
      {5.0, 6.0, 7.0};

    auto&& r1 = xt::view(a1, 1) + a2;
    CHECK(r1 == xt::xarray<double>{7, 11, 14});

    auto&& r2 = a1 * a1;
    CHECK(r2 == xt::xarray<double>
      {{1,  4,  9},
       {4, 25, 49},
       {4, 25, 49}}    
    );
}
