//
// Created by Cedrik Kaufmann on 2019-06-12.
//

#ifndef BAYESNET_FRAMEWORK_EXCEPTION_H
#define BAYESNET_FRAMEWORK_EXCEPTION_H

#include <exception>

namespace BayesNet {

    class NotInitializedException : public std::exception {
        const char *what() const noexcept override;
    };

    class BayesNodeAlreadyDefinedException : public std::exception {
        const char *what() const noexcept override;
    };

    class BayesNodeNotFoundException : public std::exception {
        const char *what() const noexcept override;
    };

    class InferencePropertyNotImplementedException : public std::exception {
        const char *what() const noexcept override;
    };
}

#endif //BAYESNET_FRAMEWORK_EXCEPTION_H
