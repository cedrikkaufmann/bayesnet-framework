//
// Created by Cedrik Kaufmann on 2019-05-28.
//

#include <bayesnet/node.h>
#include <bayesnet/state.h>
#include <include/bayesnet/util.h>

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

    Node::~Node() {

    }

    std::string Node::getName() const {
        return _name;
    }

    dai::Var &Node::getDiscrete() {
        return _discrete;
    }

    dai::Var Node::getDiscrete() const {
        return _discrete;
    }

    dai::VarSet &Node::getConditionalDiscrete() {
        return _conditionalDiscrete;
    }

    dai::VarSet Node::getConditionalDiscrete() const {
        return _conditionalDiscrete;
    }

    std::vector<Node *> &Node::getChildren() {
        return _children;
    }

    CPT &Node::getCPT() {
        return _cpt;
    }

    void Node::setFactorGraphIndex(size_t index) {
        _factorGraphIndex = index;
    }

    size_t Node::getFactorGraphIndex() const {
        return _factorGraphIndex;
    }

    bool Node::isBinary() {
        return _discrete.states() == 2;
    }

    std::ostream &operator<<(std::ostream &os, Node &node) {
        os << node.getDiscrete() << " - " << node.getConditionalDiscrete() << " " << node.getFactor();

        return os;
    }

    SensorNode::SensorNode(const std::string &name, size_t label, size_t states, FuzzySet *set) : Node(name, label,
                                                                                                       states),
                                                                                                  _fuzzySet(set) {

    }

    void SensorNode::observe(double x) {
        std::vector<double> beliefs = _fuzzySet->getBeliefs(x);

        // normalize
        utils::vectorNormalize(beliefs);

        CPT cpt(beliefs);
        setCPT(cpt);
    }

    SensorNode::~SensorNode() {

    }
}
