//
// Created by Cedrik Kaufmann on 2019-06-12.
//

#ifndef BAYESNET_FRAMEWORK_EXCEPTION_H
#define BAYESNET_FRAMEWORK_EXCEPTION_H

#include <exception>

namespace BayesNet {

    class NotInitializedException : public std::exception {
        const char *what() const throw();
    };

    class BayesNodeAlreadyDefinedException : public std::exception {
        const char *what() const throw();
    };

    class BayesNodeNotFoundException : public std::exception {
        const char *what() const throw();
    };
}

#endif //BAYESNET_FRAMEWORK_EXCEPTION_H
