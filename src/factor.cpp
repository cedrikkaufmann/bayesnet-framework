//
// Created by Cedrik Kaufmann on 2019-05-31.
//

#include <bayesnet/factor.h>
#include <bayesnet/state.h>

namespace BayesNet {

    void Factor::setEvidence(size_t state) {
        if (_isEvidence) {
            restore();
        }

        backup();

        size_t jointStates = this->nrStates();
        size_t evidenceEntries = jointStates / BAYESNET_STATES;
        size_t evidenceBeginIndex = evidenceEntries * state;
        size_t evidenceEndIndex = evidenceBeginIndex + evidenceEntries;

        for (size_t i = 0; i < jointStates; i++) {

            if (i < evidenceBeginIndex || i >= evidenceEndIndex) {
                this->set(i, 0);
            }
        }

        _isEvidence = true;
    }

    void Factor::clearEvidence() {
        restore();
        _isEvidence = false;
    }

    void Factor::backup() {
        for (size_t i = 0; i < this->nrStates(); ++i) {
            _backupFactor[i] = get(i);
        }
    }

    void Factor::restore() {
        for (size_t i = 0; i < this->nrStates(); ++i) {
            set(i, _backupFactor[i]);
        }
    }

}
