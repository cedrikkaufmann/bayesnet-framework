//
// Created by Cedrik Kaufmann on 2019-05-28.
//

#ifndef BAYESNET_FRAMEWORK_NODE_H
#define BAYESNET_FRAMEWORK_NODE_H

#include <dai/factorgraph.h>
#include <bayesnet/factor.h>
#include <bayesnet/cpt.h>
#include <bayesnet/fuzzy.h>

namespace bayesNet {

    class Node {
    public:
        explicit Node(const std::string &name, size_t label, size_t states);

        virtual ~Node();

        std::string getName() const;

        dai::Var &getDiscrete();

        dai::Var getDiscrete() const;

        dai::VarSet &getConditionalDiscrete();

        dai::VarSet getConditionalDiscrete() const;

        Factor &getFactor();

        std::vector<Node *> &getChildren();

        void addChild(Node *node);

        void setEvidence(size_t state);

        void clearEvidence();

        CPT &getCPT();

        void setCPT(const CPT &cpt);

        void setFactorGraphIndex(size_t index);

        size_t getFactorGraphIndex() const;

        bool isBinary();

    private:
        std::string _name;
        dai::Var _discrete;
        dai::VarSet _conditionalDiscrete;
        Factor _factor;
        size_t _factorGraphIndex;
        CPT _cpt;
        std::vector<Node *> _children;
    };

    class SensorNode : public Node {
    public:
        explicit SensorNode(const std::string &name, size_t label, size_t states, fuzzyLogic::Set *set);

        virtual ~SensorNode();

        void observe(double x);

    private:
        fuzzyLogic::Set *_fuzzySet;
    };

    std::ostream &operator<<(std::ostream &os, Node &node);
}

#endif //BAYESNET_FRAMEWORK_NODE_H
