//
// Created by Cedrik Kaufmann on 2019-05-31.
//

#ifndef BAYESNET_FRAMEWORK_FACTOR_H
#define BAYESNET_FRAMEWORK_FACTOR_H

#include <dai/factor.h>
#include <dai/factorgraph.h>

namespace bayesNet {

    class Factor : public dai::Factor {
    public:
        explicit Factor(size_t states = 4);

        explicit Factor(dai::VarSet &vars, size_t states);

        virtual ~Factor();

        void setEvidence(size_t state);

        void clearEvidence();

    private:
        std::vector<double> _backupFactor;
        size_t _states;
        bool _isEvidence;

        void backup();

        void restore();
    };
}

#endif //BAYESNET_FRAMEWORK_FACTOR_H
