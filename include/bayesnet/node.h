//
// Created by Cedrik Kaufmann on 2019-05-28.
//

#ifndef BAYESNET_FRAMEWORK_NODE_H
#define BAYESNET_FRAMEWORK_NODE_H

#include <dai/factorgraph.h>
#include <bayesnet/factor.h>
#include <bayesnet/cpt.h>

namespace bayesNet {

    class Node {
    public:
        explicit Node(const std::string& name, size_t label, size_t states);

        std::string getName() const { return _name; }

        dai::Var &getDiscrete() { return _discrete; }

        dai::Var getDiscrete() const { return _discrete; }

        dai::VarSet &getConditionalDiscrete() { return _conditionalDiscrete; }

        dai::VarSet getConditionalDiscrete() const { return _conditionalDiscrete; }

        Factor &getFactor();

        std::vector<Node *> &getChildren() { return _children; }

        void addChild(Node *node);

        void setEvidence(size_t state);

        void clearEvidence();

        CPT &getCPT() { return _cpt; }

        void setCPT(const CPT& cpt);

        void setFactorGraphIndex(size_t index) { _factorGraphIndex = index; }

        size_t getFactorGraphIndex() const { return _factorGraphIndex; }

        bool isBinary() { return _discrete.states() == 2; }

    private:
        std::string _name;
        dai::Var _discrete;
        dai::VarSet _conditionalDiscrete;
        Factor _factor;
        size_t _factorGraphIndex;
        CPT _cpt;
        std::vector<Node *> _children;
    };

    std::ostream &operator<<(std::ostream &os, Node &node);
}

#endif //BAYESNET_FRAMEWORK_NODE_H
