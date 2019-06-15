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
#include <bayesnet/inference.h>

#include <dai/factor.h>
#include <dai/factorgraph.h>
#include <dai/daialg.h>

namespace BayesNet {

    class Network {
    public:
        Network();

        virtual ~Network() = default;

        void newNode(const std::string &name);

        void newSensor(const std::string &name, size_t type);

        void newConnection(const std::string &parentName, const std::string &childName);

        void init(InferenceProperties inf);

        void setEvidence(const std::string &name, size_t state);

        void clearEvidence(const std::string &name);

        void doInference();

        BayesBelief getBelief(const std::string &name);

        Node &getNode(const std::string &name);

    private:
        size_t _nodeCounter;
        std::unordered_map<std::string, size_t> _registry;
        std::vector<Node> _nodes;
        dai::FactorGraph _factorGraph;
        dai::PropertySet _properties;
        dai::DAIAlgFG *_inferenceInstance;
        bool _init;

        void createInferenceInstance(InferenceProperties inf);
    };
}

#endif //BAYESNET_FRAMEWORK_NETWORK_H
