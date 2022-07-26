include_directories(
    ${CONAN_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/src
)

add_executable(
    main
    main.cpp
)

set_property(TARGET main PROPERTY CXX_STANDARD 20)
