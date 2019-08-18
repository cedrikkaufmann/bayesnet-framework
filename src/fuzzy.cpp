//
// Created by Cedrik Kaufmann on 2019-08-16.
//

#include <bayesnet/fuzzy.h>
#include <math.h>

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
        if (_binary) {

            if (_functions[0] != nullptr) {

                double val = _functions[0]->fx(y);
                _cpt.set(0, val);
                _cpt.set(1, 1 - val);
            } else {

                double val = _functions[1]->fx(y);
                _cpt.set(1, val);
                _cpt.set(0, 1 - val);
            }

            return _cpt;
        }

        for (size_t i = 0; i < 4; ++i) {

            _cpt.set(i, _functions[i]->fx(y));
        }

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
        if (_m > 0) {

            if (x < _fxMin) {

                return 0;
            }

            if (x > _fxMax) {

                return 1;
            }

        } else {

            if (x < _fxMin) {

                return 1;
            }

            if (x > _fxMax) {

                return 0;
            }
        }

        return (_m * (x - _b));
    }

    membershipFunctions::Linear::Linear(double fxMin, double fxMax) : _fxMin(fxMin), _fxMax(fxMax) {
        if (_fxMax > fxMin) {

            _m = 1 / (fxMax - fxMin);
            _b = fxMin;
        } else {

            _m = -1 / (fxMin - fxMax);
            _b = fxMin;
        }
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
        return 0;
    }
}
