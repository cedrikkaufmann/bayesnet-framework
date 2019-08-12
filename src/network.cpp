//
// Created by Cedrik Kaufmann on 2019-06-06.
//

#include <exception>

#include <bayesnet/network.h>
#include <bayesnet/exception.h>

namespace bayesNet {

    Network::Network(const std::string &file) : _inferenceAlgorithm(nullptr), _nodeCounter(0), _init(false) {
        load(file);
    }

    void Network::newNode(const std::string &name, size_t states) {
        std::unordered_map<std::string, size_t>::const_iterator search = _registry.find(name);

        if (search != _registry.end()) {
            throw BayesNodeAlreadyDefinedException();
        }

        _nodeNames.push_back(name);

        size_t nodeValue = _nodeCounter++;
        _registry[name] = nodeValue;

        Node *node = new Node(name, nodeValue, states);
        _nodes.push_back(node);
    }

    void Network::newNode(const std::string &name) {
        newNode(name, 4); // create node with 4 states as default
    }

    void Network::newBinaryNode(const std::string &name) {
        newNode(name, 2); // create node with 2 states
    }

    void Network::newConnection(const std::string &parentName, const std::string &childName) {
        size_t nodeChildValue = _registry.at(childName);
        size_t nodeParentValue = _registry.at(parentName);

        Node *node = _nodes[nodeChildValue];
        _nodes[nodeParentValue]->addChild(node);
    }

    Node *Network::getNode(const std::string &name) {
        size_t nodeValue = _registry.at(name);
        return _nodes[nodeValue];
    }

    void Network::init(inference::Algorithm *alg) {
        std::vector<dai::Factor> factors;

        for (size_t i = 0; i < _nodes.size(); ++i) {
            factors.push_back(_nodes[i]->getFactor());
        }

        _factorGraph = dai::FactorGraph(factors);

        for (size_t i = 0; i < _nodes.size(); ++i) {
            size_t factorIndex = _factorGraph.findFactor(_nodes[i]->getConditionalDiscrete());
            _nodes[i]->setFactorGraphIndex(factorIndex);
        }

        _inferenceAlgorithm = alg;
        alg->generateInferenceInstance(_factorGraph);
        alg->getInstance()->init();

        _init = true;
    }

    void Network::setEvidence(const std::string &name, belief::BeliefState state) {
        if (!_init) {
            throw NotInitializedException();
        }

        try {
            Node *node = getNode(name);

            if (node->getDiscrete().states() == 2 && state > 3 && state < 6) {
                node->setEvidence(state - 4);
            } else if (state > 3) {
                throw IndexOutOfBoundException();
            } else {
                node->setEvidence(state);
            }

            _inferenceAlgorithm->getInstance()->fg().setFactor(node->getFactorGraphIndex(), node->getFactor(), false);
            _inferenceAlgorithm->getInstance()->init(node->getConditionalDiscrete());
        } catch (const std::exception &) {
            throw BayesNodeNotFoundException();
        }
    }

    void Network::clearEvidence(const std::string &name) {
        if (!_init) {
            throw NotInitializedException();
        }

        try {
            size_t nodeValue = _registry.at(name);
            Node *node = _nodes[nodeValue];
            node->clearEvidence();
            _inferenceAlgorithm->getInstance()->fg().setFactor(node->getFactorGraphIndex(), node->getFactor(), false);
            _inferenceAlgorithm->getInstance()->init(node->getConditionalDiscrete());
        } catch (const std::exception &) {
            throw BayesNodeNotFoundException();
        }
    }

    void Network::doInference() {
        if (!_init) {
            throw NotInitializedException();
        }

        this->_inferenceAlgorithm->getInstance()->run();
    }

    belief::BayesBelief Network::getBelief(const std::string &name) {
        if (!_init) {
            throw NotInitializedException();
        }

        Node *node = getNode(name);

        dai::Factor belief = this->_inferenceAlgorithm->getInstance()->belief(node->getDiscrete());
        belief::BayesBelief bayesBelief(node->isBinary());

        for (size_t i = 0; i < belief.nrStates(); ++i) {
            bayesBelief[i] = belief[i];
        }

        return bayesBelief;
    }

    void Network::setCPT(const std::string &name, const CPT &cpt) {
        getNode(name)->setCPT(cpt);
    }

    void Network::load(const std::string &file) {
        load(json::parse(file));
    }

    void Network::load(json::InitializationVector *iv) {
        // add nodes with 4 states to network
        for (size_t i = 0; i < iv->nodes.size(); ++i) {

            newNode(iv->nodes[i]);
        }

        // add binary nodes to network
        for (size_t i = 0; i < iv->binaryNodes.size(); ++i) {

            newBinaryNode(iv->binaryNodes[i]);
        }

        // add connections for nodes to network
        for (std::unordered_map<std::string, std::vector<std::string> >::const_iterator it = iv->connections.begin();
             it != iv->connections.end(); it++) {

            for (size_t i = 0; i < (*it).second.size(); ++i) {

                newConnection((*it).first, (*it).second[i]);
            }
        }

        // add cpt for nodes to network
        for (std::unordered_map<std::string, std::vector<double> >::const_iterator it = iv->cpt.begin();
             it != iv->cpt.end(); it++) {

            setCPT((*it).first, CPT((*it).second));
        }

        // generate inference algorithm from string
        inference::Algorithm *algorithm = new inference::Algorithm(iv->inferenceAlgorithm);

        // initialize network
        init(algorithm);
    }

    void Network::save(const std::string &filename) {
        json::InitializationVector *iv = new json::InitializationVector();

        for (size_t i = 0; i < _nodeNames.size(); ++i) {

            Node *node = getNode(_nodeNames[i]);

            // add node names to iv
            if (node->isBinary()) {

                iv->binaryNodes.push_back(node->getName());
            } else {

                iv->nodes.push_back(node->getName());
            }

            // add connections to iv
            std::vector<Node *> children = node->getChildren();

            for (size_t j = 0; j < children.size(); ++j) {

                iv->connections[node->getName()].push_back(children[j]->getName());
            }

            // add cpt to iv
            iv->cpt[node->getName()] = node->getCPT().getProbabilities();

            // add inference algorithm
            if (_init && !_inferenceAlgorithm->getFilename().empty()) {
                iv->inferenceAlgorithm = _inferenceAlgorithm->getFilename();
            }
        }

        json::save(filename, iv);
    }

    void Network::save(const std::string &networkFilename, const std::string &algorithmFilename) {
        if (_inferenceAlgorithm != nullptr) {

            _inferenceAlgorithm->save(algorithmFilename);
        }

        save(networkFilename);
    }
}
