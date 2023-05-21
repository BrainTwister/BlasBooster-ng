set(TestName CoreTest)

include_directories(
    ${PROJECT_SOURCE_DIR}/src
)

add_executable(
    ${TestName}
    test_dense.cpp
    test_dynamic.cpp
    test_sparse.cpp
    test_xtensor.cpp
)

target_link_libraries(
    ${TestName}
    Catch2::Catch2WithMain
    fmt::fmt
    xtensor
)

add_test(NAME ${TestName} COMMAND ${TestName})

set_property(TARGET ${TestName} PROPERTY CXX_STANDARD 20)
