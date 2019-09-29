#include <map>

#include <bayesnet/network.h>
#include <bayesnet/exception.h>

namespace bayesNet {

    Network::Network() : _inferenceAlgorithm(NULL), _nodeCounter(0), _init(false) {}

    Network::Network(const std::string &file) : _inferenceAlgorithm(NULL), _nodeCounter(0), _init(false) {
        load(file::InitializationVector::parse(file));
    }

    Network::~Network() {}

    void Network::newNode(const std::string &name, bool binary) {
        size_t states;

        // set states based on binary flag
        if (binary) {
            states = 2;
        } else {
            states = 4;
        }

        // look up name and throw exception if already exists
        std::unordered_map<std::string, size_t>::const_iterator search = _registry.find(name);

        if (search != _registry.end()) {
            BAYESNET_THROW(NODE_ALREADY_EXISTS);
        }

        // increment node counter
        size_t nodeValue = _nodeCounter++;
        // use current node counter as label and save in map
        _registry[name] = nodeValue;

        // create new node
        Node *node = new Node(name, nodeValue, states);
        // save node reference
        _nodes.push_back(node);
    }

    void Network::newConnection(const std::string &parentName, const std::string &childName) {
        // lookup labels
        size_t nodeChildValue = _registry[childName];
        size_t nodeParentValue = _registry[parentName];

        // add node as child to parent
        Node *node = _nodes[nodeChildValue];
        _nodes[nodeParentValue]->addChild(node);
    }

    Node *Network::getNode(const std::string &name) {
        // lookup and return node
        size_t nodeValue = _registry.at(name);
        return _nodes[nodeValue];
    }

    void Network::init(inference::Algorithm *alg) {
        // get factors from nodes
        std::vector<dai::Factor> factors;

        for (size_t i = 0; i < _nodes.size(); ++i) {
            factors.push_back(_nodes[i]->getFactor());
        }

        // create factorgraph from factors
        _factorGraph = dai::FactorGraph(factors);

        // lookup factorgraph index and set in node
        for (size_t i = 0; i < _nodes.size(); ++i) {
            size_t factorIndex = _factorGraph.findFactor(_nodes[i]->getConditionalDiscrete());
            _nodes[i]->setFactorGraphIndex(factorIndex);
        }

        // create inference algorithm instance using factorgraph
        _inferenceAlgorithm = alg;
        alg->generateInferenceInstance(_factorGraph);
        alg->getInstance()->init();

        // set initialized flag
        _init = true;
    }

    void Network::setEvidence(const std::string &name, size_t state) {
        // check if initialized
        if (!_init) {
            BAYESNET_THROW(NET_NOT_INITIALIZED);
        }

        try {
            // set evidence on node
            Node *node = getNode(name);

            if (state < node->getDiscrete().states()) {
                node->setEvidence(state);
            } else {
                BAYESNET_THROW(INDEX_OUT_OF_BOUNDS);
            }

            // update factorgraph
            refreshFactorGraph(node);
        } catch (const std::exception &) {
            BAYESNET_THROW(NODE_NOT_FOUND);
        }
    }

    void Network::clearEvidence(const std::string &name) {
        // check if initialized
        if (!_init) {
            BAYESNET_THROW(NET_NOT_INITIALIZED);
        }

        try {
            // clear evidence
            size_t nodeValue = _registry.at(name);
            Node *node = _nodes[nodeValue];
            node->clearEvidence();
            // update factorgraph
            refreshFactorGraph(node);
        } catch (const std::exception &) {
            BAYESNET_THROW(NODE_NOT_FOUND);
        }
    }

    void Network::doInference() {
        // check if initialized
        if (!_init) {
            BAYESNET_THROW(NET_NOT_INITIALIZED);
        }

        // run infernece algorithm
        this->_inferenceAlgorithm->getInstance()->run();
    }

    state::BayesBelief Network::getBelief(const std::string &name) {
        // check if initialized
        if (!_init) {
            BAYESNET_THROW(NET_NOT_INITIALIZED);
        }

        // get node and read belief from factorgraph
        Node *node = getNode(name);

        dai::Factor belief = this->_inferenceAlgorithm->getInstance()->belief(node->getDiscrete());
        state::BayesBelief bayesBelief(node->isBinary());

        for (size_t i = 0; i < belief.nrStates(); ++i) {
            bayesBelief[i] = belief[i];
        }

        return bayesBelief;
    }

    void Network::setCPT(const std::string &name, CPT &cpt) {
        getNode(name)->setCPT(cpt);
    }

