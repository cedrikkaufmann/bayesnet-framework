//
// Created by Cedrik Kaufmann on 2019-08-16.
//

#ifndef BAYESNET_FRAMEWORK_FUZZY_H
#define BAYESNET_FRAMEWORK_FUZZY_H

#include <vector>

#include <bayesnet/cpt.h>
#include <bayesnet/state.h>

namespace bayesNet {

    class MembershipFunction {
    public:
        virtual ~MembershipFunction() {}

        virtual double fx(double x) = 0;

        virtual double findMaximum();
    };

    class FuzzySet {
    public:
        explicit FuzzySet(size_t states) : _nullBeliefTolerance(0), _mf(states) {}

        explicit FuzzySet(size_t states, double tol) : _nullBeliefTolerance(tol), _mf(states) {}

        void setMf(size_t state, MembershipFunction *mf) { _mf[state] = mf; }

        MembershipFunction *getMf(size_t state) { return _mf[state]; }

        double findMaximum(size_t state);

        std::vector<double> getBeliefs(double x);

    private:
        double _nullBeliefTolerance;
        std::vector<MembershipFunction *> _mf;
    };

    class FuzzyRule {
    public:
        FuzzyRule() {}

        explicit FuzzyRule(const std::vector<size_t> &parentStates, size_t state) : _state(
                state) { _parentStates = parentStates; }

        void addParentState(size_t state);

        void setParentStates(const std::vector<size_t> &parentStates) { _parentStates = parentStates; }

        void setState(size_t state) { _state = state; }

        std::vector<size_t> &getParentStates() { return _parentStates; }

        size_t getState() { return _state; }

        size_t nrJointStates();

    private:
        std::vector<size_t> _parentStates;
        size_t _state;
    };

    class FuzzyRuleSet {
    public:
        FuzzyRuleSet() {}

        explicit FuzzyRuleSet(const std::vector<FuzzyRule *> &rules) { _rules = rules; }

        void addRule(FuzzyRule *rule);

        void setRules(const std::vector<FuzzyRule *> &rules) { _rules = rules; }

        std::vector<FuzzyRule *> &getRules() { return _rules; }

    private:
        std::vector<FuzzyRule *> _rules;
    };

    class FuzzyController {
    public:
        FuzzyController() {}

        CPT &generateCPT(double x);

        void addFuzzySet(FuzzySet *set);

        void setFuzzyRuleSet(FuzzyRuleSet *rules) { _rules = rules; }

    private:
        FuzzyRuleSet *_rules;
        std::vector<FuzzySet *> _fuzzySet;
    };

    namespace membershipFunctions {

        class Linear : public MembershipFunction {
        public:
            explicit Linear(double fxMin, double fxMax);

            virtual ~Linear() {}

            virtual double fx(double x);

            virtual double findMaximum() { return _fxMax; }

        private:
            double _m;
            double _b;
            double _fxMin;
            double _fxMax;
        };

        class Triangle : public MembershipFunction {
        public:
            explicit Triangle(double begin, double max, double end) : _begin(begin), _max(max), _end(end),
                                                                      _increasing(_begin, _max),
                                                                      _decreasing(_end, _max) {}

            virtual ~Triangle() {}

            virtual double fx(double x);

            virtual double findMaximum() { return _max; }

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
                                                                             _decreasingLinear(x4, x3) {}

            virtual ~Trapezoid() {}

            virtual double fx(double x);

            virtual double findMaximum();

        private:
            double _increasingBegin;
            double _increasingEnd;
            double _decreasingBegin;
            double _decreasingEnd;
            Linear _increasingLinear;
            Linear _decreasingLinear;
        };

        class SShape : public MembershipFunction {
        public:
            explicit SShape(double a, double b) : _a(a), _b(b) {}

            virtual ~SShape() {}

            virtual double fx(double x);

            double getA() { return _a; }

            double getB() { return _b; }

        private:
            double _a;
            double _b;
        };

        class ZShape : public MembershipFunction {
        public:
            explicit ZShape(double a, double b) : _sShape(a, b) {}

            virtual ~ZShape() {}

            virtual double fx(double x);

            double getA() { return _sShape.getA(); }

            double getB() { return _sShape.getB(); }

        private:
            SShape _sShape;
        };

        class PiShape : public MembershipFunction {
        public:
            explicit PiShape(double a, double b, double c, double d) : _sShape(a, b), _zShape(c, d) {}

            virtual ~PiShape() {}

            virtual double fx(double x);

        private:
            SShape _sShape;
            ZShape _zShape;
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

            double getMean() { return _mean; }

            virtual double findMaximum() { return _mean; }

        private:
            double _mean;
            double _deviation;
        };

        class Gaussian2 : public MembershipFunction {
        public:
            explicit Gaussian2(double meanLeft, double deviationLeft, double meanRight, double deviationRight) : _left(
                    meanLeft, deviationLeft), _right(meanRight, deviationRight) {}

            virtual ~Gaussian2() {}

            virtual double fx(double x);

            virtual double findMaximum();

        private:
            Gaussian _left;
            Gaussian _right;
        };
    }
}

#endif //BAYESNET_FRAMEWORK_FUZZY_H
