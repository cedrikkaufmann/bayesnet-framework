//
// Created by Cedrik Kaufmann on 2019-06-13.
//

#include <sstream>

#include <bayesnet/state.h>
#include <bayesnet/exception.h>

namespace bayesNet {

    namespace state {

        BayesBelief::BayesBelief(bool binary) : _binary(binary) {
            if (binary) {

                _beliefs = std::vector<double>(2, 0);
            } else {

                _beliefs = std::vector<double>(4, 0);
            }
        }

        BayesBelief::~BayesBelief() {

        }

        bool BayesBelief::isBinary() const {
            return _binary;
        }

        void BayesBelief::set(size_t state, double belief) {
            if (state >= _beliefs.size()) {

                throw IndexOutOfBoundException();
            }

            _beliefs[state] = belief;
        }

        double BayesBelief::get(size_t state) const {
            if (state >= _beliefs.size()) {

                throw IndexOutOfBoundException();
            }

            return _beliefs[state];
        }

        std::string BayesBelief::toString() const {
            std::stringstream ss;
            ss << *this;
            return ss.str();
        }

        double &BayesBelief::operator[](size_t index) {
            if (index >= _beliefs.size()) {

                throw IndexOutOfBoundException();
            }

            return _beliefs[index];
        }

        std::ostream &operator<<(std::ostream &os, const BayesBelief &bayesBelief) {
            os << "{";

            if (bayesBelief.isBinary()) {

                for (size_t i = 0; i < 2; ++i) {

                    switch (i) {

                        case TRUE: {

                            os << "TRUE: ";
                            break;
                        }

                        case FALSE: {

                            os << "FALSE: ";
                            break;
                        }
                            
                    }

                    os << bayesBelief.get(i);

                    if (i < 1) {
                        
                        os << "; ";
                    }
                }
            }  else {

                for (size_t i = 0; i < 2; ++i) {

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
                    }
                }
            }    

            os << "}";

            return os;
        }
    }
}
