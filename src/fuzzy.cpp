//
// Created by Cedrik Kaufmann on 2019-08-16.
//

#include <bayesnet/fuzzy.h>
#include <cmath>

namespace bayesNet {

    FuzzyController::FuzzyController(bool binary) : _binary(binary) {
        if (binary) {

            _functions = std::vector<FuzzyFunction *>(2);
            _cpt = CPT(2);
        } else {

            _functions = std::vector<FuzzyFunction *>(4);
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

    void FuzzyController::setFunction(belief::BeliefState state, FuzzyFunction *fun) {
        size_t offset = 0;

        if (_binary) {

            offset = 4;
        }

        _functions[state - offset] = fun;
    }

    double fuzzyFunctions::Linear::fx(double x) {
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

    fuzzyFunctions::Linear::Linear(double fxMin, double fxMax) : _fxMin(fxMin), _fxMax(fxMax) {
        if (_fxMax > fxMin) {

            _m = 1 / (fxMax - fxMin);
            _b = fxMin;
        } else {

            _m = -1 / (fxMin - fxMax);
            _b = fxMin;
        }
    }

    double fuzzyFunctions::Gaussian::fx(double x) {
        return 0.5 * (1 + std::erf((x - _mean) / (std::sqrt(2 * std::pow(_deviation, 2)))));
    }
}
