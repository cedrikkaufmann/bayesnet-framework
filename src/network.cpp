#include <map>
#include <iostream>
#include <fstream>
#include <string>

#include <bayesnet/network.h>
#include <bayesnet/exception.h>
#include <bayesnet/util.h>


namespace bayesNet {

    Network::Network() : _nodeCounter(0), _init(false) {}

    Network::Network(size_t type) : _inferenceAlgorithm(type), _nodeCounter(0), _init(false) {}

    Network::Network(const inference::Algorithm &algorithm) : _inferenceAlgorithm(algorithm), _nodeCounter(0), _init(false) {}

    Network::Network(const std::string &file) : _nodeCounter(0), _init(false) {
        file::InitializationVector *iv = file::InitializationVector::parse(file);
        load(iv);

        // free memory for iv
        delete iv;
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
            BAYESNET_THROWE(NODE_ALREADY_EXISTS, name);
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

    Node &Network::getNode(const std::string &name) {
        // lookup and return node
        size_t nodeValue = _registry.at(name);
        return *_nodes[nodeValue];
    }

    void Network::init() {
        // create inference algorithm instance using nodes
        _inferenceAlgorithm.init(_nodes);

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
            Node &node = getNode(name);

            if (state < node.getDiscrete().states()) {
                node.setEvidence(state);
            } else {
                BAYESNET_THROW(INDEX_OUT_OF_BOUNDS);
            }

            // update inference instance
            _inferenceAlgorithm.init(node);
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
            Node &node = getNode(name);
            node.clearEvidence();

            // update inference instance
            _inferenceAlgorithm.init(node);
        } catch (const std::exception &) {
            BAYESNET_THROW(NODE_NOT_FOUND);
        }
    }

    void Network::run() {
        // check if initialized
        if (!_init) {
            BAYESNET_THROW(NET_NOT_INITIALIZED);
        }

        // run infernece algorithm
        _inferenceAlgorithm.run();
    }

    state::BayesBelief Network::getBelief(const std::string &name) {
        // check if initialized
        if (!_init) {
            BAYESNET_THROW(NET_NOT_INITIALIZED);
        }

        // get node and read belief from inference instance
        Node &node = getNode(name);
        return _inferenceAlgorithm.belief(node);
    }

    double Network::getContinousBelief(const std::string &name) {
        // check if initialized
        if (!_init) {
            BAYESNET_THROW(NET_NOT_INITIALIZED);
        }

        // get node and 
        Node &node = getNode(name);
        // read belief from inference instance
        auto belief = _inferenceAlgorithm.belief(node);
      
        size_t nrStates = belief.nrStates();
        double continousBelief;

        for (size_t i = 0; i < nrStates; i++) {
            continousBelief += belief[i] * i;
        }

        // normalize to a range of 2 and move by -1 thus we get a value from -1 to 1, where -1 is worst and 1 best state
        continousBelief *= (2.0 / (nrStates - 1));
        continousBelief -= 1;

        // if node is not binary inverse value
        if (!node.isBinary()) {
            continousBelief *= -1;
        }
        
        // return continous belief
        return continousBelief;
    }

    void Network::setCPT(const std::string &name, const CPT &cpt) {
        getNode(name).setCPT(cpt);
    }

