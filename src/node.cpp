//
// Created by Cedrik Kaufmann on 2019-05-28.
//

#include "bayesnet/node.h"

namespace BayesNet {

    Node::Node(int label, int states) {
        this->_discrete = dai::Var(label, states);
        this->_conditionalDiscrete = dai::VarSet(this->_discrete);
    }

    void Node::refreshConditionalDiscrete(const dai::VarSet& conditionalDiscrete) {
        this->_conditionalDiscrete |= conditionalDiscrete;

        for (auto & node : this->_children) {
            node->refreshConditionalDiscrete(this->_conditionalDiscrete);
        }
    }

    void Node::addChild(Node *node) {
        this->_children.push_back(node);
        node->refreshConditionalDiscrete(this->_conditionalDiscrete);
    }
}