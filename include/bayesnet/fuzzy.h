//
// Created by Cedrik Kaufmann on 2019-08-16.
//

#ifndef BAYESNET_FRAMEWORK_FUZZY_H
#define BAYESNET_FRAMEWORK_FUZZY_H

#include <vector>

#include <bayesnet/continuous.h>
#include <bayesnet/cpt.h>
#include <bayesnet/state.h>

namespace bayesNet {

    class FuzzyFunction {
    public:
        virtual ~FuzzyFunction() {}

        virtual double fx(double x) = 0;
    };

    class FuzzyController : ContinuousVariable {
    public:
        FuzzyController() : _binary(false), _cpt(4), _functions(4) {}

        explicit FuzzyController(bool binary);

        CPT &getCPT(double x);

        void setFunction(belief::BeliefState state, FuzzyFunction *func);

    private:
        bool _binary;
        CPT _cpt;
        std::vector<FuzzyFunction *> _functions;
    };

    namespace fuzzyFunctions {

        class Linear : public FuzzyFunction {
        public:
            explicit Linear(double fxMin, double fxMax);

            virtual ~Linear() {}

            virtual double fx(double x);

        private:
            double _m;
            double _b;
            double _fxMin;
            double _fxMax;
        };

        class Gaussian : public FuzzyFunction {
        public:
            explicit Gaussian(double mean, double deviation) : _mean(mean), _deviation(deviation) {}

            virtual ~Gaussian() {}

            virtual double fx(double x);

        private:
            double _mean;
            double _deviation;
        };
    }
}

#endif //BAYESNET_FRAMEWORK_FUZZY_H
