/// @file
/// @brief BayesNet CLI, used to infer the network´s CPTs from a given network file and rules file

#include <iostream>

#include <bayesnet/network.h>

int main(int argc, char **argv) {
    if (argc == 3) {
        std::string networkFile(argv[1]);
        std::string ruleFile(argv[2]);

        // std::string networkFile("failure_stop.bayesnet");
        // std::string ruleFile("failure_stop.rules");

        std::cout << "FuzzyRuleGenerator using following arguments" << std::endl;
        std::cout << "Network >> " << networkFile << std::endl;
        std::cout << "Fuzzy rules >> " << ruleFile << std::endl << std::endl;

        // create network instance
        std::cout << ">> Load network" << std::endl;
        bayesNet::Network network(networkFile);

        // generate fuzzy rules
        std::cout << ">> Generate fuzzy rules" << std::endl;
        network.generateDefaultFuzzyRules(ruleFile);
    } else {
        std::cout << "FuzzyRuleGenerator is a tool to generate a default set of fuzzy rules for a given network" << std::endl << std::endl;
        std::cout << "Usage:   " << "generate_fuzzy_rules <network_file> <logic_file> <fuzzy_rules_file>" << std::endl;
    }

    return 0;
}