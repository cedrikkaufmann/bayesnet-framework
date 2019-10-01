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

#include <dai/factor.h>
#include <dai/factorgraph.h>
#include <dai/daialg.h>


namespace bayesNet {

    /// Represents a bayesian network
    /** The Network class is used to create new nodes, connect the nodes and apply the corresponding inference algorithm on the network.
     *  Threrefore the class acts as Facade for the libDAI to provide an expressive easy to use interface. Further it is used as Factory
     *  to for other classes and interfaces provided by the libBayesNet (this library). So there is no need to deal directly with other
     *  components of this framework than this factory class. The network class provides an interface for all functionality the libBayesNet
     *  provides. 
     *  After adding nodes, connecting them and filling them with needed information in form of CPTs, fuzzy sets, fuzzy rules, ... .
     *  The network is ready to be initialized. At this point the Network class processing the Nodes and its Factors and built a 
     *  factorgraph using the interfaces provided by libDAI. After building the factorgraph the inference is applied on the befor built
     *  factorgraph using a builtin inference algorithm provided by libDAI. The results then are read from the factorgraph and can be
     *  accessed trough this Network class.
     */
    class Network {
    public:
        /// Constructor
        Network();

        /// Constructs a network using the @a file
        explicit Network(const std::string &file);

        /// Destructor
        virtual ~Network();

        /// Adds a new node to the network with @a name and @a binary info
        void newNode(const std::string &name, bool binary = false);

        /// Adds a new sensor node to the newtork with @a name and @a binary info
        void newSensorNode(const std::string &name, bool binary = false);

        /// Connects two nodes by its name @a parentName as parent and @a childName as child 
        void newConnection(const std::string &parentName, const std::string &childName);

        /// Initialize the network using the inference algorithm @a alg
        void init(inference::Algorithm *alg);

        /// Sets a memebership function @a mf for node @a name and @a state
        void setMembershipFunction(const std::string &name, size_t state, fuzzyLogic::MembershipFunction *mf);

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
        void doInference();

        /// Returns a Node @a name
        Node *getNode(const std::string &name);

        /// Returns all parents of a node @a name
        std::vector<Node *> getParents(const std::string &name);

        /// Returns bayes belief for node @a name 
        state::BayesBelief getBelief(const std::string &name);

        /// Initializes a network from InitializationVector @a iv
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

        /// Stores the factorgraph
        dai::FactorGraph _factorGraph;

        /// Stores the inference algorithm instance
        inference::Algorithm *_inferenceAlgorithm;

        /// Stores counter for nodes in network
        size_t _nodeCounter;

        /// Stores network initialized flag
        bool _init;

        std::vector<std::string> _availableFuzzySets;

        /// Refreshes the factorgraph based on changed @a node 
        void refreshFactorGraph(Node *node);

        /// Returns parents of a @a node
        std::vector<Node *> getParents(Node *node);
    };
}


#endif //BAYESNET_FRAMEWORK_NETWORK_H
