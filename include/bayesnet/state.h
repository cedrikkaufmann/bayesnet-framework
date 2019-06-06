//
// Created by Cedrik Kaufmann on 2019-05-27.
//

#ifndef BAYESNET_FRAMEWORK_STATE_H
#define BAYESNET_FRAMEWORK_STATE_H

#define BAYESNET_STATES 4

namespace BayesNet {
    /**
     * Enumeration for better readability of bayes node prediction states
     */
    enum BeliefState {
        BeliefGood = 0, BeliefProbablyGood = 1, BeliefProbablyBad = 2, BeliefBad = 3
    };
}

#endif //BAYESNET_FRAMEWORK_STATE_H
