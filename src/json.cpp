//
// Created by Cedrik Kaufmann on 2019-08-03.
//

#include <fstream>
#include <regex>

#include <bayesnet/json.h>
#include <bayesnet/util.h>
#include <bayesnet/exception.h>

namespace bayesNet {

    namespace json {

        InitializationVector *parse(const std::string &filename) {
            // regular expressions to parse json file
            std::regex beginRegEx("^\\s*\\{\\s*$");
            std::regex endRegEx("^\\s*}\\s*,?\\s*$");
            std::regex sectionRegEx("^\\s*\"(nodes|connections|cpt)\"\\s*:\\s*\\{\\s*$");
            std::regex nodesRegEx("^\\s*\"([a-zA-Z0-9_]+)\"\\s*:\\s*(2|4)\\s*,?\\s*$");
            std::regex connectionsRegEx("^\\s*\"([a-zA-Z0-9_]+)\":\\s*\\[((\\s*\"([a-zA-Z0-9_]+)\"\\s*,?)*)\\],?$");
            std::regex cptRegEx("^\\s*\"([a-zA-Z0-9_]+)\"\\s*:\\s*\\[((\\s*([0-9]+\\.?)\\s*,?)*)\\],?$");
            std::regex inferenceRegEx("^\\s*\"inference\"\\s*:\\s*\"([a-zA-Z0-9_.\\/]*)\"\\s*$");

            // open file
            std::ifstream file(filename);
            std::string line;

            // parsing file
            if (file.is_open()) {
                InitializationVector *iv = new InitializationVector();

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

                        // add node to iv
                        if (match.str(2) == "2") {

                            iv->binaryNodes.push_back(match.str(1));
                        } else {

                            iv->nodes.push_back(match.str(1));
                        }

                        continue;
                    }

                    if (sectionConnections && std::regex_match(line, match, connectionsRegEx)) {

                        // get connection list
                        std::string connections = match.str(2);

                        // further string processing
                        // remove whitespaces and quotation marks
                        connections.erase(
                                std::remove_if(connections.begin(), connections.end(), isWhitespaceOrQuotationMark),
                                connections.end());

                        // split connection list and at to iv
                        iv->connections[match.str(1)] = utils::split(connections, ',');

                        continue;
                    }

                    if (sectionCPT && std::regex_match(line, match, cptRegEx)) {

                        // get cpt list
                        std::string cpt = match.str(2);

                        // remove whitespaces
                        cpt.erase(std::remove(cpt.begin(), cpt.end(), ' '), cpt.end());
                        // split connection list
                        std::vector<std::string> splitCPT = utils::split(cpt, ',');

                        // convert string to double and add to iv
                        iv->cpt[match.str(1)] = std::vector<double>(splitCPT.size());

                        for (size_t i = 0; i < splitCPT.size(); ++i) {

                            iv->cpt[match.str(1)][i] = std::stod(splitCPT[i]);
                        }

                        continue;
                    }

                    // check for end of section/file
                    if (std::regex_match(line, match, endRegEx)) {

                        // end of section nodes/connections/cpt
                        if (sectionNodes || sectionConnections || sectionCPT) {

                            sectionConnections = false;
                            sectionNodes = false;
                            sectionCPT = false;

                            continue;
                        }

                        // end of json file, stop parsing
                        break;
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

                        iv->inferenceAlgorithm = match.str(1);
                        continue;
                    }
                }

                // close file descriptor
                file.close();

                // return initialization vector
                return iv;
            } else {

                throw FileNotFoundException();
            }
        }

        void save(const std::string &filename, InitializationVector *iv) {
            // open file
            std::ofstream file(filename);

            // check if file is open
            if (file.is_open()) {

                // write data
                file << *iv;

                // close file
                file.close();
            } else {

                // error while opening file
                throw UnableWriteFileException();
            }
        }

        bool isWhitespaceOrQuotationMark(char c) {
            return c == ' ' || c == '"';
        }

        std::ostream &operator<<(std::ostream &os, const InitializationVector &iv) {
            // set indentations
            std::string indentSection(2, ' ');
            std::string indentSectionEntries(4, ' ');

            // write json begin
            os << "{" << std::endl;

            // write nodes section
            // begin nodes section
            os << indentSection << "\"nodes\": {" << std::endl;

            // write each node
            // binary nodes
            for (size_t i = 0; i < iv.binaryNodes.size(); ++i) {

                os << indentSectionEntries << "\"" << iv.binaryNodes[i] << "\": 2";

                if (i == iv.binaryNodes.size() - 1) {

                    os << std::endl;
                } else {

                    os << "," << std::endl;
                }
            }

            // nodes with 4 states
            for (size_t i = 0; i < iv.nodes.size(); ++i) {

                os << indentSectionEntries << "\"" << iv.nodes[i] << "\": 4";

                if (i == iv.nodes.size() - 1) {

                    os << std::endl;
                } else {

                    os << "," << std::endl;
                }
            }

            // end nodes section
            os << indentSection << "}," << std::endl;

            // write connections section
            // begin connections section
            os << indentSection << "\"connections\": {" << std::endl;

            // write each connection
            size_t itCounter = 0;

            for (std::unordered_map<std::string, std::vector<std::string> >::const_iterator it = iv.connections.begin();
                 it != iv.connections.end(); it++) {

                os << indentSectionEntries << "\"" << (*it).first << "\": [";

                for (size_t i = 0; i < (*it).second.size(); ++i) {

                    os << "\"" << (*it).second[i] << "\"";

                    if (i == (*it).second.size() - 1) {

                        os << "]";
                    } else {

                        os << ", ";
                    }
                }

                if (++itCounter < iv.connections.size()) {

                    os << "," << std::endl;
                } else {

                    os << std::endl;
                }
            }

            // end connections section
            os << indentSection << "}," << std::endl;

            // write cpt section
            // begin cpt section
            os << indentSection << "\"cpt\": {" << std::endl;

            // write each connection
            itCounter = 0;

            for (std::unordered_map<std::string, std::vector<double> >::const_iterator it = iv.cpt.begin();
                 it != iv.cpt.end(); it++) {

                os << indentSectionEntries << "\"" << (*it).first << "\": [";

                for (size_t i = 0; i < (*it).second.size(); ++i) {

                    os << (*it).second[i];

                    if (i == (*it).second.size() - 1) {

                        os << "]";
                    } else {

                        os << ", ";
                    }
                }

                if (++itCounter < iv.cpt.size()) {

                    os << "," << std::endl;
                } else {

                    os << std::endl;
                }
            }

            // end cpt section
            os << indentSection << "}";

            // inference section
            if (!iv.inferenceAlgorithm.empty()) {

                os << "," << std::endl << indentSection << "\"inference\": \"" << iv.inferenceAlgorithm << "\""
                   << std::endl;
            } else {

                os << std::endl;
            }

            // write end json file
            os << "}" << std::endl;

            return os;
        }
    }
}
