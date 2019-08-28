//
// Created by Cedrik Kaufmann on 2019-08-04.
//

#include <bayesnet/network.h>
#include <bayesnet/state.h>

#include <iostream>

using namespace std;

int main( int argc, char **argv ) {
    bayesNet::Network net("../../networks/pregnancy.bayesnet");

    net.setEvidence("scanningTest", bayesNet::state::FALSE);
    net.setEvidence("bloodTest", bayesNet::state::FALSE);
    net.setEvidence("urineTest", bayesNet::state::FALSE);

    net.doInference();

    cout << "Variable marginals:" << endl;
    cout << "P: " << net.getBelief("pregnant") << endl;
    cout << "L: " << net.getBelief("progesteroneLevel") << endl;
    cout << "S: " << net.getBelief("scanningTest") << endl;
    cout << "U: " << net.getBelief("urineTest") << endl;
    cout << "B: " << net.getBelief("bloodTest") << endl;

    return 0;
}
