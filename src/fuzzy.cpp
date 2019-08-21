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
            return utils::uniformRandom(_increasingEnd, _decreasingBegin);
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

        double SShape::getA() {
            return _a;
        }

        double SShape::getB() {
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

        double ZShape::getA() {
            return _sShape.getB();
        }

        double ZShape::getB() {
            return _sShape.getA();
        }

        double ZShape::findMaximum() {
            return _sShape.getA();
        }

        double PiShape::fx(double x) {
            if (x <= _zShape.getA()) {

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
            return (_sShape.getB() + _zShape.getA()) / 2;
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

    MembershipFunction *FuzzySet::getMf(size_t state) {
        return _mf[state];
    }

    std::vector<double> FuzzySet::getBeliefs(double x) {
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

    double FuzzySet::findMaximum(size_t state) {
        return _mf[state]->findMaximum();
    }

    FuzzySet::FuzzySet(size_t states) : _nullBeliefTolerance(0), _mf(states) {

    }

    FuzzySet::FuzzySet(size_t states, double tol) : _nullBeliefTolerance(tol), _mf(states) {

    }

    void FuzzySet::setMf(size_t state, MembershipFunction *mf) {
        _mf[state] = mf;
    }

    void FuzzyRule::addParentState(size_t state) {
        _parentStates.push_back(state);
    }

    size_t FuzzyRule::nrJointStates() {
        return 0;
    }

    FuzzyRule::FuzzyRule() : _state() {

    }

    FuzzyRule::FuzzyRule(const std::vector<size_t> &parentStates, size_t state) : _state(state) {
        _parentStates = parentStates;
    }

    void FuzzyRule::setParentStates(const std::vector<size_t> &parentStates) {
        _parentStates = parentStates;
    }

    void FuzzyRule::setState(size_t state) {
        _state = state;
    }

    std::vector<size_t> &FuzzyRule::getParentStates() {
        return _parentStates;
    }

    size_t FuzzyRule::getState() {
        return _state;
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

    FuzzyController::FuzzyController() : _rules(NULL) {

    }

    void FuzzyController::addFuzzySet(FuzzySet *set) {
        _fuzzySet.push_back(set);
    }

    CPT &FuzzyController::generateCPT(double x) {
        CPT cpt;

        return cpt;
    }

    void FuzzyController::setFuzzyRuleSet(FuzzyRuleSet *rules) {

    }
}
