//
// Created by Cedrik Kaufmann on 2019-05-28.
//

#ifndef BAYESNET_FRAMEWORK_NODE_H
#define BAYESNET_FRAMEWORK_NODE_H

#include <dai/factorgraph.h>

namespace BayesNet {

    class Node {
    public:
        Node(int label, int states);

        virtual ~Node() = default;

        dai::Var &discrete();
        dai::VarSet &conditionalDiscrete();
        dai::Factor &factor();
        void addChild(Node *node);
    private:
        dai::Var _discrete;
        dai::VarSet _conditionalDiscrete;
        dai::Factor _factor;
        std::vector<Node*> _children;

        void refreshConditionalDiscrete(const dai::VarSet& conditionalDiscrete);
    };
}

#endif //BAYESNET_FRAMEWORK_NODE_H
