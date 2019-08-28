//
// Created by Cedrik Kaufmann on 2019-08-16.
//

#include <math.h>
#include <limits>

#include <bayesnet/fuzzy.h>
#include <bayesnet/util.h>

namespace bayesNet {

    namespace membershipFunctions {

        double Linear::fx(double x) {
            if (_fxMin < _fxMax) {

                if (x <= _fxMin) {

                    return 0;
                }

                if (x >= _fxMax) {

                    return 1;
                }

            } else {

                if (x <= _fxMax) {

                    return 1;
                }

                if (x >= _fxMin) {

                    return 0;
                }
            }

            return (_m * (x - _b));
        }

        Linear::Linear(double fxMin, double fxMax) : _fxMin(fxMin), _fxMax(fxMax) {
            _m = 1 / (fxMax - fxMin);
            _b = fxMin;
        }

        double Linear::findMaximum() {
            return _fxMax;
        }

        Triangle::Triangle(double begin, double max, double end) : _begin(begin), _max(max), _end(end),
                                                                   _increasing(_begin, _max),
                                                                   _decreasing(_end, _max) {

        }

        double Triangle::fx(double x) {
            if (x <= _begin || x >= _end) {

                return 0;
            }

            if (x > _begin && x < _max) {

                return _increasing.fx(x);
            }

            if (x > _max && x < _end) {

                return _decreasing.fx(x);
            }

            return 1;
        }

        double Triangle::findMaximum() {
            return _max;
        }

        Trapezoid::Trapezoid(double x1, double x2, double x3, double x4) : _increasingBegin(x1),
                                                                           _increasingEnd(x2), _decreasingBegin(x3),
                                                                           _decreasingEnd(x4),
                                                                           _increasingLinear(x1, x2),
                                                                           _decreasingLinear(x4, x3) {

        }

        double Trapezoid::fx(double x) {
            if (x <= _increasingBegin || x >= _decreasingEnd) {

                return 0;
            }

            if (x >= _increasingEnd && x <= _decreasingBegin) {

                return 1;
            }

            if (x > _increasingBegin && x < _increasingEnd) {

                return _increasingLinear.fx(x);
            }

            return _decreasingLinear.fx(x);
        }

        double Trapezoid::findMaximum() {
            return (_decreasingBegin - _increasingEnd) / 2.0;
        }

        SShape::SShape(double a, double b) : _a(a), _b(b) {

        }

        double SShape::fx(double x) {
            if (x <= _a) {

                return 0;
            }

            if (x >= _b) {

                return 1;
            }

            if (x > _a && x <= (_a + _b) / 2) {

                return 2 * std::pow((x - _a) / (_b - _a), 2);
            }

            return 1 - 2 * std::pow((x - _b) / (_b - _a), 2);
        }

        double SShape::getMinPos() {
            return _a;
        }

        double SShape::getMaxPos() {
            return _b;
        }

        double SShape::findMaximum() {
            return _b;
        }

        ZShape::ZShape(double a, double b) : _sShape(a, b) {

        }

        double ZShape::fx(double x) {
            return 1 - _sShape.fx(x);
        }

        double ZShape::getMaxPos() {
            return _sShape.getMaxPos();
        }

        double ZShape::getMinPos() {
            return _sShape.getMinPos();
        }

        double ZShape::findMaximum() {
            return _sShape.getMinPos();
        }

        double PiShape::fx(double x) {
            if (x <= _zShape.getMaxPos()) {

                return _sShape.fx(x);
            }

            return _zShape.fx(x);
        }

        double Sigmoidal::fx(double x) {
            return 1 / (1 + std::exp(-1 * _a * (x - _c)));
        }

        Bell::Bell(double a, double b, double c) : _a(a), _b(b), _c(c) {

        }

        double Bell::fx(double x) {
            return 1 / (1 + std::pow(std::abs((x - _c) / _a), 2 * _b));
        }

        double Bell::findMaximum() {
            return _c;
        }

