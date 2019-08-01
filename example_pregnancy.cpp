//
// Created by Cedrik Kaufmann on 2019-08-01.
//
#include <bayesnet/network.h>
#include <bayesnet/inference.h>
#include <bayesnet/cpt.h>

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

    std::vector<double> pP;
    pP.push_back(1-0.87);
    pP.push_back(0.87);
    net.setCPT("pregnant", BayesNet::CPT(pP));

    std::vector<double> pL;
    pL.push_back(1-0.01);
    pL.push_back(0.10);
    pL.push_back(0.01);
    pL.push_back(1-0.10);
    net.setCPT("progesteroneLevel", BayesNet::CPT(pL));

    std::vector<double> pS;
    pS.push_back(1-0.01);
    pS.push_back(0.10);
    pS.push_back(0.01);
    pS.push_back(1-0.10);
    net.setCPT("scanningTest", BayesNet::CPT(pS));

    std::vector<double> pU;
    pU.push_back(1-0.10);
    pU.push_back(0.20);
    pU.push_back(0.10);
    pU.push_back(1-0.20);
    net.setCPT("urineTest", BayesNet::CPT(pU));

    std::vector<double> pB;
    pB.push_back(1-0.10);
    pB.push_back(0.30);
    pB.push_back(0.10);
    pB.push_back(1-0.30);
    net.setCPT("bloodTest", BayesNet::CPT(pB));

    net.init(BayesNet::Inference::LOOPY_BELIEF_PROPAGATION_SUMPROD);

    net.setEvidence("scanningTest", BayesNet::BELIEF_STATE_FALSE);
    net.setEvidence("bloodTest", BayesNet::BELIEF_STATE_TRUE);
    net.setEvidence("urineTest", BayesNet::BELIEF_STATE_TRUE);

    net.doInference();

    cout << "Approximate (loopy belief propagation) variable marginals:" << endl;
    cout << "P: " << net.getBelief("pregnant") << endl;
    cout << "L: " << net.getBelief("progesteroneLevel") << endl;
    cout << "S: " << net.getBelief("scanningTest") << endl;
    cout << "U: " << net.getBelief("urineTest") << endl;
    cout << "B: " << net.getBelief("bloodTest") << endl;
}