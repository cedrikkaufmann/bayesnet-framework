#include <iostream>

#include <bayesnet/network.h>

int main(int argc, char **argv) {
    if (argc == 4) {
        std::string networkFile = std::string(argv[1]);
        std::string ruleFile = std::string(argv[2]);
        std::string destFile = std::string(argv[3]);

        std::cout << "Inferring CPTs using following arguments" << std::endl;
        std::cout << "Network >> " << networkFile << std::endl;
        std::cout << "Fuzzy rules >> " << ruleFile << std::endl << std::endl;

        // create network instance
        std::cout << ">> Load network" << std::endl;
        bayesNet::Network network(networkFile);

        // load fuzzy rules
        std::cout << ">> Load fuzzy rules" << std::endl;
        network.setFuzzyRules(ruleFile);

        // infer CPTs
        std::cout << ">> Apply inference" << std::endl;
        network.inferCPT();

        // save network to destination
        std::cout << ">> Save new network file" << std::endl;
        network.save(destFile);
    } else {
        std::cout << "InferCPT is a tool to calculate CPTs based on a set of fuzzy rules" << std::endl << std::endl;
        std::cout << "Usage:   " << "infer_cpt <network_file> <fuzzy_rules_file> <dest_file>" << std::endl;
    }

    return 0;
}