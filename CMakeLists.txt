cmake_minimum_required(VERSION 3.13)
project(BayesNet_Framework)

# options to specify which parts of the framework should be built
# default library will always be built
option(SHARED "Defines if library should be dynamically linked" OFF)
option(BUILD_EXAMPLES "Defines if examples should be built" OFF)
option(BUILD_GUI "Defines if gui applications based on Qt5 should be built" OFF)
option(BUILD_CLI "Defines if cli applications should be built " OFF)

# make sure correct settings are specified

# Qt5 prefix path
set(CMAKE_PREFIX_PATH /Users/cedrik/Qt/5.14.1/clang_64)
# Libdai Path
set(LIBDAI_PREFIX_PATH /opt/libDAI-0.3.2)

# Global Compiler flags
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_INCLUDE_CURRENT_DIR YES)

include_directories(${LIBDAI_PREFIX_PATH}/include)
link_directories(${LIBDAI_PREFIX_PATH}/lib)

# output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Bayesnet Library
if (SHARED)
        message(STATUS "Shared library enabled")
        add_library(
                bayesnet_lib SHARED
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
else ()
        message(STATUS "Static library enabled")
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
endif ()


target_include_directories(
	bayesnet_lib PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

set_target_properties(
        bayesnet_lib PROPERTIES
        OUTPUT_NAME bayesnet
)

target_link_libraries(
        bayesnet_lib
        dai
        gmp
        gmpxx
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  # using Clang
  target_compile_options(
	bayesnet_lib PRIVATE
        -Weverything
  )
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  # using GCC
  target_compile_options(
	bayesnet_lib PRIVATE
        -Wall -Wextra
  )
endif()



if (BUILD_CLI)
        message(STATUS "CLI will be built")

        # CPT inference tool
        add_executable(
                infer_cpt
                tools/cpt_inference.cpp
        )

        target_link_libraries(
                infer_cpt
                bayesnet_lib
        )

        add_dependencies(
                infer_cpt
                bayesnet_lib
        )
endif ()


if (BUILD_EXAMPLES)
        message(STATUS "Examples will be built")

        # Sprinkler example
        add_executable(
                example_sprinkler
                examples/example_sprinkler.cpp
        )

        target_link_libraries(
                example_sprinkler
                bayesnet_lib
        )

        add_dependencies(
                example_sprinkler
                bayesnet_lib
        )


        # Load network from file example
        add_executable(
                example_file
                examples/example_network_file.cpp
        )

        target_link_libraries(
                example_file
                bayesnet_lib
        )

        add_dependencies(
                example_file
                bayesnet_lib
        )

        # CPT inference example
        add_executable(
                example_cpt_inference
                examples/example_cpt_inference.cpp
        )

        target_link_libraries(
                example_cpt_inference
                bayesnet_lib
        )

        add_dependencies(
                example_cpt_inference
                bayesnet_lib
        )
endif ()

if (BUILD_GUI)
        message(STATUS "GUI will be built")

        # Look for Qt5 dependecies
        find_package(Qt5 COMPONENTS Core GUI Widgets)

        # Build GUI components if Qt5 found
        if (Qt5_FOUND)
                # GUI Library
                add_library(
                        bayesnet_gui_lib STATIC
                        ${PROJECT_SOURCE_DIR}/include/bayesnet/gui/inference.h
                        ${PROJECT_SOURCE_DIR}/include/bayesnet/gui/editor.h
                        ${PROJECT_SOURCE_DIR}/include/bayesnet/gui/diagramscene.h
                        ${PROJECT_SOURCE_DIR}/include/bayesnet/gui/node.h
                        ${PROJECT_SOURCE_DIR}/include/bayesnet/gui/connection.h
                        ${PROJECT_SOURCE_DIR}/src/gui/inference.cpp
                        ${PROJECT_SOURCE_DIR}/src/gui/editor.cpp
                        ${PROJECT_SOURCE_DIR}/src/gui/diagramscene.cpp
                        ${PROJECT_SOURCE_DIR}/src/gui/node.cpp
                        ${PROJECT_SOURCE_DIR}/src/gui/connection.cpp
                )

                target_compile_options(
                        bayesnet_gui_lib PRIVATE
                        -Weverything
                )

                target_include_directories(
                        bayesnet_gui_lib SYSTEM INTERFACE   
                        ${Qt5Core_INCLUDE_DIRS}
                        ${Qt5GUI_INCLUDE_DIRS}
                        ${Qt5Widgets_INCLUDE_DIRS}
                )

                target_link_directories(
                        bayesnet_gui_lib INTERFACE
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

                # Inference algorithm tool
                add_executable(inference_algorithm_tool tools/inference_algorithm_tool.cpp)

                target_link_libraries(inference_algorithm_tool bayesnet_gui_lib)

                add_dependencies(inference_algorithm_tool bayesnet_gui_lib)

                # BayesNet Editor

                add_executable(bayesnet_editor tools/bayesnet_editor.cpp)

                target_link_libraries(bayesnet_editor bayesnet_gui_lib)

                add_dependencies(bayesnet_editor bayesnet_gui_lib)
        endif ()
endif ()
