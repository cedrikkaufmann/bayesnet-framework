//
// Created by Cedrik Kaufmann on 2019-06-12.
//

#include <bayesnet/exception.h>

namespace BayesNet {

    const char *NotInitializedException::what() const throw() {
        return "Bayesian network not initialized";
    }

    const char *BayesNodeAlreadyDefinedException::what() const throw() {
        return "Bayes node with the same identifier already registered";
    }

    const char *BayesNodeNotFoundException::what() const throw() {
        return "Bayes node with given identifier does not exist";
    }

    const char *IndexOutOfBoundException::what() const throw() {
        return "Index out of bounds for bayes belief";
    }
}
