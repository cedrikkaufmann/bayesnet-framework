/*  This file is part of libBayesNet
 *
 *  Copyright (c) 2019, The libBayesNet authors. All rights reserved.
 */


/// @file
/// @brief Defines Useful helper classes and function which are used by libBayesNet.
/// @author Cedrik Kaufmann
/// @version 1.0


#ifndef BAYESNET_FRAMEWORK_UTIL_H
#define BAYESNET_FRAMEWORK_UTIL_H


#include <vector>
#include <string>
#include <random>


namespace bayesNet {

    namespace utils {

        /// Returns splitted string. Splits @a s by @a delimiter 
        std::vector<std::string> split(const std::string &s, char delimiter);

        /// Writes file list of directory @a name to @a v
        void readDirectory(const std::string &name, std::vector<std::string> &v);

        /// Returns the maximum entry of vector @a v
        double vectorMaximum(const std::vector<double> &v);

        /// Returns the sum of the entries of the vector @a v
        double vectorSum(const std::vector<double> &v);

        /// Returns the product of the entrues of the vector @a v
        size_t vectorProduct(const std::vector<size_t> &v);

        /// Normalizes the vector @a v
        void vectorNormalize(std::vector<double> &v);

        /// Returns true if @a c is whitespace or quotation character
        bool isWhitespaceOrQuotationMark(char c);

        /// Bitwise counter class
        /**
         * The class increments a number by each digit seperatly with overflow carry if maximum state is reached.
         * Least significant digit is at zero position of vector.
         */
        class Counter {
        public:
        /// @name Constructors and Destructors
        //@{
            /// Constructs a Counter with @a digits and maxmimum @a states for eaxch digit
            Counter(size_t digits, const std::vector<size_t> &states);

            /// Destructor
            ~Counter();
        //@}

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
