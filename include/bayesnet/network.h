/// @file
/// @brief Defines Network class used to construct a bayesian network and apply bayesian inference.


#ifndef BAYESNET_FRAMEWORK_NETWORK_H
#define BAYESNET_FRAMEWORK_NETWORK_H


#include <string>
#include <vector>
#include <unordered_map>

#include <bayesnet/node.h>
#include <bayesnet/state.h>
#include <bayesnet/cpt.h>
#include <bayesnet/inference.h>
#include <bayesnet/fuzzy.h>
#include <bayesnet/file.h>


namespace bayesNet {

    /// Represents a bayesian network
    /** The Network class is used to create new nodes, connect the nodes and apply the corresponding inference algorithm on the network.
     *  Therefore the class acts as Factory to provide an expressive and easy to use interface. So there is no need to deal directly with other
     *  components of this framework than this factory class. The network class provides an interface for all functionality the libBayesNet
     *  provides.
     */
    class Network {
    public:
        /// Constructor
        Network();

        /// Constructs an empty network using default algorithm properties for @a type
        explicit Network(size_t type);

        /// Constructs an empty network using given @a algorithm
        explicit Network(const inference::Algorithm &algorithm);

        /// Constructs a network using the @a file
        explicit Network(const std::string &file);

        /// Destructor
        virtual ~Network();

        /// Adds a new node to the network with @a name and @a binary info
        void newNode(const std::string &name, bool binary = false);

        /// Adds a new sensor node to the newtork with @a name and @a binary info
        void newSensorNode(const std::string &name, bool binary = false);

        /// Connects two nodes by its name, @a parent node and @a child node
        void newConnection(const std::string &parent, const std::string &child);

        /// Initialize the network
        void init();

        /// Sets a membership function @a mf for node @a name and @a state
        void setMembershipFunction(const std::string &name, size_t state, const std::string &mf);

        /// Sets observation of sensor value @a x on sensor node @a name
        void observe(const std::string &name, double x);

        /// Sets evidence on a node with @a name and @a state
        void setEvidence(const std::string &name, size_t state);

        /// Clears evidence on a node @a name
        void clearEvidence(const std::string &name);

        /// Sets the @a cpt for node @a name 
        void setCPT(const std::string &name, const CPT &cpt);

        /// Parses fuzzy rules from @a file and apply them to the corresponding nodes
        void setFuzzyRules(const std::string &file);

        /// Infer CPTs from fuzzy rules for all available nodes, which has got set fuzzy sets
        void inferCPT();

        /// Infer CPT from fuzzy rules for node @a name
        void inferCPT(const std::string &name);

        /// Apply inference on the network
        void run();

        /// Returns a Node @a name
        Node &getNode(const std::string &name);

        /// Returns all parents of a node @a name
        std::vector<Node *> getParents(const std::string &name);

        /// Returns bayes belief for node @a name 
        state::BayesBelief getBelief(const std::string &name);

        /// Loads a network from @a iv
        void load(file::InitializationVector *iv);

        /// Saves the network to file @a filename
        void save(const std::string &filename);

        /// Saves the network to file @a networkFilename and writing algorithm settings to file @a algorithmFilename
        void save(const std::string &networkFilename, const std::string &algorithmFilename);

    private:
        /// Stores node names for label lookup
        std::unordered_map<std::string, size_t> _registry;

        /// Stores references to nodes
        std::vector<Node *> _nodes;

        /// Stores the inference algorithm instance
        inference::Algorithm _inferenceAlgorithm;

        /// Stores counter for nodes in network
        size_t _nodeCounter;

        /// Stores network initialized flag
        bool _init;

        /// Stores the nodes with available fuzzy sets
        std::vector<std::string> _availableFuzzySets;

        /// Returns parents of a @a node
        std::vector<Node *> getParents(Node &node);

        /// Returns SensorNode instance for @a node or throws exception
        static SensorNode &getSensor(Node &node);

        /// Returns boolean whether @a node is sensor
        static bool isSensor(Node &node);
    };
}


#endif //BAYESNET_FRAMEWORK_NETWORK_H
