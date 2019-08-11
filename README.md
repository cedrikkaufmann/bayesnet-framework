# BayesNet Framework

Framework for machine prediction (inference) using bayesian networks.
This framework is a fascade using the [libDAI](https://staff.fnwi.uva.nl/j.m.mooij/libDAI/) inference engine.

## Dependencies

All dependencies are assumed to be available as system libraries for static linking. 

- [libDAI](https://staff.fnwi.uva.nl/j.m.mooij/libDAI/)
- libGMP

## Build

The project uses CMake as build system. Make sure to set the correct search paths for includes and libraries in the CMakeLists.txt.

Build successful on the following systems:
- MacOS 10.14 (64-Bit), Clang++ Apple LLVM 10.0.1, libGMP 6.1.2, libDAI 0.3.2
- MacOS 10.14 (64-Bit), Clang++ LLVM 8.0.0, libGMP 6.1.2, libDAI 0.3.2

## Usage

### Create new network

```
bayesNet::Network net;
```

### Add new nodes to network

```
// new binary node
net.newBinaryNode("node1");
net.newBinaryNode("node2");

// new node with 4 states
net.newNode("node3");
```

### Connect two nodes

```
// connect node1 and node2 (node2 is child of node1)
net.newConnection("node1", "node2");

// connect node1 and node3
net.newConnection("node1", "node3");
```

### Create and set cpt for nodes

```
// Missing complementary probabilities are tried to calculate automatically 
std::vector<double> probTable;
probTable.pushBack(0.7);

net.setCPT("node1", bayesNet::CPT(probTable));
```

### Set and clear evidence on nodes

Set evidence:
```
// node with 2 states
net.setEvidence("node1", bayesNet::FALSE);

// node with 4 states
net.setEvidecne("node3", bayesNet::GOOD);
```

Clear evidence:
```
net.clearEvidence("node1");
net.clearEvidence("node3");
```

Allowed values for binary nodes are 
- ```bayesNet::belief::FALSE```
- ```bayesNet::belief::TRUE```

Allowed values for nodes with 4 states
- ```bayesNet::belief::GOOD```
- ```bayesNet::belief::PROBABLY_GOOD```
- ```bayesNet::belief::PROBABLY_BAD```
- ```bayesNet::belief::BAD```

### inference on network

First the network needs to be initialized, this will build a factor graph on base of your build bayesian network and pass this to the inference engine. The following shows the initialization using the ```Loopy Belief Propagation``` algorithm. The initialization only needs to be done once after building the network, or if new nodes are added or old ones are deleted.

```
net.init(BayesNet::inference::LOOPY_BELIEF_PROPAGATION_SUMPROD);
```

After the initialization the network is ready to get inferred.

```
net.doInference();
```

### Read inferred beliefs

The inferred beliefs can be read through the following command
```
net.getBlief("node2");
```

### Save/load network to/from file

To save a network just call
```
net.save("./someFileName.bayesnet");
```

To load a network from file
```
BayesNet::Net net("./someFileName.bayesnet");
```