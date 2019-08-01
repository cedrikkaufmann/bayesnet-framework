//
// Created by Cedrik Kaufmann on 2019-06-06.
//

#include <exception>

#include <bayesnet/network.h>
#include <bayesnet/exception.h>
#include <bayesnet/inference.h>

#include <dai/bp.h>
#include <dai/cbp.h>
#include <dai/fbp.h>

namespace BayesNet {

    using namespace Inference;

    void Network::newNode(const std::string &name, size_t states) {
        std::unordered_map<std::string, size_t>::const_iterator search = _registry.find(name);

        if (search != _registry.end()) {
            throw BayesNodeAlreadyDefinedException();
        }

        size_t nodeValue = _nodeCounter++;
        _registry[name] = nodeValue;

        Node node(nodeValue, states);
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

        Node &node = _nodes.at(nodeChildValue);

        _nodes.at(nodeParentValue).addChild(&node);
    }

    Node &Network::getNode(const std::string &name) {
        size_t nodeValue = _registry.at(name);
        return _nodes.at(nodeValue);
    }

    void Network::init(InferenceProperties algorithm) {
        std::vector<dai::Factor> factors;

        for (size_t i = 0; i < _nodes.size(); ++i) {
            factors.push_back(_nodes[i].getFactor());
        }

        _factorGraph = dai::FactorGraph(factors);

        for (size_t i = 0; i < _nodes.size(); ++i) {
            size_t factorIndex = _factorGraph.findFactor(_nodes[i].getConditionalDiscrete());
            _nodes[i].setFactorGraphIndex(factorIndex);
        }

        createInferenceInstance(algorithm);
        _inferenceInstance->init();

        _init = true;
    }

    void Network::setEvidence(const std::string &name, size_t state) {
        if (!_init) {
            throw NotInitializedException();
        }

        try {
            Node &node = getNode(name);

            if (node.getDiscrete().states() == 2 && state > 3 && state < 6) {
                node.setEvidence(state - 4);
            } else if (state > 3) {
                throw IndexOutOfBoundException();
            } else {
                node.setEvidence(state);
            }

            _inferenceInstance->setFactor(node.getFactorGraphIndex(), node.getFactor(), false);
            _inferenceInstance->init(node.getConditionalDiscrete());
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
            Node &node = _nodes.at(nodeValue);
            node.clearEvidence();
            _inferenceInstance->setFactor(node.getFactorGraphIndex(), node.getFactor(), false);
            _inferenceInstance->init(node.getConditionalDiscrete());
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

        Node &node = getNode(name);

        dai::Factor belief = _inferenceInstance->belief(node.getDiscrete());
        BayesBelief bayesBelief(node.getDiscrete().states() == 2);

        for (size_t i = 0; i < belief.nrStates(); ++i) {
            bayesBelief[i] = belief[i];
        }

        return bayesBelief;
    }

    void Network::setCPT(const std::string &name, const CPT &cpt) {
        Factor& factor = getNode(name).getFactor();

        for (size_t i = 0; i < cpt.size(); ++i) {
            factor.set(i, dai::Real(cpt.get(i)));
        }
    }
}
