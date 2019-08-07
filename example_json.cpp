//
// Created by Cedrik Kaufmann on 2019-08-04.
//

#include <bayesnet/network.h>
#include <bayesnet/json.h>
#include <bayesnet/inference.h>
#include <bayesnet/state.h>

#include <iostream>

using namespace std;

int main() {
    BayesNet::Network net("../pregnancy_bayesnet.json");
    net.save("../pregnancy.bayesnet");

    net.init(BayesNet::Inference::LOOPY_BELIEF_PROPAGATION_SUMPROD);

    net.setEvidence("scanningTest", BayesNet::BELIEF_STATE_FALSE);
    net.setEvidence("bloodTest", BayesNet::BELIEF_STATE_FALSE);
    net.setEvidence("urineTest", BayesNet::BELIEF_STATE_FALSE);

    net.doInference();

    cout << "Approximate (loopy belief propagation) variable marginals:" << endl;
    cout << "P: " << net.getBelief("pregnant") << endl;
    cout << "L: " << net.getBelief("progesteroneLevel") << endl;
    cout << "S: " << net.getBelief("scanningTest") << endl;
    cout << "U: " << net.getBelief("urineTest") << endl;
    cout << "B: " << net.getBelief("bloodTest") << endl;
}