    void Network::save(const std::string &filename) {
        file::InitializationVector *iv = new file::InitializationVector();

        for (size_t i = 0; i < _nodes.size(); ++i) {
            // add node names to iv
            if (_nodes[i]->isBinary()) {
                iv->addNode(_nodes[i]->getName(), 2, isSensor(*_nodes[i]));
            } else {
                iv->addNode(_nodes[i]->getName(), 4, isSensor(*_nodes[i]));
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
            fuzzyLogic::FuzzySet &fuzzySet = _nodes[i]->getFuzzySet();
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

            // add inference algorithm
            if (!_inferenceAlgorithm.getFilename().empty()) {
                iv->setInferenceAlgorithm(_inferenceAlgorithm.getFilename());
            }
        }

        iv->save(filename);
    }

    void Network::save(const std::string &networkFilename, const std::string &algorithmFilename) {
        _inferenceAlgorithm.save(algorithmFilename);
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
            BAYESNET_THROWE(NODE_ALREADY_EXISTS, name);
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
        Node &node = getNode(name);
        // get SensorNode from Node instance
        SensorNode &sensor = getSensor(node);
        // set oberved variable
        sensor.observe(x);

        // update inference instance
        _inferenceAlgorithm.init(sensor);
    }

    void Network::setMembershipFunction(const std::string &name, size_t state, const std::string &mf) {
        Node &node = getNode(name);
        fuzzyLogic::MembershipFunction *instance = fuzzyLogic::membershipFunctions::fromString(mf);
        node.setMembershipFunction(state, instance);
    }

    void Network::inferCPT() {
        for (size_t i = 0; i < _availableFuzzySets.size(); ++i) {
            inferCPT(_availableFuzzySets[i]);
        }
    }

    void Network::inferCPT(const std::string &name) {
        Node &node = getNode(name);
        std::vector<Node *> parents = getParents(node);

        std::vector<fuzzyLogic::FuzzySet *> fuzzySets(parents.size());

        // collect fuzzy sets
        for (size_t i = 0; i < parents.size(); ++i) {
            fuzzySets[i] = &parents[i]->getFuzzySet();
        }

        // create inference controller instance
        fuzzyLogic::Controller inferenceCtrl(fuzzySets, &node.getFuzzyRules(), 0.01);

        // infer cpt
        CPT cpt = inferenceCtrl.inferCPT();

        // set cpt for node
        setCPT(name, cpt);
    }

    std::vector<Node *> Network::getParents(Node &node) {
        std::vector<Node *> parents;

        // iterate over all nodes
        for (size_t i = 0; i < _nodes.size(); ++i) {
            std::vector<Node *> &children = _nodes[i]->getChildren();

            // iterate over children
            for (size_t j = 0; j < children.size(); ++j) {
                // check if node has given node as child
                if (children[j] == &node) {
                    // add parent to parents vector
                    parents.push_back(_nodes[i]);
                    break;
                }
            }
        }

        return parents;
    }

    std::vector<Node *> Network::getParents(const std::string &name) {
        Node &node = getNode(name);
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
            Node &node = getNode(v[i]->getName());

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
                    Node &parent = getNode(it->first);

                    if (parent.isBinary()) {
                        sortedClauses[parent.getDiscrete().label()] = binaryStates[it->second];
                    } else {
                        sortedClauses[parent.getDiscrete().label()] = states[it->second];
                    }
                }

                // iterate over sorted map and add rule states
                for (std::map<size_t, fuzzyLogic::RuleState*>::const_iterator it = sortedClauses.begin(); it != sortedClauses.end(); it++) {
                    ruleStates.push_back(it->second);
                }

                // create fuzzy rule from states
                fuzzyLogic::RuleState *thenState;

                if (node.isBinary()) {
                    thenState = binaryStates[rules[j]->getThenClause()];
                } else {
                    thenState = states[rules[j]->getThenClause()];
                }

                fuzzyLogic::Rule *rule = new fuzzyLogic::Rule(ruleStates, thenState);
                fuzzyRules.push_back(rule);
            }

