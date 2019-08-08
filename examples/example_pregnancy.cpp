//
// Created by Cedrik Kaufmann on 2019-08-01.
//
#include <bayesnet/network.h>
#include <bayesnet/inference.h>
#include <bayesnet/cpt.h>
#include <bayesnet/state.h>

#include <iostream>
#include <vector>

using namespace std;

int main() {
    BayesNet::Network net;

    net.newBinaryNode("pregnant");
    net.newBinaryNode("progesteroneLevel");
    net.newBinaryNode("scanningTest");
    net.newBinaryNode("urineTest");
    net.newBinaryNode("bloodTest");

    net.newConnection("pregnant", "progesteroneLevel");
    net.newConnection("pregnant", "scanningTest");
    net.newConnection("progesteroneLevel", "urineTest");
    net.newConnection("progesteroneLevel", "bloodTest");

    std::vector<double> pP(2);
    pP[1] = 0.87;
    net.setCPT("pregnant", BayesNet::CPT(pP));

    std::vector<double> pL(4);
    pL[1] = 0.10;
    pL[2] = 0.01;
    net.setCPT("progesteroneLevel", BayesNet::CPT(pL));

    std::vector<double> pS(4);
    pS[1] = 0.10;
    pS[2] = 0.01;
    net.setCPT("scanningTest", BayesNet::CPT(pS));

    std::vector<double> pU(4);
    pU[1] = 0.20;
    pU[2] = 0.10;
    net.setCPT("urineTest", BayesNet::CPT(pU));

    std::vector<double> pB(4);
    pB[1] = 0.30;
    pB[2] = 0.10;
    net.setCPT("bloodTest", BayesNet::CPT(pB));

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