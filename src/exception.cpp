#include <bayesnet/exception.h>


namespace bayesNet {

    Exception::Exception(Code code, const char *filename, const char *function, const char *line, const std::string& detailedMsg) :
            std::runtime_error(ErrorStrings[code] + (detailedMsg.empty() ? "" : (": " + detailedMsg)) + " [File " + filename + ", line " + line + ", function: " + function + "]"), 
            _errorCode(code), _detailedMsg(detailedMsg), _filename(filename), _function(function), _line(line) {}

    Exception::~Exception() throw() {}

    Exception::Code Exception::getCode() const {
        return _errorCode;
    }

    const std::string &Exception::getMsg() const {
        return ErrorStrings[_errorCode];
    }

    const std::string &Exception::getDetailedMsg() const {
        return _detailedMsg;
    }

    const std::string &Exception::message(const Code c) const {
        return ErrorStrings[c];
    }

    std::string Exception::ErrorStrings[Exception::NUM_ERRORS] = {
        "Bayesian network not initialized",
        "Bayes node already exists",
        "Bayes node not found",
        "Index out of bounds",
        "File not found",
        "Unable to write file",
        "Invalid bayes algorithm file",
        "Invalid conditional probability table",
        "Invalid curve string"
    };
}
