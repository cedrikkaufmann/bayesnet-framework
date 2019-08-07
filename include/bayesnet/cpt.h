//
// Created by Cedrik Kaufmann on 2019-07-02.
//

#ifndef BAYESNET_FRAMEWORK_CPT_H
#define BAYESNET_FRAMEWORK_CPT_H

#include <vector>

#include <bayesnet/factor.h>

namespace BayesNet {

    class CPT {
    public:
        CPT() {}

        explicit CPT(size_t jointSize) : _probabilities(jointSize, 0) {}

        explicit CPT(std::vector<double> probabilities);

        explicit CPT(const Factor &factor) : _probabilities(factor.nrStates()) {}

        size_t size() const { return _probabilities.size(); }

        void set(size_t index, double value) { _probabilities[index] = value; }

        double get(size_t index) const { return _probabilities[index]; }

        std::vector<double> &getProbabilities() { return _probabilities; }

        std::vector<double> getProbabilities() const { return _probabilities; }

    private:
        std::vector<double> _probabilities;
    };
}

#endif //BAYESNET_FRAMEWORK_CPT_H
