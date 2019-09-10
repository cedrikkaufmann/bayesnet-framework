//
// Created by Cedrik Kaufmann on 2019-05-27.
//

#include <iostream>
#include <bayesnet/file.h>

using namespace std;

int main() {
    bayesNet::file::InitializationVector *iv = bayesNet::file::InitializationVector::parse("test.bayesnet");
    
    // add connections for nodes to network
    std::unordered_map<std::string, std::vector<std::string> > &fuzzySets = iv->getFuzzySets();

    for (std::unordered_map<std::string, std::vector<std::string> >::const_iterator it = fuzzySets.begin(); it != fuzzySets.end(); it++) {
        for (size_t i = 0; i < (*it).second.size(); ++i) {
            cout << (*it).first << "--> " << i << ": " << (*it).second[i] << std::endl;
        }
    }

    return 0;
}
