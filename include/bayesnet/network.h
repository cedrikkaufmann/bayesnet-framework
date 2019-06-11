//
// Created by Cedrik Kaufmann on 2019-06-06.
//

#ifndef BAYESNET_FRAMEWORK_NETWORK_H
#define BAYESNET_FRAMEWORK_NETWORK_H

#include <string>
#include <vector>
#include <unordered_map>

#include <bayesnet/node.h>

#include <dai/alldai.h>
#include <dai/bp.h>
#include <dai/cbp.h>
#include <dai/fbp.h>
#include <dai/trwbp.h>
#include <dai/decmap.h>

#define BP_MAXIMUM_ITERATIONS 1000
#define BP_TOLERANCE 1e-9
#define BP_VERBOSE 1
#define BP_UPDATES "SEQRND"
#define BP_INFERENCE "SUMPROD"
#define BP_LOG_DOMAIN false

namespace BayesNet {

    enum InferenceAlgorithm {
        LOOPY_BELIEF_PROPAGATION = 0,
        CONDITIONED_BELIEF_PROPAGATION = 1,
        FRACTIONAL_BELIEF_PROPAGATION = 2,
        TREE_REWEIGHTED_BELIEF_PROPAGATION = 3,
        DECIMATION = 4,
    };

    class Network {
    public:
        Network();

        virtual ~Network() = default;

        void newNode(const std::string &name);

        void newSensor(const std::string &name, size_t type);

        void newConnection(const std::string &parentName, const std::string &childName);

        void init(InferenceAlgorithm algorithm);

        void setEvidence(const std::string &name, size_t state);

        void clearEvidence(const std::string &name);

        void doInference();

        dai::Factor getBelief(const std::string &name);

        Node &getNode(const std::string &name);

    private:
        size_t _nodeCounter;
        std::unordered_map<std::string, size_t> _registry;
        std::vector<Node> _nodes;
        dai::FactorGraph _factorGraph;
        dai::PropertySet _properties;
        dai::DAIAlgFG *_inferenceInstance;
        bool _init;

        void createInferenceInstance(InferenceAlgorithm algorithm);
    };
}

#endif //BAYESNET_FRAMEWORK_NETWORK_H
