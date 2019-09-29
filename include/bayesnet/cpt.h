/// @file
/// @brief Defines CPT class which represent a conditional probability table of a node in a bayesian network.


#ifndef BAYESNET_FRAMEWORK_CPT_H
#define BAYESNET_FRAMEWORK_CPT_H


#include <vector>

#include <bayesnet/factor.h>


namespace bayesNet {

    /// Represents a conditional probability table
    /** CPT is used to represent a nodes probability, which then is used to build a factor
     *  based on the a nodes conditional variables.
     */
    class CPT {
    public:
        /// Constructor
        CPT();

        /// Destructor
        virtual ~CPT();

        /// Constructs a CPT with size @a jointSize
        explicit CPT(size_t jointSize);

        /// Constructs a CPT from vector @a probabilities
        /// @param probabilities - joint probability vector
        explicit CPT(std::vector<double> probabilities);

        /// Constructs a CPT from probability @a factor
        explicit CPT(const Factor &factor);
    
        /// Returns the joint size of the CPT
        size_t size() const;

        /// Sets the probability of @a index with @a value
        void set(size_t index, double value);

        /// Returns the probability of entry @a index
        double get(size_t index);

        /// Returns the whole CPT as vector
        std::vector<double> &getProbabilities();

    private:
        /// Stores the probabilities
        std::vector<double> _probabilities;

        /// Stores changed flag
        bool _changed;
    };
}


#endif //BAYESNET_FRAMEWORK_CPT_H
