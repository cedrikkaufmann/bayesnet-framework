cmake_minimum_required(VERSION 3.13)
project(BayesNet_Framework)

# Compiler settings and flags
set(CMAKE_CXX_COMPILER /usr/local/opt/llvm/bin/clang++)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-march=native")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Search paths for includes and libraries
include_directories(/usr/local/opt/llvm/include /usr/local/include)
link_directories(/usr/local/opt/llvm/lib /usr/local/lib)

# Qt5 configuration
#set(CMAKE_PREFIX_PATH /usr/local/opt/qt)
#find_package(Qt5 COMPONENTS Core GUI Widgets)
#include_directories("${Qt5Core_INCLUDE_DIRS} ${Qt5GUI_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS}")
#link_directories("${Qt5Core_LINK_DIRS} ${Qt5GUI_LINK_DIRS} ${Qt5Widgets_LINK_DIRS}")

# Library info for linker
link_libraries(gmp dai)

# Executable to compile
add_executable(example example.cpp)