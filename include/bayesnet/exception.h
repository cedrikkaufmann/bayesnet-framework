//
// Created by Cedrik Kaufmann on 2019-06-12.
//

#ifndef BAYESNET_FRAMEWORK_EXCEPTION_H
#define BAYESNET_FRAMEWORK_EXCEPTION_H

#include <exception>
#include <stdexcept>
#include <string>

/// Used by DAI_THROW
#define BAYESNET_QUOTE(x) #x

/// Used by DAI_THROW
#define BAYESNET_TOSTRING(x) BAYESNET_QUOTE(x)

#if defined __GNUG__ // GNU C++
  #define FUNCTION_NAME __PRETTY_FUNCTION__
#elif defined _MSC_VER // Visual Studio
  #define FUNCTION_NAME __FUNCTION__
#else // other compilers
  #define FUNCTION_NAME __func__
#endif

#define BAYESNET_THROW(code) throw bayesNet::Exception(bayesNet::Exception::code, __FILE__, FUNCTION_NAME, BAYESNET_TOSTRING(__LINE__), "")
#define BAYESNET_THROWE(code,msg) throw bayesNet::Exception(bayesNet::Exception::code, __FILE__, FUNCTION_NAME, BAYESNET_TOSTRING(__LINE__), msg)

namespace bayesNet {

    class Exception : public std::runtime_error {
    public:
        enum Code {
            NET_NOT_INITIALIZED,
            NODE_ALREADY_EXISTS,
            NODE_NOT_FOUND,
            INDEX_OUT_OF_BOUNDS,
            FILE_NOT_FOUND,
            UNABLE_TO_WRITE_FILE,
            INVALID_ALGORITHM_FILE,
            INVALID_CPT,
            INVALID_CURVE_STRING,
            NUM_ERRORS
        };

        Exception(Code code, const char *filename, const char *function, const char *line, const std::string& detailedMsg);

        ~Exception() throw();

        Code getCode() const;

        const std::string &getMsg() const;

        const std::string &getDetailedMsg() const;

        const std::string& getFilename() const;

        const std::string& getFunction() const;

        const std::string& getLine() const;

        const std::string & message(const Code c) const;

    private:
        Code _errorCode;
        std::string _detailedMsg;
        std::string _filename;
        std::string _function;
        std::string _line;
        static std::string ErrorStrings[NUM_ERRORS];
    };
}

#endif //BAYESNET_FRAMEWORK_EXCEPTION_H
