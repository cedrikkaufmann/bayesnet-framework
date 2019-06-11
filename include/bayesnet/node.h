//
// Created by Cedrik Kaufmann on 2019-05-28.
//

#ifndef BAYESNET_FRAMEWORK_NODE_H
#define BAYESNET_FRAMEWORK_NODE_H

#include <dai/factorgraph.h>
#include <bayesnet/factor.h>

namespace BayesNet {

    class Node {
    public:
        explicit Node(size_t label, size_t states);

        virtual ~Node() = default;

        dai::Var &getDiscrete() { return _discrete; };

        dai::VarSet &getConditionalDiscrete() { return _conditionalDiscrete; };

        Factor &getFactor();

        void addChild(Node *node);

        void setEvidence(size_t state);

        void clearEvidence();

        void setFactorGraphIndex(size_t index) { this->_factorGraphIndex = index; };

        size_t getFactorGraphIndex() { return this->_factorGraphIndex; };

    private:
        dai::Var _discrete;
        dai::VarSet _conditionalDiscrete;
        Factor _factor;
        size_t _factorGraphIndex;
        std::vector<Node *> _children;
    };
}

#endif //BAYESNET_FRAMEWORK_NODE_H
