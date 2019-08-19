//
// Created by Cedrik Kaufmann on 2019-05-28.
//

#include <bayesnet/node.h>
#include <bayesnet/state.h>

namespace bayesNet {

    Node::Node(const std::string &name, size_t label, size_t states) : _name(name), _factor(Factor(states)),
                                                                       _factorGraphIndex(0) {
        _discrete = dai::Var(label, states);
        _conditionalDiscrete = dai::VarSet(_discrete);
    }

    void Node::addChild(Node *node) {
        _children.push_back(node);
        node->_conditionalDiscrete |= _discrete;
    }

    Factor &Node::getFactor() {
        if (_factor.vars() != _conditionalDiscrete) {
            _factor = Factor(_conditionalDiscrete, _discrete.states());
        }

        return _factor;
    }

    void Node::setEvidence(size_t state) {
        getFactor().setEvidence(state);
    }

    void Node::clearEvidence() {
        getFactor().clearEvidence();
    }

    void Node::setCPT(const CPT &cpt) {
        _cpt = cpt;
        Factor &factor = getFactor();

        for (size_t i = 0; i < cpt.size(); ++i) {
            factor.set(i, dai::Real(cpt.get(i)));
        }
    }

    std::ostream &operator<<(std::ostream &os, Node &node) {
        os << node.getDiscrete() << " - " << node.getConditionalDiscrete() << " " << node.getFactor();

        return os;
    }

    void SensorNode::observe(double x) {
        std::vector<double> beliefs = _fuzzySet->getBeliefs(x);
        size_t states = beliefs.size();

        CPT cpt(states);

        // normalize
        double sum = 0;

        for (size_t i = 0; i < states; ++i) {

            sum += beliefs[i];
        }

        for (size_t i = 0; i < states; ++i) {

            cpt.set(i, beliefs[i] / sum);
        }

        setCPT(cpt);
    }
}
