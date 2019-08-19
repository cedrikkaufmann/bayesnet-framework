//
// Created by Cedrik Kaufmann on 2019-08-08.
//

#ifndef BAYESNET_FRAMEWORK_UTIL_H
#define BAYESNET_FRAMEWORK_UTIL_H

#include <vector>
#include <string>
#include <random>

namespace bayesNet {

    namespace utils {

        std::vector<std::string> split(const std::string &s, char delimiter);

        void readDirectory(const std::string &name, std::vector<std::string> &v);

        double uniformRandom(double min, double max);
    }
}

#endif //BAYESNET_FRAMEWORK_UTIL_H
