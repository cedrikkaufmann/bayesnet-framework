/// @file
/// @brief Defines BayesBelief class which represent a belief calculated in a bayesian network and State, StateBinary which defines belief states for better readability.


#ifndef BAYESNET_FRAMEWORK_STATE_H
#define BAYESNET_FRAMEWORK_STATE_H


#include <string>
#include <vector>
#include <iostream>


namespace bayesNet {

    namespace state {

        /// Enumeration of discrete belief states used in bayesian networks 
        enum State {
            GOOD = 0,
            PROBABLY_GOOD = 1,
            PROBABLY_BAD = 2,
            BAD = 3
        };

        /// Enumeration of discrete binary belief states used in bayesian networks
        enum StateBinary {
            FALSE = 0,
            TRUE = 1
        };

        /// Returns the state value for a given string @a state
        size_t fromString(const std::string &state);

        /// Represents a bayesian belief
        /** BayesBelief is used to represent a nodes belief, which was calculated in a bayesian network
         *  using bayesian inference.
         */
        class BayesBelief {
        public:
            /// Constructs a bayesian belief 
            explicit BayesBelief(bool binary = false);

            /// Destructor
            virtual ~BayesBelief();

            /// Access operator
            double &operator[](size_t index);

            /// Returns if belief states are binary or not
            bool isBinary() const;

            /// Returns the number of states
            size_t nrStates() const;

            /// Sets the @a belief for @a state
            void set(size_t state, double belief);

            /// Returns the belief for @a state
            double get(size_t state) const;

            /// Returns string representation of the belief
            std::string toString() const;

        private:
            /// Stores the bliefs
            std::vector<double> _beliefs;

            /// Binary flag
            bool _binary;
        };

        /// stream operator used to write string representation of State @a sate to iostream @a os
        std::ostream &operator<<(std::ostream &os, const State &state);

        /// stream operator used to write string representation of Binary state @a sate to iostream @a os
        std::ostream &operator<<(std::ostream &os, const StateBinary &state);

        /// stream operator used to write string representation of bayes belief @a belief to iostream @a os
        std::ostream &operator<<(std::ostream &os, const BayesBelief &bayesBelief);
    }
}


#endif //BAYESNET_FRAMEWORK_STATE_H
