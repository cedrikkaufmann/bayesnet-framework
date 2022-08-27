/// @file
/// @brief Defines intermediate classes to represent parsed files. These can be used to load a network and fuzzy rules from file.


#ifndef BAYESNET_FRAMEWORK_FILE_H
#define BAYESNET_FRAMEWORK_FILE_H


#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <memory>


namespace bayesNet {

    namespace file {

        /// Represents a parsed bayesian node, that can be used in InitializationVector
        /** This node representation is an intermediate node representation, which then is used
         *  to initialize a bayesian network.
         */
        class Node {
        public:
            /// Constructs a node from @a name @a states and @a isSensor
            explicit Node(const std::string &name, size_t states = 4, bool isSensor = false);

            /// Destructor
            virtual ~Node();

            /// Returns the name of the node
            const std::string &getName() const;

            /// Returns the states of the node
            size_t nrStates();

            /// Returns if node is binary or not
            bool isBinary();

            /// Returns if node is sensor or not
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

            /// Sets the connections between nodes corresponding to a parent @a a and its children @a b
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

            /// Saves the InitializationVector to @a filename
            void save(const std::string &filename);

            /// Parses a network based on the given @a filename and returns the @return InitializationVector
            static InitializationVector *parse(const std::string &filename);

        private:
            /// Stores nodes
            std::vector<Node *> _nodes;

            /// Stores connections between nodes
            std::unordered_map<std::string, std::vector<std::string> > _connections;

            /// Stores CPTs for nodes
            std::unordered_map<std::string, std::vector<double> > _cpt;

            /// Stores fuzzy set of each node
            std::unordered_map<std::string, std::vector<std::string> > _fuzzySets;

            /// Stores inference algorithm filename
            std::string _inferenceAlgorithm;
        };

        /// Stream operator used to write string representation of @a iv to iostream @a os
        std::ostream &operator<<(std::ostream &os, InitializationVector &iv);

        /// Represents a parsed Fuzzy Rule in a raw intermediate format
        /** This is a intermediate representation of a fuzzy rule which then can be used
         *  to infer CPTs
         */
        class FuzzyRule {
        public:
            /// Constructor
            FuzzyRule();

            /// Destructor
            virtual ~FuzzyRule();

            /// Adds a new if-clause for node @a name with @a state
            void addIfClause(const std::string &name, const std::string state);

            /// Sets the then-clause
            void setThenClause(const std::string state);

            /// Returns the then-clause
            size_t getThenClause() const;

            /// Returns the if-clauses
            std::unordered_map<std::string, size_t> &getIfClauses();

        private:
            /// Stores the if clauses of a fuzzy rule
            std::unordered_map<std::string, size_t> _ifClauses;

            /// Stores the then clause of a fuzzy rule
            size_t _thenClause;
        };

        /// Represents a set of Fuzzy Rules in a raw intermediate format
        /** This is a intermediate representation of a whole system of fuzzy rules which then can be used
         *  to infer CPTs
         */
        class FuzzyRuleVector {
        public:
            /// Constructs a new FuzzyRuleVector for node @a name
            FuzzyRuleVector(const std::string &name);

            /// Destructor
            virtual ~FuzzyRuleVector();

            /// Adds a new fuzzy @a rule
            void addRule(FuzzyRule *rule);

            /// Returns fuzzy rules
            std::vector<FuzzyRule *> &getRules();

            /// Returns the corresponding node name
            const std::string getName() const;

            /// Parses a fuzzy rule file based on the given @a filename and returns a set of FuzzyRuleVector
            static std::vector<FuzzyRuleVector *> parse(const std::string &filename);

        private:
            /// Stores the node name
            std::string _name;
            
            /// Stores the rules
            std::vector<FuzzyRule *> _rules;
        };
 

        struct NodeLogic { 
            NodeLogic() = default;
            virtual ~NodeLogic() = default;

            std::string mf[4];
            std::unordered_map<std::string, double> weights;
        };

        class GeneratorLogic {
        public:
            GeneratorLogic(const std::string& file);
            virtual ~GeneratorLogic() = default;

            void setLogicFile(const std::string& file);
            void parse();
            NodeLogic* getNodeLogic(const std::string& node);

        private:
            std::string _file;
            std::unordered_map<std::string, std::unique_ptr<NodeLogic> > _nodes;
        };
    }
}


#endif //BAYESNET_FRAMEWORK_FILE_H
