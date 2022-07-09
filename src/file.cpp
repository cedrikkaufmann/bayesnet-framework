#include <fstream>
#include <regex>
#include <algorithm>

#include <bayesnet/file.h>
#include <bayesnet/util.h>
#include <bayesnet/exception.h>
#include <bayesnet/state.h>


namespace bayesNet {

    namespace file {

        Node::Node(const std::string &name, size_t states, bool isSensor) : _name(name), _states(states), _isSensor(isSensor) {}

        Node::~Node() {}

        const std::string &Node::getName() const {
            return _name;
        }

        size_t Node::nrStates() {
            return _states;
        }

        bool Node::isBinary() {
            return (_states == 2);
        }

        bool Node::isSensor() {
            return _isSensor;
        }

        InitializationVector::InitializationVector() {}

        InitializationVector::~InitializationVector() {}
        
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

        void InitializationVector::setFuzzySet(const std::string &sensorName, std::vector<std::string> mf) {
            _fuzzySets[sensorName] = mf;
        }

        void InitializationVector::addFuzzySetMembershipFunction(const std::string &sensorName, std::string mf) {
            _fuzzySets[sensorName].push_back(mf);
        }

        std::unordered_map<std::string, std::vector<std::string> > &InitializationVector::getFuzzySets() {
            return _fuzzySets;
        }

        void InitializationVector::setInferenceAlgorithm(const std::string &algo) {
            _inferenceAlgorithm = algo;
        }

        const std::string &InitializationVector::getInferenceAlgorithm() const {
            return _inferenceAlgorithm;
        }

