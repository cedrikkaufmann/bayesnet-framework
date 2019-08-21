//
// Created by Cedrik Kaufmann on 2019-07-02.
//

#ifndef BAYESNET_FRAMEWORK_CPT_H
#define BAYESNET_FRAMEWORK_CPT_H

#include <vector>

#include <bayesnet/factor.h>

namespace bayesNet {

    class CPT {
    public:
        CPT();

        virtual ~CPT();

        explicit CPT(size_t jointSize);

        explicit CPT(std::vector<double> probabilities);

        explicit CPT(const Factor &factor);

        size_t size() const;

        void set(size_t index, double value);

        double get(size_t index) const;

        std::vector<double> &getProbabilities();

        std::vector<double> getProbabilities() const;

    private:
        std::vector<double> _probabilities;
    };
}

#endif //BAYESNET_FRAMEWORK_CPT_H
