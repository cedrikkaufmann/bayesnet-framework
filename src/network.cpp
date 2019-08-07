//
// Created by Cedrik Kaufmann on 2019-06-06.
//

#include <exception>

#include <bayesnet/network.h>
#include <bayesnet/exception.h>

#include <dai/bp.h>
#include <dai/cbp.h>
#include <dai/fbp.h>

namespace BayesNet {

    using namespace Inference;

    Network::Network(const std::string &file)
            : _properties(), _inferenceInstance(nullptr), _nodeCounter(0), _init(false) {
        InitializationVector *iv = parseJson(file);
        load(iv);
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

    void Network::init(InferenceProperties algorithm) {
        std::vector<dai::Factor> factors;

        for (size_t i = 0; i < _nodes.size(); ++i) {
            factors.push_back(_nodes[i]->getFactor());
        }

        _factorGraph = dai::FactorGraph(factors);

        for (size_t i = 0; i < _nodes.size(); ++i) {
            size_t factorIndex = _factorGraph.findFactor(_nodes[i]->getConditionalDiscrete());
            _nodes[i]->setFactorGraphIndex(factorIndex);
        }

        createInferenceInstance(algorithm);
        _inferenceInstance->init();

        _init = true;
    }

    void Network::setEvidence(const std::string &name, BeliefState state) {
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

            _inferenceInstance->setFactor(node->getFactorGraphIndex(), node->getFactor(), false);
            _inferenceInstance->init(node->getConditionalDiscrete());
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
            _inferenceInstance->setFactor(node->getFactorGraphIndex(), node->getFactor(), false);
            _inferenceInstance->init(node->getConditionalDiscrete());
        } catch (const std::exception &) {
            throw BayesNodeNotFoundException();
        }
    }

    void Network::createInferenceInstance(InferenceProperties inf) {

        switch (inf) {

            case LOOPY_BELIEF_PROPAGATION_MAXPROD: {
                dai::PropertySet opts = getInferenceProperties(inf);
                _inferenceInstance = new dai::BP(_factorGraph, opts);
                break;
            }

            case LOOPY_BELIEF_PROPAGATION_SUMPROD: {
                dai::PropertySet opts = getInferenceProperties(inf);
                _inferenceInstance = new dai::BP(_factorGraph, opts);
                break;
            }


            case CONDITIONED_BELIEF_PROPAGATION: {
                dai::PropertySet opts = getInferenceProperties(inf);
                _inferenceInstance = new dai::CBP(_factorGraph, opts);
                break;
            }

            case FRACTIONAL_BELIEF_PROPAGATION: {
                dai::PropertySet opts = getInferenceProperties(inf);
                _inferenceInstance = new dai::FBP(_factorGraph, opts);
                break;
            }
        }
    }

    void Network::doInference() {
        if (!_init) {
            throw NotInitializedException();
        }

        this->_inferenceInstance->run();
    }

    BayesBelief Network::getBelief(const std::string &name) {
        if (!_init) {
            throw NotInitializedException();
        }

        Node *node = getNode(name);

        dai::Factor belief = _inferenceInstance->belief(node->getDiscrete());
        BayesBelief bayesBelief(node->getDiscrete().states() == 2);

        for (size_t i = 0; i < belief.nrStates(); ++i) {
            bayesBelief[i] = belief[i];
        }

        return bayesBelief;
    }

    void Network::setCPT(const std::string &name, const CPT &cpt) {
        getNode(name)->setCPT(cpt);
    }

    void Network::load(const std::string &file) {
        load(parseJson(file));
    }

    void Network::load(InitializationVector *iv) {
        std::vector<std::string> nodes;

        // add nodes with 4 states to network
        for (size_t i = 0; i < iv->nodes.size(); ++i) {

            std::cout << "adding node " << iv->nodes[i] << std::endl;
            newNode(iv->nodes[i]);
            nodes.push_back(iv->nodes[i]);
        }

        // add binary nodes to network
        for (size_t i = 0; i < iv->binaryNodes.size(); ++i) {

            std::cout << "adding node " << iv->binaryNodes[i] << std::endl;
            newBinaryNode(iv->binaryNodes[i]);
            nodes.push_back(iv->binaryNodes[i]);
        }

        // add connections for nodes to network
        for (size_t i = 0; i < nodes.size(); ++i) {

            for (size_t j = 0; j < iv->connections[nodes[i]].size(); ++j) {

                std::cout << "adding connection from " << nodes[i] << " to " << iv->connections[nodes[i]][j]
                          << std::endl;
                newConnection(nodes[i], iv->connections[nodes[i]][j]);
            }
        }

        // add cpt for nodes to network
        for (size_t i = 0; i < nodes.size(); ++i) {

            std::cout << "set cpt for " << nodes[i] << std::endl;
            setCPT(nodes[i], CPT(iv->cpt[nodes[i]]));
        }
    }

    void Network::save(const std::string &file) {
        InitializationVector *iv = new InitializationVector();

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
        }

        saveJson(file, iv);
    }
}
