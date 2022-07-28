set(TestName CoreTest)

include_directories(
    ${CONAN_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/src
)

add_executable(
    ${TestName}
    main.cpp
)

target_link_libraries(
    ${TestName}
    ${CONAN_LIBS}
)

add_test(NAME ${TestName} COMMAND ${TestName})

set_property(TARGET ${TestName} PROPERTY CXX_STANDARD 20)
