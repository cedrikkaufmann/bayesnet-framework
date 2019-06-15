//
// Created by Cedrik Kaufmann on 2019-06-13.
//

#include <bayesnet/state.h>
#include <sstream>

namespace BayesNet {

    void BayesBelief::set(BeliefState state, double belief) {
        this->_beliefs[state] = belief;
    }


    double BayesBelief::get(BeliefState state) const {
        return this->_beliefs[state];
    }

    std::string BayesBelief::toString() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    double &BayesBelief::operator[](size_t index) {
        if (index > BAYESNET_STATES - 1) {
            throw 0;
        }

        return this->_beliefs[index];
    }

    std::ostream &operator<<(std::ostream &os, const BayesBelief &bayesBelief) {
        os << "{";

        for (int i = 0; i < BAYESNET_STATES; ++i) {
            switch (i) {
                case BELIEF_STATE_GOOD: {
                    os << "GOOD: ";
                    break;
                }

                case BELIEF_STATE_PROBABLY_GOOD: {
                    os << "PROBABLY GOOD: ";
                    break;
                }

                case BELIEF_STATE_PROBABLY_BAD: {
                    os << "PROBABLY BAD: ";
                    break;
                }

                case BELIEF_STATE_BAD: {
                    os << "BAD: ";
                    break;
                }

                default:
                    os << "UNKNOWN STATE: "; // should never happen
            }

            os << bayesBelief.get(static_cast<BeliefState>(i)) << "; ";
        }

        os << "}";

        return os;
    }
}
