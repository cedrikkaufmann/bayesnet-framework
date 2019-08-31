//
// Created by Cedrik Kaufmann on 2019-07-02.
//

#include <bayesnet/cpt.h>
#include <bayesnet/exception.h>

namespace bayesNet {

    CPT::CPT() {}

    CPT::CPT(size_t jointSize) : _probabilities(jointSize) {}

    CPT::CPT(const Factor &factor) : _probabilities(factor.nrStates()) {}

    CPT::~CPT() {}

    CPT::CPT(std::vector<double> probabilities) {
        // try to calculate complementary probability if missing
        size_t states = probabilities.size() / 2;

        for (size_t i = 0; i < states; ++i) {

            if (probabilities[i] + probabilities[i + states] < 1) {

                if (probabilities[i] > 0 && probabilities[i + states] > 0) {
                    // cannot calculate complement, thus throw exception
                    throw InvalidCPTException();
                }

                // calculate complementary probability
                if (probabilities[i] > 0) {
                    probabilities[i + states] = 1 - probabilities[i];
                } else {
                    probabilities[i] = 1 - probabilities[i + states];
                }

            } else if (probabilities[i] + probabilities[i + states] > 1) {
                // probability and its complement are greater than 1, thus throw exception
                throw InvalidCPTException();
            }
        }

        // set completed cpt
        _probabilities = probabilities;
    }

    size_t CPT::size() const {
        return _probabilities.size();
    }

    void CPT::set(size_t index, double value) {
        _probabilities[index] = value;
    }

    double CPT::get(size_t index) const {
        return _probabilities[index];
    }

    std::vector<double> &CPT::getProbabilities() {
        return _probabilities;
    }

    std::vector<double> CPT::getProbabilities() const {
        return _probabilities;
    }

    double &CPT::operator[](size_t index) {
        if (index > _probabilities.size()) {
            throw IndexOutOfBoundException();
        }

        return _probabilities[index];
    }
}