        double Gaussian::fx(double x) {
            return std::exp(-std::pow(x - _mean, 2) / (2 * std::pow(_deviation, 2)));
        }

        Gaussian::Gaussian(double mean, double deviation) : _mean(mean), _deviation(deviation) {

        }

        double Gaussian::getMean() {
            return _mean;
        }

        double Gaussian::findMaximum() {
            return _mean;
        }

        Gaussian2::Gaussian2(double meanLeft, double deviationLeft, double meanRight, double deviationRight) : _left(
                meanLeft, deviationLeft), _right(meanRight, deviationRight) {

        }

        double Gaussian2::fx(double x) {
            double gaussianLeft;
            double gaussianRight;

            if (x >= _left.getMean()) {

                gaussianLeft = 1;
            } else {

                gaussianLeft = _left.fx(x);
            }

            if (x <= _right.getMean()) {

                gaussianRight = 1;
            } else {

                gaussianRight = _right.fx(x);
            }

            return gaussianLeft * gaussianRight;
        }

        double Gaussian2::findMaximum() {
            return (_left.getMean() + _right.getMean()) / 2;
        }

        PiShape::PiShape(double a, double b, double c, double d) : _sShape(a, b), _zShape(c, d) {

        }

        double PiShape::findMaximum() {
            return (_sShape.getMaxPos() + _zShape.getMaxPos()) / 2;
        }

        Sigmoidal::Sigmoidal(double a, double c) : _a(a), _c(c) {

        }

        double Sigmoidal::findMaximum() {
            double pos = _c;
            double step = 0.1;

            double val = fx(_c);
            double valBefore = val - 1;

            while (std::abs(val - valBefore) > 0.01) {

                pos += step;
                valBefore = val;
                val = fx(pos);

                std::cout << "Pos:" << pos << "; Val:" << val << std::endl;
            }

            return pos;
        }
    }

    MembershipFunction::MembershipFunction() {

    }

    MembershipFunction::~MembershipFunction() {

    }

    MembershipFunction *FuzzySet::getMembershipFunction(size_t state) {
        return _mf[state];
    }

    std::vector<double> FuzzySet::getBeliefs(double x) const {
        size_t states = _mf.size();
        std::vector<double> beliefs(states);

        for (size_t i = 0; i < states; ++i) {

            beliefs[i] = _mf[i]->fx(x);

            if (beliefs[i] < _nullBeliefTolerance) {

                beliefs[i] = _nullBeliefTolerance;
            }
        }

        return beliefs;
    }

    double FuzzySet::getBelief(double x, size_t state) const {
        double belief = _mf[state]->fx(x);

        if (belief < _nullBeliefTolerance) {

            belief = _nullBeliefTolerance;
        }

        return belief;
    }

    double FuzzySet::findMaximum(size_t state) {
        return _mf[state]->findMaximum();
    }

    FuzzySet::FuzzySet(size_t states, double tol) : _nullBeliefTolerance(tol), _mf(states) {

    }

    void FuzzySet::setMembershipFunction(size_t state, MembershipFunction *mf) {
        _mf[state] = mf;
    }

    FuzzyRule::FuzzyRule(const std::vector<FuzzyRuleState *> &parentStates, FuzzyRuleState *state) : _state(*state) {
        _parentStates = parentStates;
    }

    std::vector<FuzzyRuleState *> &FuzzyRule::getParentStates() {
        return _parentStates;
    }

    FuzzyRuleState &FuzzyRule::getChildState() {
        return _state;
    }

    size_t FuzzyRule::nrJointStates() {
        size_t jointStates = 1;
        
        if (_state.isBinary()) {

            jointStates *= 2;
        } else {

            jointStates *= 4;
        }

        for (size_t i = 0; i < _parentStates.size(); ++i) {

            if (_parentStates[i]->isBinary()) {

                jointStates *= 2;
            } else {

                jointStates *= 4;
            }
        }

        return jointStates;
    }

