/// @file
/// @brief Defines the Exception class and macros used for throwing exceptions.


#ifndef BAYESNET_FRAMEWORK_EXCEPTION_H
#define BAYESNET_FRAMEWORK_EXCEPTION_H


#include <exception>
#include <stdexcept>
#include <string>


/// Used by BAYESNET_THROW
#define BAYESNET_QUOTE(x) #x

/// Used by BAYESNET_THROW
#define BAYESNET_TOSTRING(x) BAYESNET_QUOTE(x)

/// Macro that simplifies throwing an exception with a useful default error message.
/** The error message consists of a description of the exception, the source
 *  code file and line number where the exception has been thrown.
 *  @param code Corresponds to one of the enum values of bayesNet::Exception::Code
 *
 *  @par Example:
 *  @code
 *  BAYESNET_THROW(NET_NOT_INITIALIZED);
 *  @endcode
 */
#if defined __GNUG__ // GNU C++
  #define FUNCTION_NAME __PRETTY_FUNCTION__
#elif defined _MSC_VER // Visual Studio
  #define FUNCTION_NAME __FUNCTION__
#else // other compilers
  #define FUNCTION_NAME __func__
#endif
#define BAYESNET_THROW(code) throw bayesNet::Exception(bayesNet::Exception::code, __FILE__, FUNCTION_NAME, BAYESNET_TOSTRING(__LINE__), "")

/// Macro that simplifies throwing an exception with a user-defined error message.
/** @param code Corresponds to one of the enum values of bayesNet::Exception::Code
 *  @param msg Detailed error message that will be written to std::cerr.
 *
 *  @par Example:
 *  @code
 *  BAYESNET_THROWE(NET_NOT_INITIALIZED,"Detailed error message");
 *  @endcode
 */
#define BAYESNET_THROWE(code,msg) throw bayesNet::Exception(bayesNet::Exception::code, __FILE__, FUNCTION_NAME, BAYESNET_TOSTRING(__LINE__), msg)


namespace bayesNet {

    /// Error handling in libBayesNet is done by throwing an instance of the Exception class.
    /** The Exception class inherits from std::runtime_error. It defines several types of exceptions
     *  and corresponding error messages. The recommended way to throw an instance of the Exception
     *  class is by using the #BAYESNET_THROW or #BAYESNET_THROWE macros.
     */
    class Exception : public std::runtime_error {
    public:
        /// Enumeration of exceptions used in libBayesNet
        enum Code {
            NET_NOT_INITIALIZED,
            NODE_ALREADY_EXISTS,
            NODE_NOT_FOUND,
            INDEX_OUT_OF_BOUNDS,
            UNABLE_TO_OPEN_FILE,
            UNABLE_TO_WRITE_FILE,
            INVALID_ALGORITHM_FILE,
            INVALID_MF_STRING,
            NO_EVIDENCE_ON_FACTOR,
            UNKNOWN_STATE_VALUE,
            ALGORITHM_NOT_INITIALIZED,
            UNKNOWN_ALGORITHM_TYPE,
            NO_SENSOR,
            NUM_ERRORS
        };

        /// Constructor
        Exception(Code code, const char *filename, const char *function, const char *line, const std::string& detailedMsg);

        /// Destructor
        ~Exception() throw();

        /// Returns error code of this exception
        Code getCode() const;

        /// Returns short error message of this exception
        const std::string &getMsg() const;

        /// Returns detailed error message of this exception
        const std::string &getDetailedMsg() const;

        /// Returns filename where this exception was thrown
        const std::string& getFilename() const;

        // Returns function name where this exception was thrown
        const std::string& getFunction() const;

        // Returns line number where this exception was thrown
        const std::string& getLine() const;

        /// Returns error message corresponding to an error code @a c 
        const std::string & message(const Code c) const;

    private:
        /// Contains the error code of this exception
        Code _errorCode;

        /// Contains the detailed message of this exception, if any
        std::string _detailedMsg;

        /// Contains the filename where this exception was thrown
        std::string _filename;

        /// Contains the function name where this exception was thrown
        std::string _function;

        /// Contains the line number where this exception was thrown
        std::string _line;

        /// Error messages corresponding to the exception enumerated above
        static std::string ErrorStrings[NUM_ERRORS];
    };
}


#endif //BAYESNET_FRAMEWORK_EXCEPTION_H
