//
// Created by Cedrik Kaufmann on 2019-05-31.
//

#include <bayesnet/factor.h>
#include <bayesnet/state.h>

namespace BayesNet {

    void Factor::setEvidence(size_t state) {
        if (this->_isEvidence) {
            this->restore();
        }

        this->backup();

        size_t jointStates = this->nrStates();
        size_t evidenceEntries = jointStates / BAYESNET_STATES;
        size_t evidenceBeginIndex = evidenceEntries * state;
        size_t evidenceEndIndex = evidenceBeginIndex + evidenceEntries;

        for (size_t i = 0; i < jointStates; i++) {

            if (i < evidenceBeginIndex || i >= evidenceEndIndex) {
                this->set(i, 0);
            }
        }

        this->_isEvidence = true;
    }

    void Factor::clearEvidence() {
        this->restore();
        this->_isEvidence = false;
    }

    void Factor::backup() {
        for (int j = 0; j < this->nrStates(); ++j) {
            this->_backupFactor.at(j) = this->get(j);
        }
    }

    void Factor::restore() {
        for (int j = 0; j < this->nrStates(); ++j) {
            this->set(j, this->_backupFactor.at(j));
        }
    }

}