    void Network::load(file::InitializationVector *iv) {
        // add nodes to network
        std::vector<file::Node *> &nodes = iv->getNodes();

        for (size_t i = 0; i < nodes.size(); i++) {

            if (nodes[i]->isSensor()) {
                newSensorNode(nodes[i]->getName(), nodes[i]->isBinary());
                continue;
            }

            newNode(nodes[i]->getName(), nodes[i]->isBinary());
        }
        
        // add connections for nodes to network
        std::unordered_map<std::string, std::vector<std::string> > &connections = iv->getConnections();

        for (std::unordered_map<std::string, std::vector<std::string> >::const_iterator it = connections.begin(); it != connections.end(); it++) {
            for (size_t i = 0; i < (*it).second.size(); ++i) {
                newConnection((*it).first, (*it).second[i]);
            }
        }

        // add cpt for nodes to network
        std::unordered_map<std::string, std::vector<double> > &cpts = iv->getCPTs();

        for (std::unordered_map<std::string, std::vector<double> >::const_iterator it = cpts.begin(); it != cpts.end(); it++) {
            CPT cpt(it->second);
            setCPT(it->first, cpt);
        }

        // add fuzzy sets for sensor nodes
        std::unordered_map<std::string, std::vector<std::string> > &fuzzySets = iv->getFuzzySets();

        for (std::unordered_map<std::string, std::vector<std::string> >::const_iterator it = fuzzySets.begin(); it != fuzzySets.end(); it++) {
            for (size_t i = 0; i < (*it).second.size(); ++i) {
                // compile membership function from string
                fuzzyLogic::MembershipFunction *mf = fuzzyLogic::membershipFunctions::fromString((*it).second[i]);
                setMembershipFunction((*it).first, i, mf);
            }
        }

        // generate inference algorithm from string
        inference::Algorithm *algorithm = new inference::Algorithm(iv->getInferenceAlgorithm());

        // initialize network
        init(algorithm);
    }

    void Network::save(const std::string &filename) {
        file::InitializationVector *iv = new file::InitializationVector();

        for (size_t i = 0; i < _nodes.size(); ++i) {
            bool isSensor = false;

            // add node names to iv
            SensorNode *sensorNode = dynamic_cast<SensorNode *>(_nodes[i]);

            if (sensorNode != NULL) {
                isSensor = true;
            }

            if (_nodes[i]->isBinary()) {
                iv->addNode(_nodes[i]->getName(), 2, isSensor);
            } else {
                iv->addNode(_nodes[i]->getName(), 4, isSensor);
            }

            // add connections to iv
            std::vector<Node *> children = _nodes[i]->getChildren();

            std::vector<std::string> connections;

            for (size_t j = 0; j < children.size(); ++j) {
                connections.push_back(children[j]->getName());
            }

            if (connections.size() > 0) {
                iv->setConnections(_nodes[i]->getName(), connections);
            }

            // add cpt to iv
            std::vector<double> &probabilies = _nodes[i]->getCPT().getProbabilities();
            
            if (probabilies.size() > 0) {
                iv->setCPT(_nodes[i]->getName(), _nodes[i]->getCPT().getProbabilities());
            }

            // add fuzzy sets
            //if (isSensor) {
            fuzzyLogic::Set &fuzzySet = _nodes[i]->getFuzzySet();
            std::vector<std::string> curves(fuzzySet.nrStates());

            for (size_t j = 0; j < fuzzySet.nrStates(); j++) {
                fuzzyLogic::MembershipFunction *mf = fuzzySet.getMembershipFunction(j);

                if (mf != NULL) {
                    curves[j] = fuzzySet.getMembershipFunction(j)->toString();
                } else {
                    curves[j] = "NULL";
                }
            }

            iv->setFuzzySet(_nodes[i]->getName(), curves);
            //}

            // add inference algorithm
            if (_init && !_inferenceAlgorithm->getFilename().empty()) {
                iv->setInferenceAlgorithm(_inferenceAlgorithm->getFilename());
            }
        }

        iv->save(filename);
    }

    void Network::save(const std::string &networkFilename, const std::string &algorithmFilename) {
        if (_inferenceAlgorithm != NULL) {
            _inferenceAlgorithm->save(algorithmFilename);
        }

        save(networkFilename);
    }