        InitializationVector *InitializationVector::parse(const std::string &filename) {
            // make sure that string to double conversion through std::stod works correctly
            setlocale(LC_ALL, "C/de_DE.UTF-8/en_US.UTF-8/C/C/C/C");

            // regular expressions to parse file
            std::regex beginRegEx("^\\s*\\{\\s*$");
            std::regex endRegEx("^\\s*\\}\\s*,?\\s*$");
            std::regex sectionRegEx("^\\s*\"(nodes|sensors|connections|cpt|fuzzySets)\"\\s*:\\s*\\{\\s*$");
            std::regex nodesRegEx("^\\s*\"([a-zA-Z0-9_]+)\"\\s*:\\s*(2|4)\\s*,?\\s*$");
            std::regex connectionsRegEx("^\\s*\"([a-zA-Z0-9_]+)\":\\s*\\[((\\s*\"([a-zA-Z0-9_]+)\"\\s*,?)*)\\],?$");
            // not used anymore due to too large cpt tables
            // std::regex cptRegEx("^\\s*\"([a-zA-Z0-9_]+)\"\\s*:\\s*\\[(.*)],?$");
            std::regex fuzzySetsRegEx("^\\s*\"([a-zA-Z0-9_]+)\"\\s*:\\s*\\{\\s*$");
            std::regex fuzzySetSensorStateRegEx("^\\s*[0-9]+\\s*:\\s*\\{(\\s*\"([a-zA-Z0-9_]+)\"\\s*:\\s*\\[((\\s*[0-9]+\\.?)\\s*,?)*\\]\\s*)\\},?$");
            std::regex inferenceRegEx("^\\s*\"inference\"\\s*:\\s*\"([a-zA-Z0-9_.\\/]*)\"\\s*$");

            // open file
            std::ifstream file(filename);
            std::string line;

            // parsing file
            if (file.is_open()) {
                // new InitializationVector instance
                InitializationVector *iv = new InitializationVector();

                // section flags
                bool beginFile = false;
                bool sectionNodes = false;
                bool sectionSensors = false;
                bool sectionConnections = false;
                bool sectionCPT = false;
                bool sectionFuzzySets = false;
                bool sectionSensorFuzzySetBegin = false;

                // regex matcher
                std::smatch match;
                // last parsed sensor name info
                std::string lastSensorName;

                // parse file til end
                while (getline(file, line)) {
                    // check for begin of file
                    if (!beginFile && std::regex_match(line, beginRegEx)) {
                        beginFile = true;
                        continue;
                    }

                    // check for end of section/file
                    if (std::regex_match(line, match, endRegEx)) {
                        if (sectionSensorFuzzySetBegin) {
                            sectionSensorFuzzySetBegin = false;
                            continue;
                        }

                        // end of section nodes/sensors/connections/cpt/fuzzySets
                        if (sectionNodes || sectionSensors || sectionConnections || sectionCPT || sectionFuzzySets) {
                            sectionNodes = false;
                            sectionSensors = false;
                            sectionConnections = false;
                            sectionCPT = false;
                            sectionFuzzySets = false;

                            continue;
                        }

                        // end of json file, stop parsing
                        break;
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
                    if (sectionCPT) {
                        // regex not working for large strings
                        // so we use a manual parsing approach
                        bool cptBegin = false;
                        bool nameBegin = false;
                        std::vector<double> cpt;
                        std::string cptName;
                        std::string cptNumber;

                        for (size_t i = 0; i < line.size(); i++) {
                            
                            // check for begin of cpt name
                            if (!nameBegin && line[i] == '"') {
                                nameBegin = true;
                                continue;
                            } 
                            
                            // check for end of cpt name
                            if (nameBegin && line[i] == '"') {
                                nameBegin = false;
                                continue;
                            } 
                            
                            // check for begin of cpt
                            if (!cptBegin && line[i] == '[') {
                                cptBegin = true;
                                continue;
                            } 
                            
                            // check for end of cpt
                            if (cptBegin && line[i] == ']') {
                                cptBegin = false;

                                // save last parsed cpt entry
                                cpt.push_back(std::stod(cptNumber));
                                // set cpt of iv
                                iv->setCPT(cptName, cpt);
                        
                                continue;
                            }

                            // parse cpt name
                            if (nameBegin) {
                                cptName += line[i];
                                continue;
                            }

                            // parse cpt entries
                            if (cptBegin) {

                                // ignore whitespaces
                                if (line[i] == ' ') {
                                    continue;
                                }

                                // next cpt entry
                                if (line[i] == ',') {
                                    // save cpt entry
                                    cpt.push_back(std::stod(cptNumber));
                                    // reset to cpt number to empty string
                                    cptNumber = "";

                                    continue;
                                }

                                // add current char as part of cpt entry
                                cptNumber += line[i];

                                continue;
                            }
                        }

                        continue;
                    }

                    // check for fuzzy set membership function
                    if (sectionSensorFuzzySetBegin && std::regex_match(line, match, fuzzySetSensorStateRegEx)) {
                        iv->addFuzzySetMembershipFunction(lastSensorName, match.str(1));

                        continue;
                    }

                    // check for fuzzy sets 
                    if (sectionFuzzySets && std::regex_match(line, match, fuzzySetsRegEx)) {
                        sectionSensorFuzzySetBegin = true;
                        lastSensorName = match.str(1);

                        continue;
                    }

                    // check for section
                    if (std::regex_match(line, match, sectionRegEx)) {
                        std::string section = match.str(1);

                        if (section == "nodes") {
                            sectionNodes = true;
                        } else if (section == "sensors") {
                            sectionSensors = true;
                        } else if (section == "connections") {
                            sectionConnections = true;
                        } else if (section == "cpt") {
                            sectionCPT = true;
                        } else if (section == "fuzzySets") {
                            sectionFuzzySets = true;
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
                BAYESNET_THROWE(UNABLE_TO_OPEN_FILE, filename);
            }
        }

        void InitializationVector::save(const std::string &filename) {
            // open file
            std::ofstream file(filename);

            // check if file is open
            if (file.is_open()) {
                // write data
                file << *this;
                // close file descriptor
                file.close();
            } else {
                // error while opening file
                BAYESNET_THROWE(UNABLE_TO_WRITE_FILE, filename);
            }
        }

        std::ostream &operator<<(std::ostream &os, InitializationVector &iv) {
            // set indentations
            std::string indent(2, ' ');

            // write json begin
            os << "{" << std::endl;

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

            // write sensor nodes section
            // begin sensor nodes section
            os << indent << "\"sensors\": {" << std::endl;

            // write sensor nodes
            for (size_t i = 0; i < sensorNodes.size(); ++i) {
                os << indent << indent << "\"" << sensorNodes[i]->getName();

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
            os << indent << "}," << std::endl;

            // write nodes section
            // begin nodes section
            os << indent << "\"nodes\": {" << std::endl;

            // write normal nodes
            for (size_t i = 0; i < normalNodes.size(); ++i) {
                os << indent << indent << "\"" << normalNodes[i]->getName();

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
            os << indent << "}," << std::endl;

            // write connections section
            // begin connections section
            os << indent << "\"connections\": {" << std::endl;

            // write each connection
            size_t itCounter = 0;

            std::unordered_map<std::string, std::vector<std::string> > &connections = iv.getConnections();

            for (std::unordered_map<std::string, std::vector<std::string> >::const_iterator it = connections.begin(); it != connections.end(); it++) {
                os << indent << indent << "\"" << (*it).first << "\": [";

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
            os << indent << "}," << std::endl;
            // write cpt section
            // begin cpt section
            os << indent << "\"cpt\": {" << std::endl;

            // write each connection
            itCounter = 0;

            std::unordered_map<std::string, std::vector<double> > &cpts = iv.getCPTs();

            for (std::unordered_map<std::string, std::vector<double> >::const_iterator it = cpts.begin(); it != cpts.end(); it++) {
                os << indent << indent << "\"" << (*it).first << "\": [";

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
            os << indent << "}," << std::endl;

            // write fuzzy sets section
            // begin fuzzy sets section
            os << indent << "\"fuzzySets\": {" << std::endl;

            // write each fuzzy set
            std::unordered_map<std::string, std::vector<std::string> > &fuzzySets = iv.getFuzzySets();

            itCounter = 0;

            for (std::unordered_map<std::string, std::vector<std::string> >::const_iterator it = fuzzySets.begin(); it != fuzzySets.end(); it++) {
                os << indent << indent << "\"" << (*it).first << "\": {" << std::endl;

                for (size_t i = 0; i < (*it).second.size(); ++i) {
                    os << indent << indent << indent << i << ": {";

                    if ((*it).second[i] != "NULL") {
                        os << (*it).second[i];
                    }

                    if (i == (*it).second.size() - 1) {
                        os << "}" << std::endl;
                    } else {
                        os << "}," << std::endl;
                    }
                }

                if (++itCounter < fuzzySets.size()) {
                    os << indent << indent << "}," << std::endl;
                } else {
                    os << indent << indent << "}" << std::endl;
                }
            }

            // end fuzzy section
            os << indent << "}";

            // inference section
            std::string algorithm = iv.getInferenceAlgorithm();
            
            if (!algorithm.empty()) {
                os << "," << std::endl << indent << "\"inference\": \"" << algorithm << "\"" << std::endl;
            } else {
                os << std::endl;
            }

            // write end json file
            os << "}" << std::endl;

            return os;
        }

        FuzzyRule::FuzzyRule() {}

        FuzzyRule::~FuzzyRule() {}

        void FuzzyRule::addIfClause(const std::string &name, const std::string state) {
            _ifClauses[name] = state::fromString(state);
        }

        void FuzzyRule::setThenClause(const std::string state) {
            _thenClause = state::fromString(state);
        }

        size_t FuzzyRule::getThenClause() const {
            return _thenClause;
        }

        std::unordered_map<std::string, size_t>& FuzzyRule::getIfClauses() {
            return _ifClauses;
        }

        FuzzyRuleVector::FuzzyRuleVector(const std::string &name) : _name(name) {}

        FuzzyRuleVector::~FuzzyRuleVector() {}

        void FuzzyRuleVector::addRule(bayesNet::file::FuzzyRule *rule) {
            _rules.push_back(rule);
        }

        std::vector<FuzzyRule *> &FuzzyRuleVector::getRules() {
            return _rules;
        }

        const std::string FuzzyRuleVector::getName() const {
            return _name;
        }

        std::vector<FuzzyRuleVector *> FuzzyRuleVector::parse(const std::string &filename) {
            std::regex beginSection("^\\s*([a-zA-Z0-9_]+)\\s*(begin)\\s*$");
            std::regex endSection("^\\s*(end)\\s*$");
            std::regex entry("^\\s*(if)\\s*((\\s*[a-zA-Z0-9_]+=(true|false|good|probably_good|probably_bad|bad)\\s*&?)+)(then)\\s*(true|false|good|probably_good|probably_bad|bad)\\s*$");

            // open file
            std::ifstream file(filename);
            std::string line;

            // parsing file
            if (file.is_open()) {
                std::vector<FuzzyRuleVector *> rules;

                // section flags
                bool section = false;
                size_t sectionIndex = 0;

                // regex matcher
                std::smatch match;

                // parse file til end
                while (getline(file, line)) {
                    if (!section && std::regex_match(line, match, beginSection)) {
                        section = true;
                        sectionIndex++;

                        FuzzyRuleVector *ruleVector = new FuzzyRuleVector(match.str(1));
                        rules.push_back(ruleVector);

                        continue;
                    }

                    if (section && std::regex_match(line, endSection)) {
                        section = false;
                        continue;
                    }

                    if (section && std::regex_match(line, match, entry)) {
                        // get if clauses from regex matcher
                        std::string ifClauses = match.str(2);

                        // remove whitespace
                        ifClauses.erase(
                                std::remove(ifClauses.begin(), ifClauses.end(), ' '),
                                ifClauses.end()
                        );

                        // split states by '&' to get each clause like cloudy=true
                        std::vector<std::string> splitIfClauses = utils::split(ifClauses, '&');

                        // iterate over if clauses and construct rule
                        FuzzyRule *rule = new FuzzyRule();

                        for (size_t i = 0; i < splitIfClauses.size(); ++i) {
                            // split clause by '=' to get node name and state string
                            std::vector<std::string> splitNodeState = utils::split(splitIfClauses[i], '=');
                            rule->addIfClause(splitNodeState[0], splitNodeState[1]);
                        }

                        // get then clause from regex matcher and set
                        rule->setThenClause(match.str(6));
                        rules[sectionIndex - 1]->addRule(rule);

                        continue;
                    }
                }

                return rules;
            } else {
                BAYESNET_THROWE(UNABLE_TO_OPEN_FILE, filename);
            }
        }
    }
}
