//
// Created by Cedrik Kaufmann on 2019-05-27.
//

#ifndef BAYESNET_FRAMEWORK_STATE_H
#define BAYESNET_FRAMEWORK_STATE_H

#include <string>
#include <vector>
#include <iostream>

#define BAYESNET_STATES 2

namespace bayesNet {

    /**
     * Enumeration for better readability of bayes node prediction states
     */
    enum BeliefState {
        BELIEF_STATE_GOOD = 0,
        BELIEF_STATE_PROBABLY_GOOD = 1,
        BELIEF_STATE_PROBABLY_BAD = 2,
        BELIEF_STATE_BAD = 3,
        BELIEF_STATE_FALSE = 4,
        BELIEF_STATE_TRUE = 5
    };

    class BayesBelief {
    public:
        BayesBelief() : _beliefs(4, 0), _binary(false) {}

        explicit BayesBelief(bool binary);

        double &operator[](size_t index);

        bool isBinary() const { return _binary; }

        void set(BeliefState state, double belief);

        double get(BeliefState state) const;

        std::string toString() const;

    private:
        std::vector<double> _beliefs;
        bool _binary;
    };

    std::ostream &operator<<(std::ostream &os, const BayesBelief &bayesBelief);
}

#endif //BAYESNET_FRAMEWORK_STATE_H
