cmake_minimum_required(VERSION 3.13)
project(BayesNet_Framework)

# options to specify which parts of the framework should be built
# default library will always be built
option(BUILD_EXAMPLES "Defines if example should be built" OFF)
option(BUILD_GUI_TOOLS "Defines if gui applications based on Qt5 should be built" ON)
option(BUILD_CLI_TOOLS "Defines if cli applications should be built " ON)

# make sure correct settings are specified

# Qt5 prefix path
set(CMAKE_PREFIX_PATH /usr/local/opt/qt)

# GMP include and link directories
set(BayesNet_INLCUDE_DIRS_GMPXX /usr/local/include)
set(BayesNet_LINK_DIRS_GMPXX /usr/local/lib)

# LibDAI include and link directories
set(BayesNet_INLCUDE_DIRS_DAI /usr/local/opt/libdai/include)
set(BayesNet_LINK_DIRS_DAI /usr/local/opt/libdai/lib)

# Global Compiler flags
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_INCLUDE_CURRENT_DIR YES)

# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

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
        SYSTEM INTERFACE
        ${BayesNet_INLCUDE_DIRS_GMP}
        ${BayesNet_INLCUDE_DIRS_DAI}
)

target_link_directories(
        bayesnet_lib INTERFACE
        ${BayesNet_INLCUDE_DIRS_GMP}
        ${BayesNet_INLCUDE_DIRS_DAI}
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

# Set compiler specific options
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
        target_compile_options(
                bayesnet_lib PRIVATE
                -Weverything
                -Wno-deprecated 
                -Wno-padded
                -Wno-exit-time-destructors
                -Wno-global-constructors
        )
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        target_compile_options(
                bayesnet_lib PRIVATE
                -Wall -Wextra -pedantic
                -Wno-deprecated
                -Wno-padded
                -Wno-exit-time-destructors
                -Wno-global-constructors
        )
endif ()


if (BUILD_CLI_TOOLS)
        message(STATUS "Cli tools will be built")

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


if(BUILD_EXAMPLES)
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


        # Json example
        add_executable(
                example_json
                examples/example_json.cpp
        )

        target_link_libraries(
                example_json
                bayesnet_lib
        )

        add_dependencies(
                example_json
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

if (BUILD_GUI_TOOLS)
        message(STATUS "Gui tools will be built")

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

                # Set compiler specific options
                if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
                        target_compile_options(
                                bayesnet_gui_lib PRIVATE
                                -Weverything
                                -Wno-deprecated
                                -Wno-padded
                        )
                elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
                        target_compile_options(
                                bayesnet_lib PRIVATE
                                -Wall -Wextra -pedantic
                                -Wno-deprecated
                                -Wno-padded
                        )
                endif ()

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