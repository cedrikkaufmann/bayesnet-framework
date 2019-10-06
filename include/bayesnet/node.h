/// @file
/// @brief Defines Node class which is used represent a bayesian networks based on an acyclic graph.


#ifndef BAYESNET_FRAMEWORK_NODE_H
#define BAYESNET_FRAMEWORK_NODE_H


#include <dai/factorgraph.h>
#include <bayesnet/factor.h>
#include <bayesnet/cpt.h>
#include <bayesnet/fuzzy.h>


namespace bayesNet {

    /// Represents a bayes node in a bayesian network
    /** The Node is used to represent a bayes node in a network. The network then is representated as an 
     *  acylcic graph using these nodes. Therefore each nodes holds references to its children. 
     *  Furthermore it holds its own CPT and the corresponding factor representation. 
     *  The factor is used to built a factorgraph using libDAI to apply the inference algorithm,
     *  provided by libDAI. 
     *  So the Node is a facade/proxy representing a factor in a factorgraph, which is based on the libDAI,
     *  to provide a more expressive interface to the inference engine from libDAI.
     */
    class Node {
    public:
        /// Constructs a Node by its @a name, @a label and numbe of @a states  
        explicit Node(const std::string &name, size_t label, size_t states);
        
        /// Destructor
        virtual ~Node();

        /// Returns the name of the Node
        const std::string &getName() const;

        /// Returns the discrete representation used by libDAI to build factorgraph
        dai::Var &getDiscrete();

        /// Returns the discrete var representation used by libDAI to build factorgraph
        dai::Var getDiscrete() const;

        /// Returns the conditional discrete var representation used by libDAI to build factorgraph
        dai::VarSet &getConditionalDiscrete();

        /// Returns the conditional discrete var representation used by libDAI to build factorgraph
        dai::VarSet getConditionalDiscrete() const;

        /// Returns the factor representation used by libDAI to build factorgraph
        Factor &getFactor();

        // Returns the number of states
        size_t nrStates() const;

        /// Returns the children of a Node
        std::vector<Node *> &getChildren();

        /// Adds a new Node as child
        void addChild(Node *node);

        /// Sets evidence on this Node for @a state
        void setEvidence(size_t state);

        /// Clears the evidence on this Node
        void clearEvidence();

        /// Returns flag if node is currently evidence
        bool isEvidence() const;

        /// Returns the current evidence state
        size_t evidenceState() const;

        /// Sets a membership function @a mf for @a state
        void setMembershipFunction(size_t state, fuzzyLogic::MembershipFunction *mf);

        /// Sets fuzzy rules @a rules used to infer CPT
        void setFuzzyRules(const fuzzyLogic::RuleSet &rules);

        /// Adds a new fuzzy rule @a rule
        void addFuzzyRule(fuzzyLogic::Rule *rule);

        /// Returns fuzzy set
        fuzzyLogic::Set &getFuzzySet();

        /// Returns all fuzzy rules
        fuzzyLogic::RuleSet &getFuzzyRules();

        /// Returns the Node's CPT
        CPT &getCPT();

        /// Sets the @a cpt
        void setCPT(const CPT &cpt);

        /// Sets the Node's @a index in the libDAI factorgraph representation
        void setFactorGraphIndex(size_t index);

        /// Returns the Node's factorgraph index
        size_t getFactorGraphIndex() const;

        /// Returns boolean if Node is binary or not
        bool isBinary() const;

    private:
        /// Stores the name
        std::string _name;

        /// Stores the discrete var
        dai::Var _discrete;

        /// Stores the conditional discrete vars
        dai::VarSet _conditionalDiscrete;

        /// Stores the factor representation
        Factor _factor;

        /// Stores the factorgraph index
        size_t _factorGraphIndex;
        
        /// Stores CPT
        CPT _cpt;

        /// Stores the fuzzy set
        fuzzyLogic::Set _fuzzySet;

        /// Stores the fuzzy rules
        fuzzyLogic::RuleSet _fuzzyRules;

        /// Stores references to children
        std::vector<Node *> _children;
    };

    /// Represents a bayes node in a bayesian network, which can handle continuous variable observations
    /** The inference engine can apply the inference algorithm on discrete variables only, 
     *  therefore the SensorNode implementation is using fuzzy logic or more precisely the Node's
     *  fuzzy set and its membership functions to map a continuous variable (e.g. sensor reading)
     *  to a discrete CPT rather than the need to calculate inference on a set of continuous functions.
     */
    class SensorNode : public Node {
    public:
        /// Constructs a SensorNode using @a name, @a label and corresponding @a states 
        explicit SensorNode(const std::string &name, size_t label, size_t states);

        /// Destructor
        virtual ~SensorNode();

        /// Maps a continous observation @a x to a discrete CPT representation and builds updates the Node's CPT, Factor 
        void observe(double x);
    };

    /// stream operator used to write string representation of @a node to iostream @a os
    std::ostream &operator<<(std::ostream &os, Node &node);
}


#endif //BAYESNET_FRAMEWORK_NODE_H
