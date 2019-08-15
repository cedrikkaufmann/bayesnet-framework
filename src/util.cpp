//
// Created by Cedrik Kaufmann on 2019-08-08.
//

#include <sstream>

#include <bayesnet/util.h>
#include <dirent.h>

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

        void readDirectory(const std::string &name, std::vector<std::string> &v) {
            DIR *dir = opendir(name.c_str());
            struct dirent *dp;

            while ((dp = readdir(dir)) != nullptr) {

                v.push_back(dp->d_name);
            }

            closedir(dir);

            // drop items "." and ".." from list
            v.erase(v.begin(), v.begin() + 2);
        }
    }
}
