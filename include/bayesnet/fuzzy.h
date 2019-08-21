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
        MembershipFunction();

        virtual ~MembershipFunction();

        virtual double fx(double x) = 0;

        virtual double findMaximum() = 0;
    };

    class FuzzySet {
    public:
        explicit FuzzySet(size_t states);

        explicit FuzzySet(size_t states, double tol);

        void setMf(size_t state, MembershipFunction *mf);

        MembershipFunction *getMf(size_t state);

        double findMaximum(size_t state);

        std::vector<double> getBeliefs(double x);

    private:
        double _nullBeliefTolerance;
        std::vector<MembershipFunction *> _mf;
    };

    class FuzzyRule {
    public:
        FuzzyRule();

        explicit FuzzyRule(const std::vector<size_t> &parentStates, size_t state);

        void addParentState(size_t state);

        void setParentStates(const std::vector<size_t> &parentStates);

        void setState(size_t state);

        std::vector<size_t> &getParentStates();

        size_t getState();

        size_t nrJointStates();

    private:
        std::vector<size_t> _parentStates;
        size_t _state;
    };

    class FuzzyRuleSet {
    public:
        FuzzyRuleSet();

        explicit FuzzyRuleSet(const std::vector<FuzzyRule *> &rules);

        void addRule(FuzzyRule *rule);

        void setRules(const std::vector<FuzzyRule *> &rules);

        std::vector<FuzzyRule *> &getRules();

    private:
        std::vector<FuzzyRule *> _rules;
    };

    class FuzzyController {
    public:
        FuzzyController();

        CPT &generateCPT(double x);

        void addFuzzySet(FuzzySet *set);

        void setFuzzyRuleSet(FuzzyRuleSet *rules);

    private:
        FuzzyRuleSet *_rules;
        std::vector<FuzzySet *> _fuzzySet;
    };

    namespace membershipFunctions {

        class Linear : public MembershipFunction {
        public:
            explicit Linear(double fxMin, double fxMax);

            virtual double fx(double x);

            virtual double findMaximum();

        private:
            double _m;
            double _b;
            double _fxMin;
            double _fxMax;
        };

        class Triangle : public MembershipFunction {
        public:
            explicit Triangle(double begin, double max, double end);

            virtual double fx(double x);

            virtual double findMaximum();

        private:
            double _begin;
            double _max;
            double _end;
            Linear _increasing;
            Linear _decreasing;
        };

        class Trapezoid : public MembershipFunction {
        public:
            explicit Trapezoid(double x1, double x2, double x3, double x4);

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
            explicit SShape(double a, double b);

            virtual double fx(double x);

            double getA();

            double getB();

            virtual double findMaximum();

        private:
            double _a;
            double _b;
        };

        class ZShape : public MembershipFunction {
        public:
            explicit ZShape(double a, double b);

            virtual double fx(double x);

            double getA();

            double getB();

            virtual double findMaximum();

        private:
            SShape _sShape;
        };

        class PiShape : public MembershipFunction {
        public:
            explicit PiShape(double a, double b, double c, double d);

            virtual double fx(double x);

            virtual double findMaximum();

        private:
            SShape _sShape;
            ZShape _zShape;
        };

        class Sigmoidal : public MembershipFunction {
        public:
            explicit Sigmoidal(double a, double c);

            virtual double fx(double x);

            virtual double findMaximum();

        private:
            double _a;
            double _c;
        };

        class Bell : public MembershipFunction {
        public:
            explicit Bell(double a, double b, double c);

            virtual double fx(double x);

            virtual double findMaximum();

        private:
            double _a;
            double _b;
            double _c;
        };

        class Gaussian : public MembershipFunction {
        public:
            explicit Gaussian(double mean, double deviation);

            virtual double fx(double x);

            double getMean();

            virtual double findMaximum();

        private:
            double _mean;
            double _deviation;
        };

        class Gaussian2 : public MembershipFunction {
        public:
            explicit Gaussian2(double meanLeft, double deviationLeft, double meanRight, double deviationRight);

            virtual double fx(double x);

            virtual double findMaximum();

        private:
            Gaussian _left;
            Gaussian _right;
        };
    }
}

#endif //BAYESNET_FRAMEWORK_FUZZY_H
