//
// Created by Cedrik Kaufmann on 2019-08-16.
//

#include <math.h>

#include <bayesnet/fuzzy.h>
#include <bayesnet/util.h>

namespace bayesNet {

    FuzzyController::FuzzyController(bool binary) : _binary(binary) {
        if (binary) {

            _functions = std::vector<MembershipFunction *>(2);
            _cpt = CPT(2);
        } else {

            _functions = std::vector<MembershipFunction *>(4);
            _cpt = CPT(4);
        }
    }

    CPT &FuzzyController::getCPT(double y) {

        return _cpt;
    }

    void FuzzyController::setMF(belief::BeliefState state, MembershipFunction *func) {
        size_t offset = 0;

        if (_binary) {

            offset = 4;
        }

        _functions[state - offset] = func;
    }

    double membershipFunctions::Linear::fx(double x) {
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

    membershipFunctions::Linear::Linear(double fxMin, double fxMax) : _fxMin(fxMin), _fxMax(fxMax) {
        _m = 1 / (fxMax - fxMin);
        _b = fxMin;
    }

    double membershipFunctions::Triangle::fx(double x) {
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

    double membershipFunctions::Trapezoid::fx(double x) {
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

    double membershipFunctions::Trapezoid::findMaximum() {
        return utils::uniformRandom(_increasingEnd, _decreasingBegin);
    }

    double membershipFunctions::SShape::fx(double x) {
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

    double membershipFunctions::ZShape::fx(double x) {
        return 1 - _sShape.fx(x);
    }

    double membershipFunctions::PiShape::fx(double x) {
        if (x <= _zShape.getA()) {

            return _sShape.fx(x);
        }

        return _zShape.fx(x);
    }

    double membershipFunctions::Sigmoidal::fx(double x) {
        return 1 / (1 + std::exp(-1 * _a * (x - _c)));
    }

    double membershipFunctions::Bell::fx(double x) {
        return 1 / (1 + std::pow(std::abs((x - _c) / _a), 2 * _b));
    }

    double membershipFunctions::Gaussian::fx(double x) {
        return std::exp(-std::pow(x - _mean, 2) / (2 * std::pow(_deviation, 2)));
    }

    double membershipFunctions::Gaussian2::fx(double x) {
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

    double membershipFunctions::Gaussian2::findMaximum() {
        return utils::uniformRandom(_left.getMean(), _right.getMean());
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

    double MembershipFunction::findMaximum() {
        // TODO general maximum algorithm
    }
}
