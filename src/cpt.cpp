#include <sstream>

#include <bayesnet/cpt.h>
#include <bayesnet/exception.h>
#include <bayesnet/util.h>


namespace bayesNet {

    CPT::CPT() {}

    CPT::CPT(size_t jointSize) : _probabilities(jointSize), _changed(false) {}

    CPT::CPT(const Factor &factor) : _probabilities(factor.nrStates()), _changed(false) {}

    CPT::~CPT() {}

    CPT::CPT(std::vector<double> probabilities) : _probabilities(probabilities), _changed(false) {
        utils::vectorNormalize(_probabilities);
    }

    size_t CPT::size() const {
        return _probabilities.size();
    }

    void CPT::set(size_t index, double value) {
        _probabilities[index] = value;
        _changed = true;
    }

    double CPT::get(size_t index) {
        if (_changed) {
            utils::vectorNormalize(_probabilities);
            _changed = false;
        }

        return _probabilities[index];
    }

    std::vector<double> &CPT::getProbabilities() {
        if (_changed) {
            utils::vectorNormalize(_probabilities);
            _changed = false;
        }

        return _probabilities;
    }
}
