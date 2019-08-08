//
// Created by Cedrik Kaufmann on 2019-06-12.
//

#ifndef BAYESNET_FRAMEWORK_EXCEPTION_H
#define BAYESNET_FRAMEWORK_EXCEPTION_H

#include <exception>

namespace bayesNet {

    class NotInitializedException : public std::exception {
        const char *what() const throw();
    };

    class BayesNodeAlreadyDefinedException : public std::exception {
        const char *what() const throw();
    };

    class BayesNodeNotFoundException : public std::exception {
        const char *what() const throw();
    };

    class IndexOutOfBoundException : public std::exception {
        const char *what() const throw();
    };

    class InvalidCPTException : public std::exception {
        const char *what() const throw();
    };

    class FileNotFoundException : public std::exception {
        const char *what() const throw();
    };

    class UnableWriteFileException : public std::exception {
        const char *what() const throw();
    };
}

#endif //BAYESNET_FRAMEWORK_EXCEPTION_H
