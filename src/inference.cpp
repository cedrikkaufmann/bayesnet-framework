//
// Created by Cedrik Kaufmann on 2019-06-15.
//

#include <bayesnet/inference.h>
#include <bayesnet/exception.h>

namespace BayesNet {

    dai::PropertySet getInferenceProperties(InferenceProperties inf) {
        switch (inf) {

            case LOOPY_BELIEF_PROPAGATION_SUMPROD: {
                dai::PropertySet opts = dai::PropertySet()
                        ("maxiter", size_t(CONFIG_INFERENCE_MAXIMUM_ITERATIONS))
                        ("tol", dai::Real(CONFIG_INFERENCE_TOLERANCE))
                        ("verbose", size_t(CONFIG_INFERENCE_VERBOSE))
                        ("updates", std::string(CONFIG_LOOPY_BELIEF_PROPAGATION_SUMPROD_UPDATES))
                        ("inference", std::string(CONFIG_LOOPY_BELIEF_PROPAGATION_SUMPROD_INFERENCE))
                        ("logdomain", CONFIG_LOOPY_BELIEF_PROPAGATION_SUMPROD_LOGDOMAIN);

                return opts;
            }

            case LOOPY_BELIEF_PROPAGATION_MAXPROD: {
                dai::PropertySet opts = dai::PropertySet()
                        ("maxiter", size_t(CONFIG_INFERENCE_MAXIMUM_ITERATIONS))
                        ("tol", dai::Real(CONFIG_INFERENCE_TOLERANCE))
                        ("verbose", size_t(CONFIG_INFERENCE_VERBOSE))
                        ("updates", std::string(CONFIG_LOOPY_BELIEF_PROPAGATION_MAXPROD_UPDATES))
                        ("inference", std::string(CONFIG_LOOPY_BELIEF_PROPAGATION_MAXPROD_INFERENCE))
                        ("logdomain", CONFIG_LOOPY_BELIEF_PROPAGATION_MAXPROD_LOGDOMAIN);

                return opts;
            }

            case CONDITIONED_BELIEF_PROPAGATION: {
                dai::PropertySet bbpOpts = dai::PropertySet()
                        ("updates", std::string(CONFIG_CONDITIONED_BELIEF_PROPAGATION_BBP_UPDATES))
                        ("damping", dai::Real(CONFIG_CONDITIONED_BELIEF_PROPAGATION_BBP_DAMPING));

                dai::PropertySet opts = dai::PropertySet()
                        ("maxiter", size_t(CONFIG_INFERENCE_MAXIMUM_ITERATIONS))
                        ("tol", dai::Real(CONFIG_INFERENCE_TOLERANCE))
                        ("verbose", size_t(CONFIG_INFERENCE_VERBOSE))
                        ("updates", std::string(CONFIG_CONDITIONED_BELIEF_PROPAGATION_UPDATES))
                        ("rec_tol", dai::Real(
                                CONFIG_CONDITIONED_BELIEF_PROPAGATION_REC_TOL)) // Tolerance used for controlling recursion depth
                        ("min_max_adj", dai::Real(
                                CONFIG_CONDITIONED_BELIEF_PROPAGATION_MIN_MAX_ADJ)) // Maximum number of levels of recursion
                        ("choose", std::string(CONFIG_CONDITIONED_BELIEF_PROPAGATION_CHOOSE))
                        ("recursion", std::string(CONFIG_CONDITIONED_BELIEF_PROPAGATION_RECURSION))
                        ("clamp", std::string(CONFIG_CONDITIONED_BELIEF_PROPAGATION_CLAMP))
                        ("bbp_props", bbpOpts)
                        ("bbp_cfn", std::string(CONFIG_CONDITIONED_BELIEF_PROPAGATION_BBP_CFN));

                return opts;
            }

            case FRACTIONAL_BELIEF_PROPAGATION: {
                dai::PropertySet opts = dai::PropertySet()
                        ("maxiter", size_t(CONFIG_INFERENCE_MAXIMUM_ITERATIONS))
                        ("tol", dai::Real(CONFIG_INFERENCE_TOLERANCE))
                        ("verbose", size_t(CONFIG_INFERENCE_VERBOSE))
                        ("updates", std::string(CONFIG_FRACTIONAL_BELIEF_PROPAGATION_UPDATES))
                        ("inference", std::string(CONFIG_FRACTIONAL_BELIEF_PROPAGATION_INFERENCE))
                        ("logdomain", CONFIG_FRACTIONAL_BELIEF_PROPAGATION_LOGDOMAIN);

                return opts;
            }

            default:
                throw InferencePropertyNotImplementedException();

        }
    }
}