    void FuzzyRuleSet::addRule(FuzzyRule *rule) {
        _rules.push_back(rule);
    }

    FuzzyRuleSet::FuzzyRuleSet() {

    }

    FuzzyRuleSet::FuzzyRuleSet(const std::vector<FuzzyRule *> &rules) {
        _rules = rules;
    }

    void FuzzyRuleSet::setRules(const std::vector<FuzzyRule *> &rules) {
        _rules = rules;
    }

    std::vector<FuzzyRule *> &FuzzyRuleSet::getRules() {
        return _rules;
    }

    size_t FuzzyRuleSet::nrJointStates() {
        return _rules.size() * _rules[0]->nrJointStates();
    }

    FuzzyController::FuzzyController(const std::vector<FuzzySet *> &set, FuzzyRuleSet *rules, double tolerance)
            : _rules(rules), _fuzzySet(set), _nullBeliefTolerance(tolerance) {

    }

    FuzzyController::~FuzzyController() {

    }

    CPT FuzzyController::inferCPT() {
        std::vector<FuzzyRule *> &fuzzyRules = _rules->getRules();
        size_t jointStates = fuzzyRules[0]->nrJointStates();
        CPT cpt(jointStates);

        std::vector<FuzzyRuleState *> &parentStates = fuzzyRules[0]->getParentStates();
        std::vector<size_t> maxStates(parentStates.size());

        for (size_t j = 0; j < parentStates.size(); ++j) {

            if (parentStates[j]->isBinary()) {

                maxStates[j] = 2;
            } else {

                maxStates[j] = 4;
            }
        }

        utils::Counter stateCounter(parentStates.size(), maxStates);

        do {

            std::vector<double> inferred = infer(stateCounter.getCount());
            size_t maxIncrement = stateCounter.getMaximumIncrement();

            for (size_t i = 0; i < inferred.size(); i++) {

                cpt[(i * maxIncrement) + stateCounter.getIncrement()] = inferred[i];
            }
        } while (stateCounter.countUp());

        return cpt;
    }

    std::vector<double> FuzzyController::infer(const std::vector<size_t> &states) {
        std::vector<double> max(states.size());

        // find maximum for given states from fuzzy sets
        for (size_t i = 0; i < states.size(); ++i) {

            max[i] = _fuzzySet[i]->findMaximum(states[i]);
        }

        std::vector<FuzzyRule *> &rules = _rules->getRules();
        std::vector<double> conclusions(rules.size());

        for (size_t i = 0; i < rules.size(); ++i) {

            double tNorm = 1;

            for (size_t j = 0; j < states.size(); ++j) {

                tNorm *= _fuzzySet[j]->getBelief(max[j], rules[i]->getParentStates()[j]->getState());
            }

            conclusions[i] = tNorm;
        }

        size_t nrStates;

        if (_rules->getRules()[0]->getChildState().isBinary()) {

            nrStates = 2;
        } else {

            nrStates = 4;
        }

        std::vector<double> inferredProbabilities(nrStates);

        for (size_t i = 0; i < conclusions.size(); ++i) {

            size_t state = rules[i]->getChildState().getState();

            if (conclusions[i] > inferredProbabilities[state]) {

                inferredProbabilities[state] = conclusions[i];
            }
        }

        for (size_t i = 0; i < inferredProbabilities.size(); ++i) {

            if (inferredProbabilities[i] < _nullBeliefTolerance) {

                inferredProbabilities[i] = _nullBeliefTolerance;
            }
        }

        // normalize
        utils::vectorNormalize(inferredProbabilities);

        return inferredProbabilities;
    }

    FuzzyRuleState::FuzzyRuleState(size_t state, bool binary) : _state(state), _binary(binary) {

    }

    FuzzyRuleState::~FuzzyRuleState() {

    }

    bool FuzzyRuleState::isBinary() {
        return _binary;
    }

    size_t FuzzyRuleState::getState() {
        return _state;
    }
}
