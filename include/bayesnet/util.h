//
// Created by Cedrik Kaufmann on 2019-08-08.
//

#ifndef BAYESNET_FRAMEWORK_UTIL_H
#define BAYESNET_FRAMEWORK_UTIL_H

#include <vector>
#include <string>
#include <random>

namespace bayesNet {

    namespace utils {

        std::vector<std::string> split(const std::string &s, char delimiter);

        void readDirectory(const std::string &name, std::vector<std::string> &v);

        double uniformRandom(double min, double max);

        double vectorMaximum(const std::vector<double> &v);

        double vectorSum(const std::vector<double> &v);

        size_t vectorProduct(const std::vector<size_t> &v);

        void vectorNormalize(std::vector<double> &v);

        class Counter {
        public:
            Counter(size_t digits, const std::vector<size_t>& states);

            ~Counter();

            bool countUp();

            size_t getIncrement() const;

            size_t getMaximumIncrement() const;

            void reset();

            std::vector<size_t> &getCount();

        private:
            std::vector<size_t> _count;
            std::vector<size_t> _states;
            size_t _increment;
        };
    }
}

#endif //BAYESNET_FRAMEWORK_UTIL_H
