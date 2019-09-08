/// @file
/// @brief Defines inference algorithm facade.


#ifndef BAYESNET_FRAMEWORK_INFERENCE_H
#define BAYESNET_FRAMEWORK_INFERENCE_H


#include <string>

#include <dai/properties.h>
#include <dai/factorgraph.h>
#include <dai/daialg.h>


/// Macro that defines default junction tree inference algorithm property string
#define DEFAULT_JUNCTION_TREE_PROPERTIES "[verbose=1,updates=HUGIN]"
/// Macro that defines default loopy belief propagation inference algorithm property string
#define DEFAULT_LOOPY_BELIEF_PROPAGATION_PROPERTIES "[maxiter=1000,tol=1e-9,verbose=1,updates=SEQRND,inference=SUMPROD,logdomain=0]"
/// Macro that defines default fractional loopy belief propagation tree inference algorithm property string
#define DEFAULT_FRACTIONAL_BELIEF_PROPAGATION_PROPERTIES "[maxiter=1000,tol=1e-9,verbose=1,updates=SEQRND,inference=SUMPROD,logdomain=0]"
/// Macro that defines default conditioned loopy belief propagation inference algorithm property string
#define DEFAULT_CONDITIONED_BELIEF_PROPAGATION_PROPERTIES "[maxiter=1000,tol=1e-9,verbose=1,updates=SEQRND,logdomain=0,rec_tol=1e-9,min_max_adj=10,choose=CHOOSE_BBP,recursion=REC_LOGZ,clamp=CLAMP_VAR,bbp_cfn=CFN_GIBBS_B,bbp_props=[maxiter=1000,tol=1e-9,updates=SEQ_BP_REV,damping=0.1]]"


namespace bayesNet {

    namespace inference {

        /// Represents a inference algorithm class
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
                JUNCTION_TREE
            };

            /// Constructor
            Algorithm();

            /// Constructs a inference algorithm using properties defined in file @a filenam
            explicit Algorithm(const std::string &filename);

            /// Constructs a inference algorithm using algorithm type @a alg and @a properties
            explicit Algorithm(const Algorithm::Type &alg, const std::string &properties);

            /// Destructor
            virtual ~Algorithm();

            /// Generates a bayesian inference instance for the provided factor graph @a fg
            void generateInferenceInstance(dai::FactorGraph &fg);

            /// Saves the inference algorithm to file
            void save();

            /// Saves the inference algorithm to @a filename
            void save(const std::string &filename);

            /// Returns the generated inference instance
            dai::InfAlg *getInstance();

            /// Returns the algorithm type
            Algorithm::Type getType() const;

            /// Returns the algorithm properties
            dai::PropertySet getProperties() const;

            /// Returns the algorithm properties
            dai::PropertySet &getProperties();

            /// Returns the filename the algorithm will be saved to
            const std::string &getFilename() const;

        private:
            /// Stores the algorithm type
            Algorithm::Type _algorithm;

            /// Stores the algorithm properties
            dai::PropertySet _inferenceProperties;

            /// Stores the inference instance
            dai::InfAlg *_inferenceInstance;

            /// Stores the filename
            std::string _filename;
        };

        // Stream operator used to write @a algorithm string represenation to iostream @a os
        std::ostream &operator<<(std::ostream &os, const Algorithm &algorithm);
    }
}


#endif //BAYESNET_FRAMEWORK_INFERENCE_GUI_H
