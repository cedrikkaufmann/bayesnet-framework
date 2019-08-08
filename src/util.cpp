//
// Created by Cedrik Kaufmann on 2019-08-08.
//

#include <sstream>

#include <bayesnet/util.h>

namespace bayesNet {

    namespace utils {

        std::vector<std::string> split(const std::string &s, char delimiter) {
            std::vector<std::string> tokens;
            std::string token;
            std::istringstream tokenStream(s);

            while (std::getline(tokenStream, token, delimiter)) {

                tokens.push_back(token);
            }

            return tokens;
        }
    }
}
