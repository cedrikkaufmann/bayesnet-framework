# BayesNet Framework

Framework to build Expertensysteme using bayesian networks and fuzzy logic.
This framework using the [libDAI](https://staff.fnwi.uva.nl/j.m.mooij/libDAI/) inference engine.

## Dependencies

All dependencies are assumed to be available. Search and link paths for these can be set in CMakeLists.txt 

- [libDAI](https://staff.fnwi.uva.nl/j.m.mooij/libDAI/)
- libGMP
- Qt5 (Only needed for gui components)
- Additionally the Standalone BayesServer requires Qt5 for its websocket server

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

# Standalone BayesServer

The Standalone BayesServer exposes a simple JSON based API over WebSockets.

## Build
To build the Standalone BayesServer make sure Qt5 and especially the Qt5 Websocket dependencies are installed. Then just turn on the compiler switch `-DBUILD_STANDALONE_SERVER=ON`.

## API

The following methods are exposed through a JSON WebSocket API.

Action          |               
----------------|
load_network    |
get_belief      |
set_evidence    |
clear_evidence  |
observe         |

### Load the network `/networks/foo.bayesnet`

```
{
    "action": "load_network",
    "payload": {
        "file": "/networks/foo.bayesnet"
    }
}
```

### Read belief of the node `foo`

```
{
    "action": "get_belief",
    "payload": {
        "node": "foo"
    }
}
```

### Set evidence of the node `foo` to state 0

```
{
    "action": "set_evidence",
    "payload": {
        "node": "foo",
        "state": 0
    }
}
```

### Clear evidence of the node `foo`

```
{
    "action": "clear_evidence",
    "payload": {
        "node": "foo"
    }
}
```

### Observe a continous value on sensor node `bar`

```
{
    "action": "observe",
    "payload": {
        "node": "bar",
        "value": 3.43
    }
}
```

## BayesServer Client

The BayesServer Client is a Python clientside implemenation of the BayesServer WebSocet API. It can be used to connect to a BayesServer and provides a interactive shell. The interactive shell is driven by a simple scripting language implemented by the client. It is also possible to load scripts from file to automate the management of Bayesian Networks through the WebSocket API.

### BayesScript

1. Empty lines are ignored.

2. Comments are indicated by a leading `;`
    ```
    ;this is a simple comment
    ```

2. Output a message
    ```
    print this is a simple message
    ```

3. Load a network
    ```
    load_network /networks/foo.bayesnet
    ```

4. Read a belief
    ```
    get_belief foo
    ```

5. Set evidence on a node to a given state
    ```
    set_evidence foo 0
    ```

6. Clear evidence on a given node
    ```
    clear_evidence foo
    ```

7. Observe a continious value on a given sensor node
    ```
    observe foo 32.421
    ```

### Example
```
;print a message
print BayesScript v1

;load a network from file
load_network /networks/lane_change.bayesnet

;read initial beliefs for some nodes
get_belief positioning_sensor
get_belief perceive_dynamic_objects
get_belief lane_change

;set some evidence
set_evidence perceive_dynamic_objects 2
observe positioning_sensor 0.5

;read some beliefs again
get_belief positioning_sensor
get_belief perceive_dynamic_objects
get_belief lane_change
```







