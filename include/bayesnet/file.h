/// @file
/// @brief Defines InitializationVector class and other parsing tools to load bayesian networks and algorithms from file.


#ifndef BAYESNET_FRAMEWORK_FILE_H
#define BAYESNET_FRAMEWORK_FILE_H


#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>


namespace bayesNet {

    namespace file {

        /// Represents a parsed bayesian node, that can be used in InitializationVector
        /** This node representation is an intermediate node representation, which then is used
         *  to initialize a bayesian network.
         */
        class Node {
        public:
        /// @name Constrcutors and Destructors
        //@
            /// Constructs a node from @a name @a states and @a isSensor
            explicit Node(const std::string &name, size_t states = 4, bool isSensor = false);

            /// Destructor
            virtual ~Node();
        //@}

            /// Returns the name of the node
            /// @return name
            const std::string &getName() const;

            /// Returns the states of the node
            /// @return states;
            size_t nrStates();

            /// Returns if node is binary or not
            /// @return binary boolean
            bool isBinary();

            /// Returns if node is sensor or not
            /// @return sensor boolean
            bool isSensor();

        private:
            /// Stores the node name
            std::string _name;

            /// Stores the number of states
            size_t _states;

            /// Stores the sensor flag
            bool _isSensor;
        };

        /// Represents a all parsed network info, that can be used to intialize a bayesian network
        /** This InitializationVector representation is an intermediate network representation,
         *  which then is used to initialize a bayesian network.
         */
        class InitializationVector {
        public:
            /// Constructor
            InitializationVector();

            /// Destructor
            virtual ~InitializationVector();

            /// Adds a new node to the InitializationVector using @a name, @a states and @a isSensor info
            void addNode(const std::string &name, size_t states = 4, bool isSensor = false);

            /// Returns all nodes
            std::vector<Node *> &getNodes();

            /// Sets the connections betweeen nodes corresponding to a parent @a a and its children @a b
            void setConnections(const std::string &a, std::vector<std::string> b);

            /// Returns map of connections using node's name as key
            std::unordered_map<std::string, std::vector<std::string> > &getConnections();

            /// Sets the @a cpt for a node @a name
            void setCPT(const std::string &name, std::vector<double> cpt);

            /// Sets a @a value in a cpt for @a name and corresponding @a index 
            void setCPT(const std::string &name, size_t index, double value);

            /// Returns map of CPTs using node's name as key
            std::unordered_map<std::string, std::vector<double> > &getCPTs();

            /// Sets the fuzzy set for sensor with @a sensorName with corresponding membership functions @a mf
            void setFuzzySet(const std::string &sensorName, std::vector<std::string> mf);

            /// Adds a membership function @a mf for sensor @a sensorName
            void addFuzzySetMembershipFunction(const std::string &sensorName, std::string mf);

            /// Returns all fuzzy sets using node's name as key
            std::unordered_map<std::string, std::vector<std::string> > &getFuzzySets();

            /// Sets the used inference algorithm @a algo
            void setInferenceAlgorithm(const std::string &algo);

            /// Returns the string representation of the used inference algorithm
            const std::string &getInferenceAlgorithm() const;

        private:
            /// Stores nodes
            std::vector<Node *> _nodes;

            /// Stores connections between nodes
            std::unordered_map<std::string, std::vector<std::string> > _connections;

            /// Stores CPTs for nodes
            std::unordered_map<std::string, std::vector<double> > _cpt;

            /// Stores fuzzy set of each node
            std::unordered_map<std::string, std::vector<std::string> > _fuzzySets;

            /// Stores inference algorithm
            std::string _inferenceAlgorithm;
        };

        /// Parses an InitializationVector based on the given network file with @a filename
        InitializationVector *parse(const std::string &filename);

        /// Saves an InitializationVector @a iv to file @a filename
        void save(const std::string &filename, InitializationVector *iv);

        /// Stream operator used to write string representation of @a iv to iostream @a os
        std::ostream &operator<<(std::ostream &os, InitializationVector &iv);
    }
}


#endif //BAYESNET_FRAMEWORK_FILE_H
