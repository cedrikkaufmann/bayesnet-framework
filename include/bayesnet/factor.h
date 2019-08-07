//
// Created by Cedrik Kaufmann on 2019-05-31.
//

#ifndef BAYESNET_FRAMEWORK_FACTOR_H
#define BAYESNET_FRAMEWORK_FACTOR_H

#include <dai/factor.h>
#include <dai/factorgraph.h>

namespace BayesNet {

    class Factor : public dai::Factor {
    public:
        explicit Factor(size_t states) : dai::Factor(), _backupFactor(states, 0), _states(states), _isEvidence(false) {}

        explicit Factor(dai::VarSet &vars, size_t states) : dai::Factor(vars), _backupFactor(vars.nrStates().get_ui()),
                                                            _states(states), _isEvidence(false) {}

        void setEvidence(size_t state);

        void clearEvidence();

    private:
        std::vector<double> _backupFactor;
        size_t _states;
        bool _isEvidence;

        void backup();

        void restore();
    };

    class GaussianFactor : public dai::Factor {
    public:
        explicit GaussianFactor(dai::VarSet &vars) : dai::Factor(vars) {}
    };
}

#endif //BAYESNET_FRAMEWORK_FACTOR_H
