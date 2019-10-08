/// @file
/// @brief Defines inference algorithm. Implemented as facade for dai::DAIAlg<GRM>.


#ifndef BAYESNET_FRAMEWORK_INFERENCE_H
#define BAYESNET_FRAMEWORK_INFERENCE_H


#include <string>

#include <bayesnet/factor.h>
#include <bayesnet/node.h>
#include <bayesnet/state.h>

#include <dai/properties.h>
#include <dai/daialg.h>


/// Macro that defines default junction tree inference algorithm property string
#define DEFAULT_JUNCTION_TREE_PROPERTIES "[verbose=1,updates=HUGIN]"
/// Macro that defines default loopy belief propagation inference algorithm property string
#define DEFAULT_LOOPY_BELIEF_PROPAGATION_PROPERTIES "[maxiter=1000,tol=1e-9,verbose=1,updates=SEQRND,inference=SUMPROD,logdomain=0]"
/// Macro that defines default fractional loopy belief propagation tree inference algorithm property string
#define DEFAULT_FRACTIONAL_BELIEF_PROPAGATION_PROPERTIES "[maxiter=1000,tol=1e-9,verbose=1,updates=SEQRND,inference=SUMPROD,logdomain=0]"
/// Macro that defines default conditioned loopy belief propagation inference algorithm property string
#define DEFAULT_CONDITIONED_BELIEF_PROPAGATION_PROPERTIES "[bbp_cfn=CFN_GIBBS_B,bbp_props=[damping=0,maxiter=1000,tol=1e-09,updates=SEQ_FIX,verbose=1],choose=CHOOSE_BBP,clamp=CLAMP_VAR,max_levels=100,maxiter=1000,min_max_adj=1,rec_tol=1e-09,recursion=REC_LOGZ,tol=1e-09,updates=SEQRND,verbose=1]"


namespace bayesNet {

    namespace inference {

        /// Represents a inference algorithm class. Facade class to dai::DAIAlg< GRM >.
        /** The class represents a bayesian inference algorithm and its corresponding algorithm parameters.
         *  The algorithm class is able to contruct a inference instance.
         */
        class Algorithm {
        public:
            /// Enumeration of algorithm types used
            enum Type {
                LOOPY_BELIEF_PROPAGATION,
                CONDITIONED_BELIEF_PROPAGATION,
                FRACTIONAL_BELIEF_PROPAGATION,
                JUNCTION_TREE,
                NUM_TYPES
            };

            /// Constructor
            Algorithm();

            /// Constructs a inference algorithm using properties defined in file @a filename
            explicit Algorithm(const std::string &filename);

            /// Constructs a inference algorithm using algorithm type @a type and default properties for teh given type
            explicit Algorithm(size_t type);

            /// Constructs a inference algorithm using algorithm type @a alg and @a properties
            explicit Algorithm(size_t type, const std::string &properties);

            /// Destructor
            virtual ~Algorithm();

            /// Saves the inference algorithm properties to file
            void save();

            /// Saves the inference algorithm properties to @a filename
            void save(const std::string &filename);

            /// Creates a inference instance for the provided factor graph @a fg
            void init(const std::vector<Node *> &nodes);

            /// Partially init the inference instance based on @a node
            void init(Node &node);

            /// Runs the inference algorithm
            void run();

            /// Returns the belief based on the given @a node
            state::BayesBelief belief(const Node &node);

            /// Returns the algorithm type
            size_t getType() const;

            /// Returns the algorithm properties
            dai::PropertySet getProperties() const;

            /// Returns the algorithm properties
            dai::PropertySet &getProperties();

            /// Returns the filename the algorithm will be saved to
            const std::string &getFilename() const;

        private:
            /// Stores the algorithm type
            size_t _algorithm;

            /// Stores the algorithm properties
            dai::PropertySet _inferenceProperties;

            /// Stores the inference instance
            dai::InfAlg *_inferenceInstance;

            /// Stores the filename
            std::string _filename;
        };
    }
}


#endif //BAYESNET_FRAMEWORK_INFERENCE_GUI_H
