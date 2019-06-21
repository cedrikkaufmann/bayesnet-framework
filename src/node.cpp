//
// Created by Cedrik Kaufmann on 2019-05-28.
//

#include <bayesnet/node.h>
#include <bayesnet/state.h>

namespace BayesNet {

    Node::Node(size_t label, size_t states) : _factorGraphIndex(0) {
        _discrete = dai::Var(label, states);
        _conditionalDiscrete = dai::VarSet(_discrete);
    }

    void Node::addChild(Node *node) {
        _children.push_back(node);
        node->_conditionalDiscrete |= _discrete;
    }

    Factor &Node::getFactor() {
        if (_factor.vars() != _conditionalDiscrete) {
            _factor = Factor(_conditionalDiscrete);
        }

        return _factor;
    }

    void Node::setEvidence(size_t state) {
        getFactor().setEvidence(state);
    }

    void Node::clearEvidence() {
        getFactor().clearEvidence();
    }

    std::ostream &operator<<(std::ostream &os, Node &node) {
        os << node.getDiscrete() << " - " << node.getConditionalDiscrete() << " " << node.getFactor();

        return os;
    }
}
