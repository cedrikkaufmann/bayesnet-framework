//
// Created by Cedrik Kaufmann on 2019-05-27.
//

#ifndef BAYESNET_FRAMEWORK_STATE_H
#define BAYESNET_FRAMEWORK_STATE_H

#define BAYESNET_STATES 2

#include <string>
#include <vector>
#include <iostream>

namespace BayesNet {
    /**
     * Enumeration for better readability of bayes node prediction states
     */
    enum BeliefState {
        BELIEF_STATE_GOOD = 0,
        BELIEF_STATE_PROBABLY_GOOD = 1,
        BELIEF_STATE_PROBABLY_BAD = 2,
        BELIEF_STATE_BAD = 3
    };

    class BayesBelief {
    public:
        BayesBelief() {};

        virtual ~BayesBelief() {};

        double &operator[](size_t index);

        void set(BeliefState state, double belief);

        double get(BeliefState state) const;

        std::string toString() const;

    private:
        double _beliefs[BAYESNET_STATES];
    };

    std::ostream &operator<<(std::ostream &os, const BayesBelief &bayesBelief);
}

#endif //BAYESNET_FRAMEWORK_STATE_H
