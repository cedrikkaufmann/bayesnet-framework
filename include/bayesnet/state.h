//
// Created by Cedrik Kaufmann on 2019-05-27.
//

#ifndef BAYESNET_FRAMEWORK_STATE_H
#define BAYESNET_FRAMEWORK_STATE_H

#include <string>
#include <vector>
#include <iostream>

namespace bayesNet {

    namespace belief {

        /**
        * Enumeration for better readability of bayes node prediction states
        */
        enum BeliefState {
            GOOD = 0,
            PROBABLY_GOOD = 1,
            PROBABLY_BAD = 2,
            BAD = 3,
            FALSE = 4,
            TRUE = 5
        };

        class BayesBelief {
        public:
            explicit BayesBelief(bool binary = false);

            virtual ~BayesBelief();

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
}

#endif //BAYESNET_FRAMEWORK_STATE_H
