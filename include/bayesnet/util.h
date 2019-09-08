/// @file
/// @brief Defines Useful helper classes and function which are used by libBayesNet.


#ifndef BAYESNET_FRAMEWORK_UTIL_H
#define BAYESNET_FRAMEWORK_UTIL_H


#include <vector>
#include <string>
#include <random>


namespace bayesNet {

    namespace utils {

        /// Returns split string. Splits @a s by @a delimiter
        std::vector<std::string> split(const std::string &s, char delimiter);

        /// Writes file list of directory @a name to @a v
        void readDirectory(const std::string &name, std::vector<std::string> &v);

        /// Returns the sum of the entries of the vector @a v
        double vectorSum(const std::vector<double> &v);

        /// Normalizes the vector @a v
        void vectorNormalize(std::vector<double> &v);

        /// Returns true if @a c is whitespace or quotation character
        bool isWhitespaceOrQuotationMark(char c);

        /// Digitwise counter class
        /** The class increments a number by each digit seperatly with overflow carry if maximum state is reached.
         *  Least significant digit is at zero position of vector.
         */
        class Counter {
        public:
            /// Constructs a Counter with @a digits and maximum @a states for eaxch digit
            Counter(size_t digits, const std::vector<size_t> &states);

            /// Destructor
            ~Counter();

            /// Increments the counter and returns false if maxiumum was reached
            bool countUp();

            /// Returns current incremental
            size_t getIncrement() const;

            /// Returns the maximum possible increment befor overflow
            size_t getMaximumIncrement() const;

            /// Resets the counter
            void reset();

            /// Returns the current counter value as digits
            std::vector<size_t> &getCount();

        private:
            /// Stores the current counter value
            std::vector<size_t> _count;

            /// Stores the maximum possible values for each digit
            std::vector<size_t> _states;

            /// Stores the current increment
            size_t _increment;
        };
    }
}


#endif //BAYESNET_FRAMEWORK_UTIL_H
