//
// Created by Cedrik Kaufmann on 2019-06-06.
//

#include <exception>

#include <bayesnet/network.h>
#include <bayesnet/exception.h>

namespace bayesNet {

    Network::Network() : _inferenceAlgorithm(NULL), _nodeCounter(0), _init(false) {

    }

    Network::Network(const std::string &file) : _inferenceAlgorithm(NULL), _nodeCounter(0), _init(false) {
        load(file);
    }

    Network::~Network() {

    }

    void Network::newNode(const std::string &name, bool binary) {
        size_t states;

        if (binary) {

            states = 2;
        } else {

            states = 4;
        }

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

    void Network::setEvidence(const std::string &name, size_t state) {
        if (!_init) {

            throw NotInitializedException();
        }

        try {
            Node *node = getNode(name);

            if (state < node->getDiscrete().states()) {

                node->setEvidence(state);
            } else {

                throw IndexOutOfBoundException();
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
            refreshFactorGraph(node);
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

    state::BayesBelief Network::getBelief(const std::string &name) {
        if (!_init) {

            throw NotInitializedException();
        }

        Node *node = getNode(name);

        dai::Factor belief = this->_inferenceAlgorithm->getInstance()->belief(node->getDiscrete());
        state::BayesBelief bayesBelief(node->isBinary());

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

            newNode(iv->binaryNodes[i], true);
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
        if (_inferenceAlgorithm != NULL) {

            _inferenceAlgorithm->save(algorithmFilename);
        }

        save(networkFilename);
    }

    void Network::newSensorNode(const std::string &name, bool binary) {
        size_t states;

        if (binary) {

            states = 2;
        } else {

            states = 4;
        }

        std::unordered_map<std::string, size_t>::const_iterator search = _registry.find(name);

        if (search != _registry.end()) {

            throw BayesNodeAlreadyDefinedException();
        }

        _nodeNames.push_back(name);

        size_t nodeValue = _nodeCounter++;
        _registry[name] = nodeValue;

        Node *node = new SensorNode(name, nodeValue, states);
        _nodes.push_back(node);
    }

    void Network::observe(const std::string &name, double x) {
        SensorNode *node = dynamic_cast<SensorNode *>(getNode(name));
        node->observe(x);

        refreshFactorGraph(node);
    }

    void Network::refreshFactorGraph(Node *node) {
        if (!_init) {

            throw NotInitializedException();
        }

        _inferenceAlgorithm->getInstance()->fg().setFactor(node->getFactorGraphIndex(), node->getFactor(), false);
        _inferenceAlgorithm->getInstance()->init(node->getConditionalDiscrete());
    }

    void Network::setMembershipFunction(const std::string &name, size_t state, fuzzyLogic::MembershipFunction *mf) {
        Node *node = getNode(name);
        node->setMembershipFunction(state, mf);
    }

    void Network::inferCPT(const std::string &name) {
        Node *node = getNode(name);
        std::vector<Node *> parents = getParents(node);

        std::vector<fuzzyLogic::Set *> fuzzySets(parents.size());

        // collect fuzzy sets
        for (size_t i = 0; i < parents.size(); ++i) {
            fuzzySets[i] = &parents[i]->getFuzzySet();
        }

        // create inference controller instance
        fuzzyLogic::Controller inferenceCtrl(fuzzySets, &node->getFuzzyRules());

        // infer cpt
        CPT cpt = inferenceCtrl.inferCPT();

        // set cpt for node
        setCPT(name, cpt);
    }

    std::vector<Node *> Network::getParents(Node *node) {
        std::vector<Node *> parents;

        // iterate over all nodes
        for (size_t i = 0; i < _nodes.size(); ++i) {

            std::vector<Node *> &children = _nodes[i]->getChildren();

            // iterate over children
            for (size_t j = 0; j < children.size(); ++j) {

                // check if node has given node as child
                if (children[j] == node) {

                    // add parent to parents vector
                    parents.push_back(_nodes[i]);
                    break;
                }
            }
        }

        return parents;
    }
}
