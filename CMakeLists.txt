cmake_minimum_required(VERSION 3.13)
project(BayesNet_Framework)

set(CMAKE_CXX_COMPILER /usr/local/opt/llvm/bin/clang++)
set(CMAKE_CXX_STANDARD 11)

include_directories(/usr/local/opt/llvm/include /usr/local/include)
link_directories(/usr/local/opt/llvm/lib /usr/local/lib)

link_libraries(gmp dai)
add_executable(example example.cpp)