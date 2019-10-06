/// @file
/// @brief Defines Factor class which represents a factor in a factor graph.


#ifndef BAYESNET_FRAMEWORK_FACTOR_H
#define BAYESNET_FRAMEWORK_FACTOR_H


#include <dai/factor.h>
#include <dai/var.h>
#include <dai/varset.h>


namespace bayesNet {

    /// Represents a (probability) factor.
    /**
     * Factor represents the joint probability of its corresponding variables.
     * 
     * The factor class is derived from the dai::Factor class of libDAI
     * and is used as facade for our own bayesian network structure
     * to build a FactorGraph based on libDAI and therefore can be used
     * with the libDAI inference engine.
     */
    class Factor : public dai::Factor {
    public:
        /// Constructs factor with number of /a states
        explicit Factor(size_t states = 4);

        /// Constructs a factor based on a conditional VarSet /a vars and number of /a states
        explicit Factor(dai::VarSet &vars, size_t states);

        /// Destructor
        virtual ~Factor();

        /// Sets evidence with @a state on the factor
        void setEvidence(size_t state);

        /// Clears all evidence and restores the old factor
        void clearEvidence();

        /// Returns flag if factor is currently evidence
        bool isEvidence() const;

        /// Returns the current evidence state
        size_t evidenceState() const;

    private:
        /// Stores the original factor
        std::vector<double> _backupFactor;

        // Stores the number of states
        size_t _states;

        // Flag if factor is currently evidence
        bool _isEvidence;

        /// Stores the current evidence state, -1 if no evidence
        size_t _evidenceState;

        // Backups the factor
        void backup();

        // Restores the factor
        void restore();
    };
}


#endif //BAYESNET_FRAMEWORK_FACTOR_H
