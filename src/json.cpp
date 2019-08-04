//
// Created by Cedrik Kaufmann on 2019-08-03.
//

#include <iostream>
#include <fstream>
#include <regex>

#include <bayesnet/json.h>

namespace BayesNet {

    // TODO: json parser
    void parseJson(const std::string &filename, Network &network) {
        // regular expressions to parse json file
        std::regex beginRegEx("^\\s*\\{\\s*$");
        std::regex endRegEx("^\\s*}\\s*,?\\s*");
        std::regex sectionRegEx("^\\s*\"(nodes|connections|cpt)\"\\s*:\\s*\\{\\s*$");
        std::regex nodesRegEx("^\\s*\"([a-zA-Z0-9]+)\"\\s*:\\s*(2|4)\\s*,?\\s*$");
        std::regex connectionsRegEx("^\\s*\"([a-zA-Z0-9]+)\":\\s*\\[((\\s*\"([a-zA-Z0-9]+)\"\\s*,?)*)\\],?$");
        std::regex cptRegEx("^\\s*\"([a-zA-Z0-9]+)\"\\s*:\\s*\\[((\\s*([0-9]+\\.?)\\s*,?)*)\\],?$");
        std::regex inferenceRegEx("^\\s*\"inference\"\\s*:\\s*\"([A-Z_]+)\"\\s*$");

        // open file
        std::ifstream file(filename);
        std::string line;

        // parsing file
        if (file.is_open()) {

            bool beginFile = false;
            bool sectionNodes = false;
            bool sectionConnections = false;
            bool sectionCPT = false;

            std::smatch match;

            while (getline(file, line)) {

                // check for begin of file
                if (!beginFile && std::regex_match(line, beginRegEx)) {

                    beginFile = true;
                    continue;
                }

                // check for node name and count of states
                if (sectionNodes && std::regex_match(line, match, nodesRegEx)) {

                    if (match.str(2) == "4") {

                        std::cout << "Add node " << match.str(1) << std::endl;
                        // add node with 4 states
                        network.newNode(match.str(1));
                    } else {

                        std::cout << "Add binary node " << match.str(1) << std::endl;
                        // add binary node
                        network.newBinaryNode(match.str(1));
                    }

                    continue;
                }

                if (sectionConnections && std::regex_match(line, match, connectionsRegEx)) {

                    // get connection list
                    std::string connections = match.str(2);

                    // further string processing
                    // remove whitespaces and quotation marks
                    connections.erase(std::remove_if(connections.begin(), connections.end(), isWhitespaceOrQuotationMark), connections.end());
                    // split connection list
                    std::vector<std::string> splitConnections = split(connections, ',');

                    // add connections to network
                    for (size_t i = 0; i < splitConnections.size(); ++i) {

                        std::cout << "Connecting " << match.str(1) << " and " << splitConnections[i] << std::endl;
                        network.newConnection(match.str(1), splitConnections[i]);
                    }

                    continue;
                }

                if (sectionCPT  && std::regex_match(line, match, cptRegEx)) {

                    // get cpt list
                    std::string cpt = match.str(2);

                    // remove whitespaces
                    cpt.erase(std::remove(cpt.begin(), cpt.end(), ' '), cpt.end());
                    // split connection list
                    std::vector<std::string> splitCPT = split(cpt, ',');

                    // convert string to double
                    std::vector<double> doubleCPT(splitCPT.size());

                    for (size_t i = 0; i < splitCPT.size(); ++i) {

                        std::cout << "Converting for " << match.str(1) << " and " << splitCPT[i] << std::endl;
                        doubleCPT[i] = std::stod(splitCPT[i]);
                    }

                    // add cpt to network
                    network.setCPT(match.str(1), CPT(doubleCPT));

                    continue;
                }

                // check for end of section/file
                if (std::regex_match(line, match, endRegEx)) {

                    if (sectionNodes || sectionConnections || sectionCPT) {

                        sectionConnections = false;
                        sectionNodes = false;
                        sectionCPT = false;

                        continue;
                    } else {

                        // end of json file, stop parsing
                        break;
                    }
                }

                // check for section
                if (std::regex_match(line, match, sectionRegEx)) {

                    if (match.str(1) == "nodes") {
                        sectionNodes = true;
                    } else if (match.str(1) == "connections") {
                        sectionConnections = true;
                    } else if (match.str(1) == "cpt") {
                        sectionCPT = true;
                    }

                    continue;
                }

                // check for inference option
                if (std::regex_match(line, match, inferenceRegEx)) {
                    // TODO: inference option

                    continue;
                }
            }

            // close file descriptor
            file.close();
        } else {

            // failed to open file
            std::cout << "Unable to open file: \"" << filename << "\"" << std::endl;
        }
    }

    bool isWhitespaceOrQuotationMark(char c) {
        return c == ' ' || c == '"';
    }

    std::vector<std::string> split(const std::string& s, char delimiter) {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(s);

        while (std::getline(tokenStream, token, delimiter)) {

            tokens.push_back(token);
        }

        return tokens;
    }
}
