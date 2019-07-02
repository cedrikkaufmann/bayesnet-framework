cmake_minimum_required(VERSION 3.13)
project(BayesNet_Framework)

# Compiler settings and flags
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_FLAGS "-march=native -Weverything")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Search paths for includes and libraries
include_directories(/usr/local/include include)
link_directories(/usr/local/lib)

# Qt5 configuration
#set(CMAKE_PREFIX_PATH /usr/local/opt/qt)
#find_package(Qt5 COMPONENTS Core GUI Widgets)
#include_directories("${Qt5Core_INCLUDE_DIRS} ${Qt5GUI_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS}")
#link_directories("${Qt5Core_LINK_DIRS} ${Qt5GUI_LINK_DIRS} ${Qt5Widgets_LINK_DIRS}")

# Linked system librariers: LibGMP, LibGMPXX, LibDai (Bayesian inference lib)
link_libraries(gmp gmpxx dai)

# Add compilable executables
add_executable(bayesnet_example example.cpp src/factor.cpp src/network.cpp src/node.cpp src/exception.cpp src/state.cpp src/inference.cpp src/cpt.cpp)

# Link QT5
# target_link_libraries(bayesnet_builder Qt5::Core Qt5::Widgets)