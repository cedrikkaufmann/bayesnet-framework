//
// Created by Cedrik Kaufmann on 2019-08-03.
//

#ifndef BAYESNET_FRAMEWORK_FILE_H
#define BAYESNET_FRAMEWORK_FILE_H

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>

namespace bayesNet {

    namespace file {

        class Node {
        public:
            explicit Node(const std::string &name, size_t states = 4, bool isSensor = false);

            std::string const getName();

            size_t getStates();

            bool isBinary();

            bool isSensor();

        private:
            std::string _name;
            size_t _states;
            bool _isSensor;
        };
        
        class FuzzyCurve {
        public:
            explicit FuzzyCurve(const std::string &name);

            std::string getName();

            void addValue(double value);

            std::vector<double> &getValues();

        private:
            std::string _name;
            std::vector<double> _values;
        };

        class InitializationVector {
        public:
            InitializationVector();

            void addNode(const std::string &name, size_t states = 4, bool isSensor = false);

            std::vector<Node *> &getNodes();

            void setConnections(const std::string &a, std::vector<std::string> b);

            std::unordered_map<std::string, std::vector<std::string> > &getConnections();

            void setCPT(const std::string &name, std::vector<double> cpt);

            void setCPT(const std::string &name, size_t index, double value);

            std::unordered_map<std::string, std::vector<double> > &getCPTs();

            void setFuzzySet(const std::string &sensorName, std::vector<FuzzyCurve *> curves);

            std::unordered_map<std::string, std::vector<FuzzyCurve *> > &getFuzzySets();

            void setInferenceAlgorithm(const std::string &algo);

            std::string const getInferenceAlgorithm();

        private:
            std::vector<Node *> _nodes;
            std::unordered_map<std::string, std::vector<std::string> > _connections;
            std::unordered_map<std::string, std::vector<double> > _cpt;
            std::unordered_map<std::string, std::vector<FuzzyCurve*> > _fuzzySets;
            std::string _inferenceAlgorithm;
        };

        InitializationVector *parse(const std::string &filename);

        void save(const std::string &filename, InitializationVector *iv);

        std::ostream &operator<<(std::ostream &os, InitializationVector &iv);

        std::ostream &operator<<(std::ostream &os, FuzzyCurve &curve);
    }
}

#endif //BAYESNET_FRAMEWORK_FILE_H
