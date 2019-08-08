//
// Created by Cedrik Kaufmann on 2019-08-04.
//

#include <bayesnet/network.h>
#include <bayesnet/inference.h>
#include <bayesnet/state.h>

#include <iostream>

using namespace std;

int main() {
    bayesNet::Network net("../examples/pregnancy.bayesnet");

    net.init(bayesNet::Inference::LOOPY_BELIEF_PROPAGATION_SUMPROD);

    net.setEvidence("scanningTest", bayesNet::belief::FALSE);
    net.setEvidence("bloodTest", bayesNet::belief::FALSE);
    net.setEvidence("urineTest", bayesNet::belief::FALSE);

    net.doInference();

    cout << "Approximate (loopy belief propagation) variable marginals:" << endl;
    cout << "P: " << net.getBelief("pregnant") << endl;
    cout << "L: " << net.getBelief("progesteroneLevel") << endl;
    cout << "S: " << net.getBelief("scanningTest") << endl;
    cout << "U: " << net.getBelief("urineTest") << endl;
    cout << "B: " << net.getBelief("bloodTest") << endl;
}
