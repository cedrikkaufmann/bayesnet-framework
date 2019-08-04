//
// Created by Cedrik Kaufmann on 2019-08-03.
//

#ifndef BAYESNET_FRAMEWORK_JSON_H
#define BAYESNET_FRAMEWORK_JSON_H

#include <string>
#include <vector>

#include <bayesnet/network.h>

namespace BayesNet {

    void parseJson(const std::string &filename, Network &network);
    void saveJson(const std::string &filename, const Network &network);
    bool isWhitespaceOrQuotationMark(char c);
    std::vector<std::string> split(const std::string& s, char delimiter);
}

#endif //BAYESNET_FRAMEWORK_JSON_H
