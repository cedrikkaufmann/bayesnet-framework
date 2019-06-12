//
// Created by Cedrik Kaufmann on 2019-06-06.
//

#include <exception>

#include <bayesnet/network.h>
#include <bayesnet/exception.h>
#include <bayesnet/state.h>

namespace BayesNet {

    Network::Network() : _nodeCounter(0), _init(false), _properties(), _inferenceInstance(nullptr) {
        // TODO: properties not working for every algo
        this->_properties.set("maxiter", size_t(BP_MAXIMUM_ITERATIONS));  // Maximum number of iterations
        this->_properties.set("tol", dai::Real(BP_TOLERANCE));          // Tolerance for convergence
        this->_properties.set("verbose", size_t(BP_VERBOSE));     // Verbosity (amount of output generated)
        this->_properties.set("updates", std::string(BP_UPDATES)); // Update method
        this->_properties.set("inference", std::string(BP_INFERENCE)); // Inference algorithm
        this->_properties.set("logdomain", BP_LOG_DOMAIN);
    }

    void Network::newNode(const std::string &name) {
        auto search = this->_registry.find(name);

        if (search != this->_registry.end()) {
            throw BayesNodeAlreadyDefinedException();
        }

        size_t nodeValue = this->_nodeCounter++;
        this->_registry[name] = nodeValue;

        Node node(nodeValue, BAYESNET_STATES);
        this->_nodes.push_back(node);
    }

    void Network::newConnection(const std::string &parentName, const std::string &childName) {
        size_t nodeChildValue = this->_registry.at(childName);
        size_t nodeParentValue = this->_registry.at(parentName);

        Node &node = this->_nodes.at(nodeChildValue);

        this->_nodes.at(nodeParentValue).addChild(&node);
    }

    Node &Network::getNode(const std::string &name) {
        size_t nodeValue = this->_registry.at(name);
        return this->_nodes.at(nodeValue);
    }
    
    void Network::init(InferenceAlgorithm algorithm) {
        std::vector<dai::Factor> factors;

        for (auto& node : this->_nodes) {
            factors.push_back(node.getFactor());
        }

        this->_factorGraph = dai::FactorGraph(factors);

        for (auto& node : this->_nodes) {
            size_t factorIndex = this->_factorGraph.findFactor(node.getConditionalDiscrete());
            node.setFactorGraphIndex(factorIndex);
        }

        createInferenceInstance(algorithm);
        this->_inferenceInstance->init();

        this->_init = true;
    }

    void Network::setEvidence(const std::string &name, size_t state) {
        if (!this->_init) {
            throw NotInitializedException();
        }

        try {
            size_t nodeValue = this->_registry.at(name);
            Node &node = this->_nodes.at(nodeValue);
            node.setEvidence(state);
            this->_inferenceInstance->setFactor(node.getFactorGraphIndex(), node.getFactor(), false);
            this->_inferenceInstance->init(node.getConditionalDiscrete());
        } catch (const std::exception&) {
            throw BayesNodeNotFoundException();
        }
    }

    void Network::clearEvidence(const std::string &name) {
        if (!this->_init) {
            throw NotInitializedException();
        }

        try {
            size_t nodeValue = this->_registry.at(name);
            Node &node = this->_nodes.at(nodeValue);
            node.clearEvidence();
            this->_inferenceInstance->setFactor(node.getFactorGraphIndex(), node.getFactor(), false);
            this->_inferenceInstance->init(node.getConditionalDiscrete());
        } catch (const std::exception&) {
            throw BayesNodeNotFoundException();
        }
    }

    void Network::createInferenceInstance(InferenceAlgorithm algorithm) {

        switch (algorithm) {

            case LOOPY_BELIEF_PROPAGATION: {
                this->_inferenceInstance = new dai::BP(this->_factorGraph, this->_properties);
                break;
            }

            case CONDITIONED_BELIEF_PROPAGATION: {
                this->_inferenceInstance = new dai::CBP(this->_factorGraph, this->_properties);
                break;
            }

            case FRACTIONAL_BELIEF_PROPAGATION: {
                this->_inferenceInstance = new dai::FBP(this->_factorGraph, this->_properties);
                break;
            }

            case TREE_REWEIGHTED_BELIEF_PROPAGATION: {
                this->_inferenceInstance = new dai::TRWBP(this->_factorGraph, this->_properties);
                break;
            }

            case DECIMATION: {
                this->_inferenceInstance = new dai::DecMAP(this->_factorGraph, this->_properties);
                break;
            }

        }
    }

    void Network::doInference() {
        if (!this->_init) {
            throw NotInitializedException();
        }

        this->_inferenceInstance->run();
    }

    dai::TFactor<dai::Real> Network::getBelief(const std::string &name) {
        if (!this->_init) {
            throw NotInitializedException();
        }

        size_t nodeValue = this->_registry.at(name);
        Node &node = this->_nodes.at(nodeValue);

        return this->_inferenceInstance->belief(node.getDiscrete());
    }
}