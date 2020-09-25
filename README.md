# BayesNet Framework

Framework to build Expertensysteme using bayesian networks and fuzzy logic.
This framework using the [libDAI](https://staff.fnwi.uva.nl/j.m.mooij/libDAI/) inference engine.

## Dependencies

All dependencies are assumed to be available. Search and link paths for these can be set in CMakeLists.txt 

- [libDAI](https://staff.fnwi.uva.nl/j.m.mooij/libDAI/)
- libGMP
- Qt5 (Only needed for gui components)

## Build

The project uses CMake as build system. Make sure to set the correct search paths for includes and libraries in the CMakeLists.txt.
The build script supports the following options:
- SHARED (build libbayesnet as shared library instead of static)
- BUILD_GUI (build qt5 gui based components)
- BUILD_CLI (build cli tools)
- BUILD_EXAMPLES (build shipped examples)

**All option´s defaults are set to OFF.**

The following cmake configuartion would build the gui and cli components alongside with the libbayesnet as static library:

```
mkdir build && cd build && cmake .. -DBUILD_CLI=ON -DBUILD_GUI=ON && make
```

Build successful on the following systems, library and compiler combinations:
- MacOS 10.14 Mojave (64-Bit)
    - Apple LLVM 10.0.1, libGMP 6.1.2, libDAI 0.3.2
    - Apple LLVM 11.0.0, libGMP 6.1.2, libDAI 0.3.2
- Ubuntu 20.04 LTS x64
    - LLVM Clang++ 10.0.0, libGMP 6.2, libDAI 0.3.2

# Supported Features
- Approximate inference on discrete bayesian networks
- Exact inference on discrete bayesian networks 
- Continuous observables as entry nodes
- Discrete evidence on other nodes
- Inference of CPTs (conditional probability tables)
- Load/Save network from file
- Simple GUI to visualize networks

# Supported inference algorithms
The following inference algorithms are supported through use of libDAI.
Algorithm                      | Type 
------------------------------ | ------------- 
Junction tree                  | exact
Belief propagation             | approximative
Fractional belief propagation  | approximative
Conditioned belief propagation | approximative
Mean field                     | approximative
Gibbs sampling                 | approximative