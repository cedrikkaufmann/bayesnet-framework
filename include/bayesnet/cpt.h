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
        explicit CPT(size_t sizeX, size_t sizeY) : _probabilities(sizeX * sizeY, 0) {}

        explicit CPT(size_t jointSize) : _probabilities(jointSize, 0) {}

        explicit CPT(const std::vector<double> &probabilities) : _probabilities(probabilities) {}

        explicit CPT(const Factor &factor) : _probabilities(factor.nrStates()) {}

        ~CPT() {}

        size_t size() const { return _probabilities.size(); }

        double get(size_t index) const { return _probabilities[index]; }

        std::vector<double> &getProbabilities() { return _probabilities; }

        std::vector<double> getProbabilities() const { return _probabilities; }

    private:
        std::vector<double> _probabilities;
    };
}

#endif //BAYESNET_FRAMEWORK_CPT_H
