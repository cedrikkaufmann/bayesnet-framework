//
// Created by Cedrik Kaufmann on 2019-05-27.
//

#include <dai/factorgraph.h>
#include <iostream>
#include <fstream>
#include <map>
#include <bayesnet/node.h>
#include <bayesnet/factor.h>
#include <bayesnet/state.h>
#include <bayesnet/cpt.h>
#include <bayesnet/network.h>
#include <bayesnet/inference.h>

using namespace std;
using namespace dai;

int main() {
    BayesNet::Network net;

    net.newBinaryNode("cloudy");
    net.newBinaryNode("sprinkler");
    net.newBinaryNode("rainy");
    net.newBinaryNode("wetGrass");

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
    pWetGrass.push_back(0.899); // S = 1, R = 1, W = 1

    BayesNet::CPT cloudy(pCloudy);
    BayesNet::CPT sprinkler(pSprinkler);
    BayesNet::CPT rainy(pRainy);
    BayesNet::CPT wetGrass(pWetGrass);

    net.setCPT("cloudy", cloudy);
    net.setCPT("sprinkler", sprinkler);
    net.setCPT("rainy", rainy);
    net.setCPT("wetGrass", wetGrass);

    net.init(BayesNet::LOOPY_BELIEF_PROPAGATION_SUMPROD);
    net.doInference();

    cout << "Approximate (loopy belief propagation) variable marginals:" << endl;
    cout << "Cloudy: " << net.getBelief("cloudy") << endl; // display the belief of bp for that variable
    cout << "Sprinkler: " << net.getBelief("sprinkler") << endl;
    cout << "Rainy: " << net.getBelief("rainy") << endl;
    cout << "Wet grass: " << net.getBelief("wetGrass") << endl;

    net.setEvidence("sprinkler", 1);
    net.doInference();

    cout << "Approximate (loopy belief propagation) variable marginals:" << endl;
    cout << "Cloudy: " << net.getBelief("cloudy") << endl; // display the belief of bp for that variable
    cout << "Sprinkler: " << net.getBelief("sprinkler") << endl;
    cout << "Rainy: " << net.getBelief("rainy") << endl;
    cout << "Wet grass: " << net.getBelief("wetGrass") << endl;

    /*
    BayesNet::Network net;

    net.newNode("cloudy");
    net.newNode("sprinkler");
    net.newNode("rainy");
    net.newNode("wetGrass");

    // divergent connection
    net.newConnection("cloudy", "sprinkler"); // sprinkler given cloudy
    net.newConnection("cloudy", "rainy"); // rainy given cloudy

    // convergent connection
    net.newConnection("rainy", "wetGrass"); // wet grass given rainy
    net.newConnection("sprinkler", "wetGrass"); // wet grass given sprinkler

    BayesNet::Node &cloudy = net.getNode("cloudy");
    BayesNet::Node &rainy = net.getNode("rainy");
    BayesNet::Node &sprinkler = net.getNode("sprinkler");
    BayesNet::Node &wetGrass = net.getNode("wetGrass");

    cloudy.getFactor().set(0, 0.5);
    cloudy.getFactor().set(1, 0.5);

    sprinkler.getFactor().set(0, 0.5);   // C = 0, S = 0
    sprinkler.getFactor().set(1, 0.9);   // C = 1, S = 0
    sprinkler.getFactor().set(2, 0.5);   // C = 0, S = 1
    sprinkler.getFactor().set(3, 0.1);   // C = 1, S = 1

    rainy.getFactor().set(0, 0.8);   // C = 0, R = 0
    rainy.getFactor().set(1, 0.2);   // C = 1, R = 0
    rainy.getFactor().set(2, 0.2);   // C = 0, R = 1
    rainy.getFactor().set(3, 0.8);   // C = 1, R = 1

    wetGrass.getFactor().set(0, 1.0);  // S = 0, R = 0, W = 0
    wetGrass.getFactor().set(1, 0.1);  // S = 1, R = 0, W = 0
    wetGrass.getFactor().set(2, 0.1);  // S = 0, R = 1, W = 0
    wetGrass.getFactor().set(3, 0.01); // S = 1, R = 1, W = 0
    wetGrass.getFactor().set(4, 0.0);  // S = 0, R = 0, W = 1
    wetGrass.getFactor().set(5, 0.9);  // S = 1, R = 0, W = 1
    wetGrass.getFactor().set(6, 0.9);  // S = 0, R = 1, W = 1
    wetGrass.getFactor().set(7, 0.899); // S = 1, R = 1, W = 1

    wetGrass.getFactor().toString();

    net.init(BayesNet::LOOPY_BELIEF_PROPAGATION_SUMPROD);
    net.doInference();


    cout << "Approximate (loopy belief propagation) variable marginals:" << endl;
    cout << "Cloudy: " << net.getBelief("cloudy") << endl; // display the belief of bp for that variable
    cout << "Sprinkler: " << net.getBelief("sprinkler") << endl;
    cout << "Rainy: " << net.getBelief("rainy") << endl;
    cout << "Wet grass: " << net.getBelief("wetGrass") << endl;

    net.setEvidence("sprinkler", 1);
    net.doInference();

    cout << "Approximate (loopy belief propagation) variable marginals:" << endl;
    cout << "Cloudy: " << net.getBelief("cloudy") << endl; // display the belief of bp for that variable
    cout << "Sprinkler: " << net.getBelief("sprinkler") << endl;
    cout << "Rainy: " << net.getBelief("rainy") << endl;
    cout << "Wet grass: " << net.getBelief("wetGrass") << endl;*/

    return 0;
}
