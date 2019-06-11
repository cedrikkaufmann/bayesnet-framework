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
        Factor() : dai::Factor(), _isEvidence(false) {}

        explicit Factor(dai::VarSet &vars) : dai::Factor(vars), _isEvidence(false),
                                             _backupFactor(vars.nrStates().get_ui()) {}

        virtual ~Factor() = default;

        void setEvidence(size_t state);

        void clearEvidence();

    private:
        bool _isEvidence;
        std::vector<double> _backupFactor;

        void backup();

        void restore();
    };

    class GaussianFactor : public dai::Factor {
    public:
        explicit GaussianFactor(dai::VarSet &vars) : dai::Factor(vars) {};
    };
}

#endif //BAYESNET_FRAMEWORK_FACTOR_H
