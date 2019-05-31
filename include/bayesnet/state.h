//
// Created by Cedrik Kaufmann on 2019-05-27.
//

#ifndef BAYESNET_FRAMEWORK_STATE_H
#define BAYESNET_FRAMEWORK_STATE_H

namespace BayesNet{
    /**
     * Enumeration for better readability of bayes node prediction states
     */
    enum PredictionState {
        PredictionGood = 0, PredictionProbablyGood = 1, PredictionProbablyBad = 2, PredictionBad = 3
    };
}

#endif //BAYESNET_FRAMEWORK_STATE_H
