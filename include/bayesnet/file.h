//
// Created by Cedrik Kaufmann on 2019-08-03.
//

#ifndef BAYESNET_FRAMEWORK_FILE_H
#define BAYESNET_FRAMEWORK_FILE_H

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>

namespace bayesNet {

    namespace file {

        struct InitializationVector {
            std::vector<std::string> binaryNodes;
            std::vector<std::string> nodes;
            std::unordered_map<std::string, std::vector<std::string> > connections;
            std::unordered_map<std::string, std::vector<double> > cpt;
            std::string inferenceAlgorithm;
        };

        InitializationVector *parse(const std::string &filename);

        void save(const std::string &filename, InitializationVector *iv);

        bool isWhitespaceOrQuotationMark(char c);

        std::ostream &operator<<(std::ostream &os, const InitializationVector &iv);
    }
}

#endif //BAYESNET_FRAMEWORK_FILE_H
