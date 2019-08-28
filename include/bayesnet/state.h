//
// Created by Cedrik Kaufmann on 2019-05-27.
//

#ifndef BAYESNET_FRAMEWORK_STATE_H
#define BAYESNET_FRAMEWORK_STATE_H

#include <string>
#include <vector>
#include <iostream>

namespace bayesNet {

    namespace state {

        enum State {
            GOOD = 0,
            PROBABLY_GOOD = 1,
            PROBABLY_BAD = 2,
            BAD = 3
        };

        enum StateBinary {
            FALSE = 0,
            TRUE = 1
        };

        class BayesBelief {
        public:
            explicit BayesBelief(bool binary = false);

            virtual ~BayesBelief();

            double &operator[](size_t index);

            bool isBinary() const;

            void set(size_t state, double belief);

            double get(size_t state) const;

            std::string toString() const;

        private:
            std::vector<double> _beliefs;
            bool _binary;
        };

        std::ostream &operator<<(std::ostream &os, const BayesBelief &bayesBelief);
    }
}

#endif //BAYESNET_FRAMEWORK_STATE_H
