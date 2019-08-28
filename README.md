# BayesNet Framework

Framework for machine prediction (inference) using bayesian networks and fuzzy logic.
This framework using the [libDAI](https://staff.fnwi.uva.nl/j.m.mooij/libDAI/) inference engine.

## Dependencies

All dependencies are assumed to be available as system libraries for static linking. 

- [libDAI](https://staff.fnwi.uva.nl/j.m.mooij/libDAI/)
- libGMP
- Qt5.7 (Only needed for gui components)

## Build

The project uses CMake as build system. Make sure to set the correct search paths for includes and libraries in the CMakeLists.txt.

Build successful on the following systems:
- MacOS 10.14 Mojave (64-Bit), Apple LLVM 10.0.1, libGMP 6.1.2, libDAI 0.3.2
- MacOS 10.14 Mojave (64-Bit), LLVM 8.0.0, libGMP 6.1.2, libDAI 0.3.2