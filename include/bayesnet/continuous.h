//
// Created by Cedrik Kaufmann on 2019-08-17.
//

#ifndef BAYESNET_FRAMEWORK_CONTINUOUS_H
#define BAYESNET_FRAMEWORK_CONTINUOUS_H

#include <bayesnet/cpt.h>

namespace bayesNet {

    class ContinuousVariable {
    public:
        virtual ~ContinuousVariable() {}

        virtual CPT &getCPT(double x) = 0;
    };
}

#endif //BAYESNET_FRAMEWORK_CONTINOUS_H
