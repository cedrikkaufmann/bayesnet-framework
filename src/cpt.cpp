#include <bayesnet/cpt.h>
#include <bayesnet/exception.h>


namespace bayesNet {

    CPT::CPT() {}

    CPT::CPT(size_t jointSize) : _probabilities(jointSize) {}

    CPT::CPT(const Factor &factor) : _probabilities(factor.nrStates()) {}

    CPT::~CPT() {}

    CPT::CPT(std::vector<double> probabilities) : _probabilities(probabilities) {}

    size_t CPT::size() const {
        return _probabilities.size();
    }

    void CPT::set(size_t index, double value) {
        // check if index is in bounds
        if (index > _probabilities.size()) {
            BAYESNET_THROW(INDEX_OUT_OF_BOUNDS);
        }
        
        _probabilities[index] = value;
    }

    double CPT::get(size_t index) const {
        // check if index is in bounds
        if (index > _probabilities.size()) {
            BAYESNET_THROW(INDEX_OUT_OF_BOUNDS);
        }

        return _probabilities[index];
    }

    std::vector<double> &CPT::getProbabilities() {
        return _probabilities;
    }

    std::vector<double> CPT::getProbabilities() const {
        return _probabilities;
    }

    double &CPT::operator[](size_t index) {
        // check if index is in bounds
        if (index > _probabilities.size()) {
            BAYESNET_THROW(INDEX_OUT_OF_BOUNDS);
        }

        return _probabilities[index];
    }
}
