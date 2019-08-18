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

    class MembershipFunction {
    public:
        virtual ~MembershipFunction() {}

        virtual double fx(double x) = 0;
    };

    class FuzzyController : ContinuousVariable {
    public:
        FuzzyController() : _binary(false), _cpt(4), _functions(4) {}

        explicit FuzzyController(bool binary);

        CPT &getCPT(double x);

        void setMF(belief::BeliefState state, MembershipFunction *func);

    private:
        bool _binary;
        CPT _cpt;
        std::vector<MembershipFunction *> _functions;
    };

    namespace membershipFunctions {

        class Linear : public MembershipFunction {
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

        class Triangle : public MembershipFunction {
        public:
            explicit Triangle(double begin, double max, double end) : _begin(begin), _max(max), _end(end), _increasing(_begin, _max), _decreasing(_max, _end) {}

            virtual ~Triangle() {}

            virtual double fx(double x);

        private:
            double _begin;
            double _max;
            double _end;
            Linear _increasing;
            Linear _decreasing;
        };

        class Trapezoid : public MembershipFunction {
        public:
            explicit Trapezoid(double x1, double x2, double x3, double x4) : _increasingBegin(x1),
                                                                             _increasingEnd(x2), _decreasingBegin(x3),
                                                                             _decreasingEnd(x4),
                                                                             _increasingLinear(x1, x2),
                                                                             _decreasingLinear(x3, x4) {}

            virtual ~Trapezoid() {}

            virtual double fx(double x);

        private:
            double _increasingBegin;
            double _increasingEnd;
            double _decreasingBegin;
            double _decreasingEnd;
            Linear _increasingLinear;
            Linear _decreasingLinear;
        };

        class Sigmoidal : public MembershipFunction {
        public:
            explicit Sigmoidal(double a, double c) : _a(a), _c(c) {}

            virtual ~Sigmoidal() {}

            virtual double fx(double x);

        private:
            double _a;
            double _c;
        };

        class Bell : public MembershipFunction {
        public:
            explicit Bell(double a, double b, double c) : _a(a), _b(b), _c(c) {}

            virtual ~Bell() {}

            virtual double fx(double x);

        private:
            double _a;
            double _b;
            double _c;
        };

        class Gaussian : public MembershipFunction {
        public:
            explicit Gaussian(double mean, double deviation) : _mean(mean), _deviation(deviation) {}

            virtual ~Gaussian() {}

            virtual double fx(double x);

        private:
            double _mean;
            double _deviation;
        };

        class Gaussian2 : public MembershipFunction {
        public:
            explicit Gaussian2(double mean1, double deviation1, double mean2, double deviation2) : _mean1(mean1), _mean2(mean2), _deviation1(deviation1), _deviation2(deviation2) {}

            virtual ~Gaussian2() {}

            virtual double fx(double x);

        private:
            double _mean1;
            double _mean2;
            double _deviation1;
            double _deviation2;
        };
    }
}

#endif //BAYESNET_FRAMEWORK_FUZZY_H
