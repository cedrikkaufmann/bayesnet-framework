/*  This file is part of libBayesNet
 *
 *  Copyright (c) 2019, The libBayesNet authors. All rights reserved.
 */


/// @file
/// @brief Defines Factor class which represent factors in probability distributions.
/// @author Cedrik Kaufmann
/// @version 1.0


#ifndef BAYESNET_FRAMEWORK_FACTOR_H
#define BAYESNET_FRAMEWORK_FACTOR_H


#include <dai/factor.h>
#include <dai/var.h>
#include <dai/varset.h>


namespace bayesNet {

    /// Represents a (probability) factor.
    /**
     * Factor represents the joint states of its corresponding variables.
     * 
     * The factor class is derived from the dai::Factor class of libDAI
     * and is used as facade for our own bayesian network structure
     * to build a FactorGraph based on libDAI and therefore can be used
     * with the libDAI inference engine.
     */
    class Factor : public dai::Factor {
    public:
    /// @name Constructors and desctructors
    //@{
        /// Constructs factor with number of /a states
        explicit Factor(size_t states = 4);

        /// Constructs a factor based on a conditional VarSet /a vars and number of /a states
        explicit Factor(dai::VarSet &vars, size_t states);

        /// Destructor
        virtual ~Factor();
    //@}

        /// Sets evidence with @a state on the factor
        void setEvidence(size_t state);

        /// Clears all evidence and restores the old factor
        void clearEvidence();

    private:
        /// Stores the original factor
        std::vector<double> _backupFactor;

        // Stores the number of states
        size_t _states;

        // Flag if factor is currently evidence
        bool _isEvidence;

        // Backups the factor
        void backup();

        // Restores the factor
        void restore();
    };
}


#endif //BAYESNET_FRAMEWORK_FACTOR_H
