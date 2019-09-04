//
// Created by Cedrik Kaufmann on 2019-08-03.
//

#include <fstream>
#include <regex>
#include <algorithm>

#include <bayesnet/file.h>
#include <bayesnet/util.h>
#include <bayesnet/exception.h>

namespace bayesNet {

    namespace file {

        Node::Node(const std::string &name, size_t states, bool isSensor) : _name(name), _states(states), _isSensor(isSensor) {}

        std::string const Node::getName() {
            return _name;
        }

        size_t Node::getStates() {
            return _states;
        }

        bool Node::isBinary() {
            return (_states == 2);
        }

        bool Node::isSensor() {
            return _isSensor;
        }

        InitializationVector::InitializationVector() {}
        
        void InitializationVector::addNode(const std::string &name, size_t states, bool isSensor) {
            Node *node = new Node(name, states, isSensor);
            _nodes.push_back(node);
        }
        
        std::vector<Node *> &InitializationVector::getNodes() {
            return _nodes;
        }

        void InitializationVector::setConnections(const std::string &a, std::vector<std::string> b) {
            _connections[a] = b;
        }

        std::unordered_map<std::string, std::vector<std::string> > &InitializationVector::getConnections() {
        return _connections;
        }

        void InitializationVector::setCPT(const std::string &name, std::vector<double> cpt) {
            _cpt[name] = cpt;
        }

        void InitializationVector::setCPT(const std::string &name, size_t index, double value) {
            _cpt[name][index] = value;
        }

        std::unordered_map<std::string, std::vector<double> > &InitializationVector::getCPTs() {
            return _cpt;
        }

        void InitializationVector::setFuzzySet(const std::string &sensorName, std::vector<std::string> curves) {
            _fuzzySets[sensorName] = curves;
        }

        std::unordered_map<std::string, std::vector<std::string> > &InitializationVector::getFuzzySets() {
            return _fuzzySets;
        }

        void InitializationVector::setInferenceAlgorithm(const std::string &algo) {
            _inferenceAlgorithm = algo;
        }

        std::string const InitializationVector::getInferenceAlgorithm() {
            return _inferenceAlgorithm;
        }

        InitializationVector *parse(const std::string &filename) {
            // regular expressions to parse json file
            std::regex beginRegEx("^\\s*\\{\\s*$");
            std::regex endRegEx("^\\s*}\\s*,?\\s*$");
            std::regex sectionRegEx("^\\s*\"(nodes|sensors|connections|cpt)\"\\s*:\\s*\\{\\s*$");
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
                bool sectionSensors = false;
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
                        size_t states = std::stoul(match.str(2));
                        iv->addNode(match.str(1), states);

                        continue;
                    }

                    // check for sensor node name and count of states
                    if (sectionSensors && std::regex_match(line, match, nodesRegEx)) {
                        // add node to iv
                        size_t states = std::stoul(match.str(2));
                        iv->addNode(match.str(1), states, true);

                        continue;
                    }

                    // check for connections
                    if (sectionConnections && std::regex_match(line, match, connectionsRegEx)) {
                        // get connection list
                        std::string connections = match.str(2);

                        // further string processing
                        // remove whitespaces and quotation marks
                        connections.erase(
                                std::remove_if(connections.begin(), connections.end(), utils::isWhitespaceOrQuotationMark),
                                connections.end()
                        );

                        // split connection list and at to iv
                        iv->setConnections(match.str(1), utils::split(connections, ','));

                        continue;
                    }

                    // check for cpts
                    if (sectionCPT && std::regex_match(line, match, cptRegEx)) {
                        // get cpt list
                        std::string cpt = match.str(2);

                        // remove whitespaces
                        cpt.erase(std::remove(cpt.begin(), cpt.end(), ' '), cpt.end());
                        // split connection list
                        std::vector<std::string> splitCPT = utils::split(cpt, ',');

                        // convert string to double and add to iv
                        iv->setCPT(match.str(1), std::vector<double>(splitCPT.size()));

                        for (size_t i = 0; i < splitCPT.size(); ++i) {

                            iv->setCPT(match.str(1), i, std::stod(splitCPT[i]));
                        }

                        continue;
                    }

                    // check for end of section/file
                    if (std::regex_match(line, match, endRegEx)) {
                        // end of section nodes/connections/cpt
                        if (sectionNodes || sectionSensors || sectionConnections || sectionCPT) {
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
                        } else if (match.str(1) == "sensors") {
                            sectionSensors = true;
                        } else if (match.str(1) == "connections") {
                            sectionConnections = true;
                        } else if (match.str(1) == "cpt") {
                            sectionCPT = true;
                        }

                        continue;
                    }

                    // check for inference option
                    if (std::regex_match(line, match, inferenceRegEx)) {
                        iv->setInferenceAlgorithm(match.str(1));
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
                // close file descriptor
                file.close();
            } else {
                // error while opening file
                throw UnableWriteFileException();
            }
        }

