#include <bayesnet/factor.h>
#include <bayesnet/exception.h>


namespace bayesNet {

    Factor::Factor(size_t states) : dai::Factor(), _backupFactor(states, 0), _states(states), _isEvidence(false), _evidenceState(0) {}

    Factor::Factor(dai::VarSet &vars, size_t states) : dai::Factor(vars), _backupFactor(vars.nrStates().get_ui()),
                                                       _states(states), _isEvidence(false), _evidenceState(0) {}

    Factor::~Factor() {}

    void Factor::setEvidence(size_t state) {
        // check if state is available
        if (state > _states - 1) {
            BAYESNET_THROW(INDEX_OUT_OF_BOUNDS);
        }

        // check if factor is evidence
        if (_isEvidence) {
            restore();
        }

        // backup factor
        backup();

        // calculate factor states which has to be set to probability 1
        size_t jointStates = nrStates();
        size_t evidenceEntries = jointStates / _states;
        size_t evidenceBeginIndex = evidenceEntries * state;
        size_t evidenceEndIndex = evidenceBeginIndex + evidenceEntries;

        for (size_t i = 0; i < jointStates; i++) {
            if (i < evidenceBeginIndex || i >= evidenceEndIndex) {
                this->set(i, 0);
            }
        }

        // set evidence flag
        _isEvidence = true;
        _evidenceState = state;
    }

    void Factor::clearEvidence() {
        restore();
        _isEvidence = false;
    }

    void Factor::backup() {
        // backup factor probabilities
        for (size_t i = 0; i < nrStates(); ++i) {
            _backupFactor[i] = get(i);
        }
    }

    void Factor::restore() {
        // restore factor probabilities
        for (size_t i = 0; i < nrStates(); ++i) {
            set(i, _backupFactor[i]);
        }
    }

    size_t Factor::evidenceState() const {
        if (!_isEvidence) {
            BAYESNET_THROW(NO_EVIDENCE_ON_FACTOR);
        }

        return _evidenceState;
    }

    bool Factor::isEvidence() const {
        return _isEvidence;
    }


}
