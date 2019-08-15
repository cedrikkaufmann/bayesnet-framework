//
// Created by Cedrik Kaufmann on 2019-08-08.
//

#ifndef BAYESNET_FRAMEWORK_UTIL_H
#define BAYESNET_FRAMEWORK_UTIL_H

#include <vector>
#include <string>

namespace bayesNet {

    namespace utils {

        std::vector<std::string> split(const std::string &s, char delimiter);
        void readDirectory(const std::string& name, std::vector<std::string>& v);
    }
}

#endif //BAYESNET_FRAMEWORK_UTIL_H