        std::ostream &operator<<(std::ostream &os, InitializationVector &iv) {
            // set indentations
            std::string indentSection(2, ' ');
            std::string indentSectionEntries(4, ' ');
            std::string indentSubSectionEntries(6, ' ');

            // write json begin
            os << "{" << std::endl;
            // write nodes section
            // begin nodes section
            os << indentSection << "\"nodes\": {" << std::endl;

            // write each node
            std::vector<Node *> &nodes = iv.getNodes();
            std::vector<Node *> sensorNodes;
            std::vector<Node *> normalNodes;

            for (size_t i = 0; i < nodes.size(); i++) {
                if (nodes[i]->isSensor()) {
                    sensorNodes.push_back(nodes[i]);
                } else {
                    normalNodes.push_back(nodes[i]);
                }
            }

            // write normal nodes
            for (size_t i = 0; i < normalNodes.size(); ++i) {
                os << indentSectionEntries << "\"" << nodes[i]->getName();

                if (normalNodes[i]->isBinary()) {
                    os << "\": 2";
                } else {
                    os << "\": 4";
                }

                if (i == normalNodes.size() - 1) {
                    os << std::endl;
                } else {
                    os << "," << std::endl;
                }
            }

            // end nodes section
            os << indentSection << "}," << std::endl;

            // write sensor nodes section
            // begin sensor nodes section
            os << indentSection << "\"sensors\": {" << std::endl;

            // write sensor nodes
            for (size_t i = 0; i < sensorNodes.size(); ++i) {
                os << indentSectionEntries << "\"" << sensorNodes[i]->getName();

                if (sensorNodes[i]->isBinary()) {
                    os << "\": 2";
                } else {
                    os << "\": 4";
                }

                if (i == sensorNodes.size() - 1) {
                    os << std::endl;
                } else {
                    os << "," << std::endl;
                }
            }

            // end sensor nodes section
            os << indentSection << "}," << std::endl;

            // write connections section
            // begin connections section
            os << indentSection << "\"connections\": {" << std::endl;

            // write each connection
            size_t itCounter = 0;

            std::unordered_map<std::string, std::vector<std::string> > &connections = iv.getConnections();

            for (std::unordered_map<std::string, std::vector<std::string> >::const_iterator it = connections.begin(); it != connections.end(); it++) {
                os << indentSectionEntries << "\"" << (*it).first << "\": [";

                for (size_t i = 0; i < (*it).second.size(); ++i) {
                    os << "\"" << (*it).second[i] << "\"";

                    if (i == (*it).second.size() - 1) {
                        os << "]";
                    } else {
                        os << ", ";
                    }
                }

                if (++itCounter < connections.size()) {
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

            std::unordered_map<std::string, std::vector<double> > &cpts = iv.getCPTs();

            for (std::unordered_map<std::string, std::vector<double> >::const_iterator it = cpts.begin(); it != cpts.end(); it++) {
                os << indentSectionEntries << "\"" << (*it).first << "\": [";

                for (size_t i = 0; i < (*it).second.size(); ++i) {
                    os << (*it).second[i];

                    if (i == (*it).second.size() - 1) {
                        os << "]";
                    } else {
                        os << ", ";
                    }
                }

                if (++itCounter < cpts.size()) {
                    os << "," << std::endl;
                } else {
                    os << std::endl;
                }
            }

            // end cpt section
            os << indentSection << "}," << std::endl;

            // write fuzzy sets section
            // begin fuzzy sets section
            os << indentSection << "\"fuzzySets\": {" << std::endl;

            // write each fuzzy set
            std::unordered_map<std::string, std::vector<std::string> > &fuzzySets = iv.getFuzzySets();

            for (std::unordered_map<std::string, std::vector<std::string> >::const_iterator it = fuzzySets.begin(); it != fuzzySets.end(); it++) {
                os << indentSectionEntries << "\"" << (*it).first << "\": {" << std::endl;

                for (size_t i = 0; i < (*it).second.size(); ++i) {
                    os << indentSectionEntries << indentSection << i << ": {" << std::endl;
                    os << indentSectionEntries << indentSection << indentSection << (*it).second[i] << std::endl;

                    if (i == (*it).second.size() - 1) {
                        os << indentSectionEntries << indentSection << "}" << std::endl;
                    } else {
                        os << indentSectionEntries << indentSection  << "}," << std::endl;
                    }
                }

                if (++itCounter < fuzzySets.size()) {
                    os << indentSectionEntries << "}," << std::endl;
                } else {
                    os << indentSectionEntries << "}" << std::endl;
                }
            }

            // end fuzzy section
            os << indentSection << "}";

            // inference section
            std::string algorithm = iv.getInferenceAlgorithm();

            if (!algorithm.empty()) {
                os << "," << std::endl << indentSection << "\"inference\": \"" << algorithm << "\"" << std::endl;
            } else {
                os << std::endl;
            }

            // write end json file
            os << "}" << std::endl;

            return os;
        }
    }
}
