cmake_minimum_required(VERSION 3.13)
project(BayesNet_Framework)


# Compiler settings and flags
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-march=x86-64 -Weverything -Wno-deprecated -arch x86_64")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")


# Set output directories
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


# Bayesnet Library
add_library(bayesnet_lib_static STATIC src/cpt.cpp src/exception.cpp src/factor.cpp src/inference.cpp src/json.cpp src/network.cpp src/node.cpp src/state.cpp src/util.cpp)

target_include_directories(
        bayesnet_lib_static
            PUBLIC
                ${PROJECT_SOURCE_DIR}/include
                /usr/local/include
)

target_link_directories(
        bayesnet_lib_static
            PUBLIC
                /usr/local/lib
)

set_target_properties(
        bayesnet_lib_static
            PROPERTIES
                OUTPUT_NAME bayesnet
)

target_link_libraries(
        bayesnet_lib_static
            gmp
            gmpxx
            dai
)


# Setup gui parts and qt depencies
set(CMAKE_PREFIX_PATH /usr/local/opt/qt)
find_package(Qt5 COMPONENTS Core GUI Widgets)

if(Qt5_FOUND)
    add_executable(algorithm_creator tools/algorithm_creator.cpp)

    target_include_directories(
            algorithm_creator
                PUBLIC
                    ${Qt5Core_INCLUDE_DIRS}
                    ${Qt5GUI_INCLUDE_DIRS}
                    ${Qt5Widgets_INCLUDE_DIRS}
    )

    target_link_directories(
            algorithm_creator
                PUBLIC
                    ${Qt5Core_LINK_DIRS}
                    ${Qt5GUI_LINK_DIRS}
                    ${Qt5Widgets_LINK_DIRS}
    )

    target_link_libraries(algorithm_creator Qt5::Core Qt5::Widgets)
endif()


# Build examples

# Sprinkler example
add_executable(
        sprinkler_example
            examples/example_sprinkler.cpp

)

target_link_libraries(
        sprinkler_example
            bayesnet_lib_static
)

add_dependencies(
        sprinkler_example
            bayesnet_lib_static
)


# Json example
add_executable(
        json_example
            examples/example_json.cpp
)

target_link_libraries(
        json_example
            bayesnet_lib_static
)

add_dependencies(
        json_example
            bayesnet_lib_static
)
