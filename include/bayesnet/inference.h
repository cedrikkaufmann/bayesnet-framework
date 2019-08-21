//
// Created by Cedrik Kaufmann on 2019-06-12.
//

#ifndef BAYESNET_FRAMEWORK_INFERENCE_H
#define BAYESNET_FRAMEWORK_INFERENCE_H

#include <string>

#include <dai/properties.h>
#include <dai/factorgraph.h>
#include <dai/daialg.h>

#define DEFAULT_JUNCTION_TREE_PROPERTIES "[verbose=1,updates=HUGIN]"
#define DEFAULT_LOOPY_BELIEF_PROPAGATION_PROPERTIES "[maxiter=1000,tol=1e-9,verbose=1,updates=SEQRND,inference=SUMPROD,logdomain=0]"
#define DEFAULT_FRACTIONAL_BELIEF_PROPAGATION_PROPERTIES "[maxiter=1000,tol=1e-9,verbose=1,updates=SEQRND,inference=SUMPROD,logdomain=0]"
#define DEFAULT_CONDITIONED_BELIEF_PROPAGATION_PROPERTIES "[maxiter=1000,tol=1e-9,verbose=1,updates=SEQRND,logdomain=0,rec_tol=1e-9,min_max_adj=10,choose=CHOOSE_BBP,recursion=REC_LOGZ,clamp=CLAMP_VAR,bbp_cfn=CFN_GIBBS_B,bbp_props=[maxiter=1000,tol=1e-9,updates=SEQ_BP_REV,damping=0.1]]"

namespace bayesNet {

    namespace inference {

        enum AlgorithmType {
            LOOPY_BELIEF_PROPAGATION,
            CONDITIONED_BELIEF_PROPAGATION,
            FRACTIONAL_BELIEF_PROPAGATION,
            JUNCTION_TREE
        };

        class Algorithm {
        public:
            Algorithm();

            explicit Algorithm(const std::string &filename);

            explicit Algorithm(const AlgorithmType &alg, const std::string &properties);

            virtual ~Algorithm();

            void generateInferenceInstance(dai::FactorGraph &fg);

            void save();

            void save(const std::string &filename);

            dai::InfAlg *getInstance();

            AlgorithmType getType() const;

            dai::PropertySet getProperties() const;

            dai::PropertySet &getProperties();

            std::string getFilename() const;

        private:
            AlgorithmType _algorithm;
            dai::PropertySet _inferenceProperties;
            dai::InfAlg *_inferenceInstance;
            std::string _filename;
        };

        std::ostream &operator<<(std::ostream &os, const Algorithm &algorithm);
    }
}

#endif //BAYESNET_FRAMEWORK_INFERENCE_GUI_H
