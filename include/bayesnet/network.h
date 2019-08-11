//
// Created by Cedrik Kaufmann on 2019-06-06.
//

#ifndef BAYESNET_FRAMEWORK_NETWORK_H
#define BAYESNET_FRAMEWORK_NETWORK_H

#include <string>
#include <vector>
#include <unordered_map>

#include <bayesnet/node.h>
#include <bayesnet/state.h>
#include <bayesnet/cpt.h>
#include <bayesnet/inference.h>
#include <bayesnet/json.h>

#include <dai/factor.h>
#include <dai/factorgraph.h>
#include <dai/daialg.h>

namespace bayesNet {

    class Network {
    public:
        Network() : _inferenceAlgorithm(nullptr), _nodeCounter(0), _init(false) {}

        explicit Network(const std::string &file);

        void newNode(const std::string &name);

        void newBinaryNode(const std::string &name);

        //void newSensor(const std::string &name, size_t type);

        void newConnection(const std::string &parentName, const std::string &childName);

        void init(inference::Algorithm *alg);

        void setEvidence(const std::string &name, belief::BeliefState state);

        void clearEvidence(const std::string &name);

        void setCPT(const std::string &name, const CPT &cpt);

        void doInference();

        belief::BayesBelief getBelief(const std::string &name);

        Node *getNode(const std::string &name);

        void load(const std::string &file);

        void load(json::InitializationVector *iv);

        void save(const std::string &file);

    private:
        std::unordered_map<std::string, size_t> _registry;
        std::vector<std::string> _nodeNames;
        std::vector<Node *> _nodes;
        dai::FactorGraph _factorGraph;
        inference::Algorithm *_inferenceAlgorithm;
        size_t _nodeCounter;
        bool _init;

        void newNode(const std::string &name, size_t states);
    };
}

#endif //BAYESNET_FRAMEWORK_NETWORK_H