            // set fuzzy set for node
            fuzzyLogic::RuleSet set(fuzzyRules);
            node.setFuzzyRules(set);
            _availableFuzzySets.push_back(node.getName());
        }
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
                newConnection(it->first, it->second[i]);
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
                // set membership function
                setMembershipFunction(it->first, i, it->second[i]);
            }
        }

        // generate inference algorithm from string
        if (!iv->getInferenceAlgorithm().empty()) {
            _inferenceAlgorithm = inference::Algorithm(iv->getInferenceAlgorithm());
        } else {
            // no algorithm file given, use default
            _inferenceAlgorithm = inference::Algorithm();
        }   
    }

    SensorNode &Network::getSensor(Node &node) {
        // cast node to sensor node
        SensorNode *sensor = dynamic_cast<SensorNode *>(&node);

        // node is no Sensor
        if (sensor == NULL) {
            BAYESNET_THROWE(NO_SENSOR, node.getName());
        }

        return *sensor;
    }

    bool Network::isSensor(Node &node) {
        // cast node to sensor node
        SensorNode *sensor = dynamic_cast<SensorNode *>(&node);

        // node is no Sensor
        if (sensor == NULL) {
            return false;
        }

        // node is sensor
        return true;
    }

    void Network::generateDefaultFuzzyRules(const std::string &file, const std::string &generatorLogicFile) {
        // open file
        std::ofstream fuzzyRuleFile(file);

        if (!fuzzyRuleFile.is_open()) {
            throw std::runtime_error("cannot write fuzzy rule file");
        }

        // define generator logic
        // TODO: load from file
        /*
        bayesNet::fuzzyLogic::MembershipFunction *mfGood = new bayesNet::fuzzyLogic::membershipFunctions::ZShape(0, 1);
        bayesNet::fuzzyLogic::MembershipFunction *mfProbablyGood = new bayesNet::fuzzyLogic::membershipFunctions::Triangle(0, 1, 2);
        bayesNet::fuzzyLogic::MembershipFunction *mfProbablyBad = new bayesNet::fuzzyLogic::membershipFunctions::Triangle(1, 2, 3);
        bayesNet::fuzzyLogic::MembershipFunction *mfBad = new bayesNet::fuzzyLogic::membershipFunctions::SShape(2, 3);

        bayesNet::fuzzyLogic::FuzzySet *generatorLogic = new bayesNet::fuzzyLogic::FuzzySet(4);
        generatorLogic->setMembershipFunction(bayesNet::state::GOOD, mfGood);
        generatorLogic->setMembershipFunction(bayesNet::state::PROBABLY_GOOD, mfProbablyGood);
        generatorLogic->setMembershipFunction(bayesNet::state::PROBABLY_BAD, mfProbablyBad);
        generatorLogic->setMembershipFunction(bayesNet::state::BAD, mfBad);
        */
        // end generator logic, tbd load from file instead of static define

        // read generator logic
        file::GeneratorLogic generatorLogic(generatorLogicFile);
        generatorLogic.parse();

        // write fuzzy rules
        // iterate over all network nodes
        for (auto node : _nodes)  {
            // check if node has any parents
            auto parents = getParents(*node);

            if (parents.size() > 0) {
                // get logic for node
                auto nodeLogic = generatorLogic.getNodeLogic(node->getName());

                // create fuzzy set
                bayesNet::fuzzyLogic::FuzzySet* generatorFuzzySet;
                
                if (node->isBinary()) {
                    generatorFuzzySet = new fuzzyLogic::FuzzySet(2);

                    // get membership functions
                    auto mfTrue = fuzzyLogic::membershipFunctions::fromString(nodeLogic->mf[state::TRUE]);
                    auto mfFalse = fuzzyLogic::membershipFunctions::fromString(nodeLogic->mf[state::FALSE]);

                    generatorFuzzySet->setMembershipFunction(bayesNet::state::TRUE, mfTrue);
                    generatorFuzzySet->setMembershipFunction(bayesNet::state::FALSE, mfFalse);
                } else {
                    generatorFuzzySet = new fuzzyLogic::FuzzySet(4);

                    // get membership functions
                    auto mfGood = fuzzyLogic::membershipFunctions::fromString(nodeLogic->mf[state::GOOD]);
                    auto mfProbablyGood = fuzzyLogic::membershipFunctions::fromString(nodeLogic->mf[state::PROBABLY_GOOD]);
                    auto mfProbablyBad = fuzzyLogic::membershipFunctions::fromString(nodeLogic->mf[state::PROBABLY_BAD]);
                    auto mfBad = fuzzyLogic::membershipFunctions::fromString(nodeLogic->mf[state::BAD]);

                    generatorFuzzySet->setMembershipFunction(bayesNet::state::GOOD, mfGood);
                    generatorFuzzySet->setMembershipFunction(bayesNet::state::PROBABLY_GOOD, mfProbablyGood);
                    generatorFuzzySet->setMembershipFunction(bayesNet::state::PROBABLY_BAD, mfProbablyBad);
                    generatorFuzzySet->setMembershipFunction(bayesNet::state::BAD, mfBad);
                }

                // write node header
                fuzzyRuleFile << node->getName() << " begin\n";
                
                // calculate joint states
                std::vector<size_t> maxStates;
                std::vector<std::string> nodeNames;
            
                for (auto parent : parents) {
                    if (parent->isBinary()) {
                        maxStates.push_back(2);
                    } else {
                        maxStates.push_back(4);
                    }

                    nodeNames.push_back(parent->getName());
                }

                utils::Counter stateCounter(parents.size(), maxStates);
                
                do {
                    auto currentStates = stateCounter.getCount();
                    std::string rule("   if ");

                    // calculate quality based on generator logic
                    // assume node weights of 1 for now
                    double quality = 0;
                   
                    for (size_t i = 0; i < currentStates.size(); i++) {
                        // get weight for node
                        auto weight = nodeLogic->weights[nodeNames[i]];

                        rule += nodeNames[i] + "=";

                        if (maxStates[i] == 4) {
                            // node with 4 states
                            switch (currentStates[i]) {
                            case 0: 
                                rule += "good";
                                quality += bayesNet::state::GOOD * weight;
                                break;

                            case 1:
                                rule += "probably_good";
                                quality += bayesNet::state::PROBABLY_GOOD * weight;
                                break;

                            case 2:
                                rule += "probably_bad";
                                quality += bayesNet::state::PROBABLY_BAD * weight;
                                break;

                            case 3:
                                rule += "bad";
                                quality += bayesNet::state::BAD * weight;
                                break;
                            
                            default:
                                break;
                            }
                        } else {
                            // binary node
                            switch (currentStates[i]) {
                            case 0:
                                rule += "true";
                                quality += bayesNet::state::TRUE * weight;
                                break;

                            case 1:
                                rule += "false";
                                quality += bayesNet::state::FALSE * weight;
                                break;
                            
                            default:
                                break;
                            }
                        }

                        if (i < currentStates.size() - 1) {
                            rule += " & ";
                        }
                    }

                    std::vector<double> strength = generatorFuzzySet->getStrength(quality / (double)currentStates.size());
                    size_t max = 0;

                    for (size_t i = 1; i < strength.size(); i++) {
                        if (strength[i] >= strength[max]) {
                            max = i;
                        }
                    }
                    
                    if (node->isBinary()) {
                        switch (max) {
                        case bayesNet::state::TRUE:
                            rule += " then true\n";
                            break;

                        case bayesNet::state::FALSE:
                            rule += " then false\n";
                            break;
                        
                        default:
                            break;
                        }
                    } else {
                        switch (max) {
                        case bayesNet::state::GOOD:
                            rule += " then good\n";
                            break;

                        case bayesNet::state::PROBABLY_GOOD:
                            rule += " then probably_good\n";
                            break;

                        case bayesNet::state::PROBABLY_BAD:
                            rule += " then probably_bad\n";
                            break;

                        case bayesNet::state::BAD:
                            rule += " then bad\n";
                            break;
                        
                        default:
                            break;
                        }
                    }
                   
                    fuzzyRuleFile << rule;
                } while (stateCounter.countUp());

                // write node end
                fuzzyRuleFile << "end\n\n";

                // delete generator fuzzy logic
                delete generatorFuzzySet;
            } 
        }
        

        // close file
        fuzzyRuleFile.close();        
    }
}
