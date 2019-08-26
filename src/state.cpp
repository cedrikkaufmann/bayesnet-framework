//
// Created by Cedrik Kaufmann on 2019-06-13.
//

#include <sstream>

#include <bayesnet/state.h>
#include <bayesnet/exception.h>

namespace bayesNet {

    namespace belief {

        BayesBelief::BayesBelief(bool binary) : _binary(binary) {
            if (binary)
                _beliefs = std::vector<double>(2, 0);
            else
                _beliefs = std::vector<double>(4, 0);
        }

        BayesBelief::~BayesBelief() {

        }

        void BayesBelief::set(BeliefState state, double belief) {
            _beliefs[state] = belief;
        }


        double BayesBelief::get(BeliefState state) const {
            return _beliefs[state];
        }

        std::string BayesBelief::toString() const {
            std::stringstream ss;
            ss << *this;
            return ss.str();
        }

        double &BayesBelief::operator[](size_t index) {
            size_t upperBoundary;

            if (_binary)
                upperBoundary = 1;
            else
                upperBoundary = 3;

            if (index > upperBoundary) {
                throw IndexOutOfBoundException();
            }

            return _beliefs[index];
        }

        std::ostream &operator<<(std::ostream &os, const BayesBelief &bayesBelief) {
            size_t upperBoundary;
            size_t offset;

            if (bayesBelief.isBinary()) {

                offset = 4;
                upperBoundary = offset + 2; // 2 entries if binary
            } else {

                offset = 0;
                upperBoundary = offset + 4; // 4 entries if not binary
            }

            os << "{";

            for (size_t i = offset; i < upperBoundary; ++i) {

                switch (i) {

                    case GOOD: {
                        os << "GOOD: ";
                        break;
                    }

                    case PROBABLY_GOOD: {
                        os << "PROBABLY GOOD: ";
                        break;
                    }

                    case PROBABLY_BAD: {
                        os << "PROBABLY BAD: ";
                        break;
                    }

                    case BAD: {
                        os << "BAD: ";
                        break;
                    }

                    case TRUE: {
                        os << "TRUE: ";
                        break;
                    }

                    case FALSE:
                        os << "FALSE: ";
                        break;

                    default:
                        os << "UNKNOWN STATE: "; // should never happen
                }

                os << bayesBelief.get(static_cast<BeliefState>(i - offset));

                if (i < upperBoundary - 1)
                    os << "; ";
            }

            os << "}";

            return os;
        }
    }
}
