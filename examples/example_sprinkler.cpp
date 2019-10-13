/// @file
/// @brief Example that demonstrates how to create a bayesian network

#include <iostream>
#include <bayesnet/state.h>
#include <bayesnet/cpt.h>
#include <bayesnet/network.h>
#include <bayesnet/inference.h>

using namespace std;

int main() {
    //bayesNet::Network net() // Use default bp
    //bayesNet::Network net(bayesNet::inference::Algorithm::JUNCTION_TREE) // Use default jt
    bayesNet::Network net(bayesNet::inference::Algorithm("../../algorithms/default_mf.algorithm"));

    net.newNode("cloudy", true);
    //net.newSensor("cloudy", true); this would make cloudy an observable sensor
    net.newNode("sprinkler", true);
    net.newNode("rainy", true);
    net.newNode("wetGrass", true);

    net.newConnection("cloudy", "sprinkler"); // sprinkler given cloudy
    net.newConnection("cloudy", "rainy"); // rainy given cloudy
    net.newConnection("rainy", "wetGrass"); // wet grass given rainy
    net.newConnection("sprinkler", "wetGrass"); // wet grass given sprinkler

    // if e.g. cloudy is a sensor
    /*
    net.setMemebershipFunction("cloudy", bayesNet::state::FALSE, "gaussian: [0, 0.35]");
    net.setMemebershipFunction("cloudy", bayesNet::state::TRUE, "gaussian: [1, 0.35]");
    */

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

    net.init();
    net.run();

    cout << "Variable marginals:" << endl;
    cout << "Cloudy: " << net.getBelief("cloudy") << endl;
    cout << "Sprinkler: " << net.getBelief("sprinkler") << endl;
    cout << "Rainy: " << net.getBelief("rainy") << endl;
    cout << "Wet grass: " << net.getBelief("wetGrass") << endl << endl;

    net.setEvidence("sprinkler", bayesNet::state::TRUE);
    //net.observe("cloudy", 2); this would update CPT based on continous obsersvation 2
    net.run();

    cout << "Variable marginals:" << endl;
    cout << "Cloudy: " << net.getBelief("cloudy") << endl;
    cout << "Sprinkler: " << net.getBelief("sprinkler") << endl;
    cout << "Rainy: " << net.getBelief("rainy") << endl;
    cout << "Wet grass: " << net.getBelief("wetGrass") << endl;

    return 0;
}
