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
#include <bayesnet/fuzzy.h>
#include <bayesnet/file.h>

#include <dai/factor.h>
#include <dai/factorgraph.h>
#include <dai/daialg.h>

namespace bayesNet {

    class Network {
    public:
        Network();

        explicit Network(const std::string &file);

        virtual ~Network();

        void newNode(const std::string &name, bool binary = false);

        void newSensorNode(const std::string &name, bool binary = false);

        void newConnection(const std::string &parentName, const std::string &childName);

        void init(inference::Algorithm *alg);

        void setEvidence(const std::string &name, size_t state);

        void setMembershipFunction(const std::string &name, size_t state, fuzzyLogic::MembershipFunction *mf);

        void observe(const std::string &name, double x);

        void clearEvidence(const std::string &name);

        void setCPT(const std::string &name, const CPT &cpt);

        void setFuzzyRules(const std::string &file);

        void inferCPT(const std::string &name);

        void doInference();

        Node *getNode(const std::string &name);

        std::vector<Node *> getParents(const std::string &name);

        state::BayesBelief getBelief(const std::string &name);

        void save(const std::string &filename);

        void save(const std::string &networkFilename, const std::string &algorithmFilename);

    private:
        std::unordered_map<std::string, size_t> _registry;
        std::vector<std::string> _nodeNames;
        std::vector<Node *> _nodes;
        dai::FactorGraph _factorGraph;
        inference::Algorithm *_inferenceAlgorithm;
        size_t _nodeCounter;
        bool _init;

        void load(file::InitializationVector *iv);

        void refreshFactorGraph(Node *node);

        std::vector<Node *> getParents(Node *node);
    };
}

#endif //BAYESNET_FRAMEWORK_NETWORK_H
