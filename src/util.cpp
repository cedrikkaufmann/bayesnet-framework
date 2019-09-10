#include <sstream>
#include <dirent.h>

#include <bayesnet/util.h>
#include <bayesnet/exception.h>


namespace bayesNet {

    namespace utils {

        std::vector<std::string> split(const std::string &s, char delimiter) {
            std::vector<std::string> tokens;
            std::string token;
            std::istringstream tokenStream(s);

            while (std::getline(tokenStream, token, delimiter)) {
                tokens.push_back(token);
            }

            return tokens;
        }

        void readDirectory(const std::string &name, std::vector<std::string> &v) {
            DIR *dir = opendir(name.c_str());
            struct dirent *dp;

            while ((dp = readdir(dir)) != NULL) {
                v.push_back(dp->d_name);
            }

            closedir(dir);

            // drop items "." and ".." from list
            v.erase(v.begin(), v.begin() + 2);
        }

        double vectorSum(const std::vector<double> &v) {
            double sum = 0;

            for (size_t i = 0; i < v.size(); ++i) {
                sum += v[i];
            }

            return sum;
        }

        void vectorNormalize(std::vector<double> &v) {
            double sum = vectorSum(v);

            for (size_t i = 0; i < v.size(); ++i) {
                v[i] = v[i] / sum;
            }
        }

        bool isWhitespaceOrQuotationMark(char c) {
            return c == ' ' || c == '"';
        }

        Counter::Counter(size_t digits, const std::vector<size_t> &states) : _count(digits), _states(states), _increment(0) {}

        Counter::~Counter() {}

        std::vector<size_t> &Counter::getCount() {
            return _count;
        }

        bool Counter::countUp() {
            size_t digit = 0;

            // increment counter
            _increment++;
            _count[digit]++;

            // increment each digit til no digit overflow has happend
            while (digit < _count.size() && _count[digit] == _states[digit]) {
                _count[digit] = 0;
                _count[++digit]++;
            }

            // check if whole counter overflow has happpend
            bool overflow = true;

            for (size_t i = 0; i < _count.size(); ++i) {
                if (_count[i] != 0) {
                    overflow = false;
                }
            }

            return !overflow;
        }

        void Counter::reset() {
            for (size_t i = 0; i < _count.size(); ++i) {
                _count[i] = 0;
                _increment = 0;
            }
        }

        size_t Counter::getIncrement() const {
            return _increment;
        }

        size_t Counter::getMaximumIncrement() const {
            size_t maxIncrement = 1;

            for (size_t i = 0; i < _states.size(); ++i) {
                maxIncrement *= _states[i];
            }

            return maxIncrement;
        }
    }
}
