# BayesNet Framework

Framework for machine prediction (inference) using bayesian networks.
This framework is a fascade using the [libDAI](https://staff.fnwi.uva.nl/j.m.mooij/libDAI/) inference engine.

## Dependencies

All dependencies are assumed to be available as system libraries for static linking. 

- [libDAI](https://staff.fnwi.uva.nl/j.m.mooij/libDAI/)
- libGMP, libGMPXX

## Build

The project uses CMake as build system. Make sure to set the correct search paths for includes and libraries in the CMakeLists.txt.

Build successful on the following systems:
- MacOS 10.14 (64-Bit), Apple LLVM 10.0.1, libGMP 6.1.2, libDAI 0.3.2

**libDAI compilation note**

To build the latest version of libDAI the file ``include/dai/util.h`` needs to be updated to use unordered_map from c++11 instead of (unsupported) tr1 version.