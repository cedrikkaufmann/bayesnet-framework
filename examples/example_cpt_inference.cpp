//
// Created by Cedrik Kaufmann on 2019-08-16.
//

#include <iostream>
#include <bayesnet/fuzzy.h>

int main() {
    // setup mf
    bayesNet::MembershipFunction *left = new bayesNet::membershipFunctions::ZShape(0, 5);
    bayesNet::MembershipFunction *right = new bayesNet::membershipFunctions::SShape(0, 5);
    bayesNet::MembershipFunction *gaussianLeft = new bayesNet::membershipFunctions::Gaussian(1, 0.7);
    bayesNet::MembershipFunction *gaussianRight = new bayesNet::membershipFunctions::Gaussian(3, 0.7);

    // setup fuzzy sets
    bayesNet::FuzzySet *fuzzySetSprinkler = new bayesNet::FuzzySet(2);
    fuzzySetSprinkler->setMembershipFunction(0, left);
    fuzzySetSprinkler->setMembershipFunction(1, right);

    bayesNet::FuzzySet *fuzzySetRainy = new bayesNet::FuzzySet(2);
    fuzzySetRainy->setMembershipFunction(0, gaussianLeft);
    fuzzySetRainy->setMembershipFunction(1, gaussianRight);

    std::vector<bayesNet::FuzzySet *> wetGrass;
    wetGrass.push_back(fuzzySetSprinkler);
    wetGrass.push_back(fuzzySetRainy);

    // define states
    bayesNet::FuzzyRuleState *stateTrue = new bayesNet::FuzzyRuleState(bayesNet::state::TRUE, true);
    bayesNet::FuzzyRuleState *stateFalse = new bayesNet::FuzzyRuleState(bayesNet::state::FALSE, true);

    // define rules

    // S=0, R=0 => W=0
    std::vector<bayesNet::FuzzyRuleState *> states_s_F_R_F;
    states_s_F_R_F.push_back(stateFalse);
    states_s_F_R_F.push_back(stateFalse);

    bayesNet::FuzzyRule *rule_s_F_R_F = new bayesNet::FuzzyRule(states_s_F_R_F, stateFalse);

    // S=1, R=0 => W=1
    std::vector<bayesNet::FuzzyRuleState *> states_s_T_R_F;
    states_s_T_R_F.push_back(stateTrue);
    states_s_T_R_F.push_back(stateFalse);

    bayesNet::FuzzyRule *rule_s_T_R_F = new bayesNet::FuzzyRule(states_s_T_R_F, stateTrue);

    // S=0, R=1 => W=1
    std::vector<bayesNet::FuzzyRuleState *> states_s_F_R_T;
    states_s_F_R_T.push_back(stateFalse);
    states_s_F_R_T.push_back(stateTrue);

    bayesNet::FuzzyRule *rule_s_F_R_T = new bayesNet::FuzzyRule(states_s_F_R_T, stateTrue);

    // S=1, R=0 => W=1
    std::vector<bayesNet::FuzzyRuleState *> states_s_T_R_T;
    states_s_T_R_T.push_back(stateTrue);
    states_s_T_R_T.push_back(stateTrue);

    bayesNet::FuzzyRule *rule_s_T_R_T = new bayesNet::FuzzyRule(states_s_T_R_T, stateTrue);

    // combine rules
    std::vector<bayesNet::FuzzyRule *> rulesWetGrass;
    rulesWetGrass.push_back(rule_s_F_R_F);
    rulesWetGrass.push_back(rule_s_T_R_F);
    rulesWetGrass.push_back(rule_s_F_R_T);
    rulesWetGrass.push_back(rule_s_T_R_T);

    bayesNet::FuzzyRuleSet *rules = new bayesNet::FuzzyRuleSet(rulesWetGrass);

    // create controller from fuzzy sets and -rules
    bayesNet::FuzzyController *wetGrassCtrl = new bayesNet::FuzzyController(wetGrass, rules, 0.01);

    // infer cpts
    bayesNet::CPT wetGrassCPT = wetGrassCtrl->inferCPT();

    for (size_t i = 0; i < wetGrassCPT.size(); ++i) {

        std::cout << wetGrassCPT[i] << std::endl;
    }

    return 0;
}