    void Network::newSensorNode(const std::string &name, bool binary) {
        size_t states;

        // set states based on binary flag
        if (binary) {
            states = 2;
        } else {
            states = 4;
        }

        // lookup name if already exists
        std::unordered_map<std::string, size_t>::const_iterator search = _registry.find(name);

        if (search != _registry.end()) {
            BAYESNET_THROW(NODE_ALREADY_EXISTS);
        }

        // increment node counter
        size_t nodeValue = _nodeCounter++;
        // use node counter as label
        _registry[name] = nodeValue;

        // create new node
        Node *node = new SensorNode(name, nodeValue, states);
        // save reference to node 
        _nodes.push_back(node);
    }

    void Network::observe(const std::string &name, double x) {
        // cast node to sensor node
        SensorNode *node = dynamic_cast<SensorNode *>(getNode(name));
        // set oberved variable
        node->observe(x);
        // update factorgraph
        refreshFactorGraph(node);
    }

    void Network::refreshFactorGraph(Node *node) {
        if (!_init) {
            BAYESNET_THROW(NET_NOT_INITIALIZED);
        }

        // update corresponding factor in factorgraph
        _inferenceAlgorithm->getInstance()->fg().setFactor(node->getFactorGraphIndex(), node->getFactor(), false);
        _inferenceAlgorithm->getInstance()->init(node->getConditionalDiscrete());
    }

    void Network::setMembershipFunction(const std::string &name, size_t state, fuzzyLogic::MembershipFunction *mf) {
        Node *node = getNode(name);
        node->setMembershipFunction(state, mf);
    }

    void Network::inferCPT() {
        for (size_t i = 0; i < _availableFuzzySets.size(); ++i) {
            inferCPT(_availableFuzzySets[i]);
        }
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
        fuzzyLogic::Controller inferenceCtrl(fuzzySets, &node->getFuzzyRules(), 0.01);

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

    std::vector<Node *> Network::getParents(const std::string &name) {
        Node *node = getNode(name);
        return getParents(node);
    }

    void Network::setFuzzyRules(const std::string &file) {
        // prepare states
        fuzzyLogic::RuleState *binaryStates[2]= {
                FUZZY_STATE(FALSE),
                FUZZY_STATE(TRUE)
        };

        fuzzyLogic::RuleState *states[4]= {
                FUZZY_STATE(GOOD),
                FUZZY_STATE(PROBABLY_GOOD),
                FUZZY_STATE(PROBABLY_BAD),
                FUZZY_STATE(BAD)
        };

        std::vector<bayesNet::file::FuzzyRuleVector *> v = bayesNet::file::FuzzyRuleVector::parse(file);

        for (size_t i = 0; i < v.size(); ++i) {
            // get node
            Node *node = getNode(v[i]->getName());

            // setup fuzzy rule set
            std::vector<bayesNet::fuzzyLogic::Rule *> fuzzyRules;

            // iterate over rules
            std::vector<bayesNet::file::FuzzyRule *> rules = v[i]->getRules();

            for (size_t j = 0; j < rules.size(); ++j) {
                std::unordered_map<std::string, size_t> ifClauses = rules[j]->getIfClauses();
                std::vector<fuzzyLogic::RuleState *> ruleStates;

                // iterate over hash map and save elements using label in map to sort
                std::map<size_t, fuzzyLogic::RuleState *> sortedClauses;

                for (std::unordered_map<std::string, size_t>::const_iterator it = ifClauses.begin(); it != ifClauses.end(); it++) {
                    Node *parent = getNode(it->first);

                    if (parent->isBinary()) {
                        sortedClauses[parent->getDiscrete().label()] = binaryStates[it->second];
                    } else {
                        sortedClauses[parent->getDiscrete().label()] = states[it->second];
                    }
                }

                // iterate over sorted map and add rule states
                for (std::map<size_t, fuzzyLogic::RuleState*>::const_iterator it = sortedClauses.begin(); it != sortedClauses.end(); it++) {
                    ruleStates.push_back(it->second);
                }

                // create fuzzy rule from states
                fuzzyLogic::RuleState *thenState;

                if (node->isBinary()) {
                    thenState = binaryStates[rules[j]->getThenClause()];
                } else {
                    thenState = states[rules[j]->getThenClause()];
                }

                fuzzyLogic::Rule *rule = new fuzzyLogic::Rule(ruleStates, thenState);
                fuzzyRules.push_back(rule);
            }

            // set fuzzy set for node
            fuzzyLogic::RuleSet set(fuzzyRules);
            node->setFuzzyRules(set);
            _availableFuzzySets.push_back(node->getName());
        }
    }
}
