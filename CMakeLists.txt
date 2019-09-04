cmake_minimum_required(VERSION 3.13)
project(BayesNet_Framework)


# settings and flags
set(CMAKE_CXX_COMPILER /usr/local/opt/llvm/bin/clang++)
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_INCLUDE_CURRENT_DIR YES)


# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(${PROJECT_SOURCE_DIR}/include)

# Bayesnet Library
add_library(
        bayesnet_lib STATIC
        ${PROJECT_SOURCE_DIR}/src/cpt.cpp
        ${PROJECT_SOURCE_DIR}/src/exception.cpp
        ${PROJECT_SOURCE_DIR}/src/factor.cpp
        ${PROJECT_SOURCE_DIR}/src/inference.cpp
        ${PROJECT_SOURCE_DIR}/src/file.cpp
        ${PROJECT_SOURCE_DIR}/src/network.cpp
        ${PROJECT_SOURCE_DIR}/src/node.cpp
        ${PROJECT_SOURCE_DIR}/src/state.cpp
        ${PROJECT_SOURCE_DIR}/src/util.cpp
        ${PROJECT_SOURCE_DIR}/src/fuzzy.cpp
)

target_include_directories(
        bayesnet_lib PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        /usr/local/include
        /usr/local/opt/llvm/include
)

target_link_directories(
        bayesnet_lib PUBLIC
        /usr/local/lib
        /usr/local/opt/llvm/lib
)

set_target_properties(
        bayesnet_lib PROPERTIES
        OUTPUT_NAME bayesnet
)

target_link_libraries(
        bayesnet_lib
        gmp
        gmpxx
        dai
)

target_compile_options(
        bayesnet_lib PRIVATE
        -Weverything
        -Wno-deprecated
        -Wno-padded
        -Wno-unused-function
        -Werror
)


# Build examples

# Sprinkler example
add_executable(
        sprinkler_example
        examples/example_sprinkler.cpp

)

target_link_libraries(
        sprinkler_example
        bayesnet_lib
)

add_dependencies(
        sprinkler_example
        bayesnet_lib
)


# Json example
add_executable(
        json_example
        examples/example_json.cpp
)

target_link_libraries(
        json_example
        bayesnet_lib
)

add_dependencies(
        json_example
        bayesnet_lib
)

# Network save example
add_executable(
        save_example
        examples/example_save_network.cpp

)

target_link_libraries(
        save_example
        bayesnet_lib
)

add_dependencies(
        save_example
        bayesnet_lib
)

# Look for Qt5 dependecies
set(CMAKE_PREFIX_PATH /usr/local/opt/qt)
find_package(Qt5 COMPONENTS Core GUI Widgets)

# Build GUI components if Qt5 found
if (Qt5_FOUND)
    # GUI Library
    add_library(
            bayesnet_gui_lib STATIC
            include/bayesnet/gui/inference.h
            src/gui/inference.cpp
    )

    target_include_directories(
            bayesnet_gui_lib PUBLIC
            ${Qt5Core_INCLUDE_DIRS}
            ${Qt5GUI_INCLUDE_DIRS}
            ${Qt5Widgets_INCLUDE_DIRS}
    )

    target_link_directories(
            bayesnet_gui_lib PUBLIC
            ${Qt5Core_LINK_DIRS}
            ${Qt5GUI_LINK_DIRS}
            ${Qt5Widgets_LINK_DIRS}
    )

    set_target_properties(
            bayesnet_gui_lib PROPERTIES
            OUTPUT_NAME bayesnet_gui
            AUTOMOC TRUE
    )

    target_link_libraries(bayesnet_gui_lib Qt5::Core Qt5::Gui Qt5::Widgets bayesnet_lib)

    add_dependencies(bayesnet_gui_lib bayesnet_lib)

    target_compile_options(
            bayesnet_gui_lib PRIVATE
            -Weverything
    )

    # Inference algorithm tool
    add_executable(inference_algorithm_tool tools/inference_algorithm_tool.cpp)

    target_include_directories(
            inference_algorithm_tool PUBLIC
            ${Qt5Core_INCLUDE_DIRS}
            ${Qt5GUI_INCLUDE_DIRS}
            ${Qt5Widgets_INCLUDE_DIRS}
    )

    target_link_directories(
            inference_algorithm_tool PUBLIC
            ${Qt5Core_LINK_DIRS}
            ${Qt5GUI_LINK_DIRS}
            ${Qt5Widgets_LINK_DIRS}
    )

    target_link_libraries(inference_algorithm_tool bayesnet_gui_lib)

    add_dependencies(inference_algorithm_tool bayesnet_gui_lib)

endif ()


# Json example
add_executable(
        cpt_inference_example
        examples/example_cpt_inference.cpp
)

target_link_libraries(
        cpt_inference_example
        bayesnet_lib
)

add_dependencies(
        cpt_inference_example
        bayesnet_lib
)