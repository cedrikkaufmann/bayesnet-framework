//
// Created by Cedrik Kaufmann on 2019-08-03.
//

#ifndef BAYESNET_FRAMEWORK_JSON_H
#define BAYESNET_FRAMEWORK_JSON_H

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>

namespace BayesNet {

    struct InitializationVector {
        std::vector<std::string> binaryNodes;
        std::vector<std::string> nodes;
        std::unordered_map<std::string, std::vector<std::string> > connections;
        std::unordered_map<std::string, std::vector<double> > cpt;
    };

    InitializationVector *parseJson(const std::string &filename);

    void saveJson(const std::string &filename, InitializationVector *iv);

    bool isWhitespaceOrQuotationMark(char c);

    std::vector<std::string> split(const std::string &s, char delimiter);

    std::ostream &operator<<(std::ostream &os, const InitializationVector &iv);
}

#endif //BAYESNET_FRAMEWORK_JSON_H
