//
// Created by Cedrik Kaufmann on 2019-05-27.
//

#include <iostream>
#include <bayesnet/state.h>
#include <bayesnet/cpt.h>
#include <bayesnet/network.h>
#include <bayesnet/inference.h>

using namespace std;

int main() {
    bayesNet::Network net;

    net.newNode("cloudy", true);
    net.newNode("sprinkler", true);
    net.newNode("rainy", true);
    net.newNode("wetGrass", true);

    net.newSensorNode("sensor1", true);
    net.newSensorNode("sensor2");

    bayesNet::fuzzyLogic::MembershipFunction *mfTrue = new bayesNet::fuzzyLogic::membershipFunctions::Gaussian(0, 0.8);
    bayesNet::fuzzyLogic::MembershipFunction *mfFalse = new bayesNet::fuzzyLogic::membershipFunctions::Triangle(1, 2, 2.5);

    bayesNet::fuzzyLogic::MembershipFunction *mfGood = new bayesNet::fuzzyLogic::membershipFunctions::Bell(0, 2, 5);
    bayesNet::fuzzyLogic::MembershipFunction *mfProbGood = new bayesNet::fuzzyLogic::membershipFunctions::SShape(3, 5);
    bayesNet::fuzzyLogic::MembershipFunction *mfProbBad = new bayesNet::fuzzyLogic::membershipFunctions::PiShape(5, 7, 8, 9);
    bayesNet::fuzzyLogic::MembershipFunction *mfBad = bayesNet::fuzzyLogic::membershipFunctions::fromString("\"gaussian2\": [9, 0.8, 12, 0.6]");

    net.setMembershipFunction("sensor1", bayesNet::state::TRUE, mfTrue);
    net.setMembershipFunction("sensor1", bayesNet::state::FALSE, mfFalse);

    net.setMembershipFunction("sensor2", bayesNet::state::GOOD, mfGood);
    net.setMembershipFunction("sensor2", bayesNet::state::PROBABLY_GOOD, mfProbGood);
    net.setMembershipFunction("sensor2", bayesNet::state::PROBABLY_BAD, mfProbBad);
    net.setMembershipFunction("sensor2", bayesNet::state::BAD, mfBad);

    net.newConnection("cloudy", "sprinkler"); // sprinkler given cloudy
    net.newConnection("cloudy", "rainy"); // rainy given cloudy
    net.newConnection("rainy", "wetGrass"); // wet grass given rainy
    net.newConnection("sprinkler", "wetGrass"); // wet grass given sprinkler

    std::vector<double> pCloudy;
    pCloudy.push_back(0.5);
    pCloudy.push_back(0.5);

    std::vector<double> pSprinkler;
    pSprinkler.push_back(0.5);
    pSprinkler.push_back(0.9);
    pSprinkler.push_back(0.5);
    pSprinkler.push_back(0.1);

    std::vector<double> pRainy;
    pRainy.push_back(0.8);
    pRainy.push_back(0.2);
    pRainy.push_back(0.2);
    pRainy.push_back(0.8);

    std::vector<double> pWetGrass;
    pWetGrass.push_back(1.0);  // S = 0, R = 0, W = 0
    pWetGrass.push_back(0.1);  // S = 1, R = 0, W = 0
    pWetGrass.push_back(0.1);  // S = 0, R = 1, W = 0
    pWetGrass.push_back(0.01); // S = 1, R = 1, W = 0
    pWetGrass.push_back(0.0);  // S = 0, R = 0, W = 1
    pWetGrass.push_back(0.9);  // S = 1, R = 0, W = 1
    pWetGrass.push_back(0.9);  // S = 0, R = 1, W = 1
    pWetGrass.push_back(0.99); // S = 1, R = 1, W = 1

    bayesNet::CPT cloudy(pCloudy);
    bayesNet::CPT sprinkler(pSprinkler);
    bayesNet::CPT rainy(pRainy);
    bayesNet::CPT wetGrass(pWetGrass);

    net.setCPT("cloudy", cloudy);
    net.setCPT("sprinkler", sprinkler);
    net.setCPT("rainy", rainy);
    net.setCPT("wetGrass", wetGrass);

    bayesNet::inference::Algorithm *algo = new bayesNet::inference::Algorithm("../../algorithms/junction_tree_default.algorithm");

    net.save("test.bayesnet");

    return 0;
}
