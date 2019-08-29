//
// Created by Cedrik Kaufmann on 2019-08-16.
//

#ifndef BAYESNET_FRAMEWORK_FUZZY_H
#define BAYESNET_FRAMEWORK_FUZZY_H

#include <vector>

#include <bayesnet/cpt.h>
#include <bayesnet/state.h>

namespace bayesNet {

    namespace fuzzyLogic {

        class MembershipFunction {
        public:
            MembershipFunction();

            virtual ~MembershipFunction();

            virtual double fx(double x) = 0;

            virtual double findMaximum() = 0;
        };

        class Set {
        public:
            explicit Set(size_t states, double tol = 0);

            virtual ~Set();

            void setMembershipFunction(size_t state, MembershipFunction *mf);

            MembershipFunction *getMembershipFunction(size_t state);

            double findMaximum(size_t state);

            std::vector<double> getBeliefs(double x) const;

            double getBelief(double x, size_t state) const;

        private:
            double _nullBeliefTolerance;
            std::vector<MembershipFunction *> _mf;
        };

        class RuleState {
        public:
            explicit RuleState(size_t state, bool binary = false);

            virtual ~RuleState();

            bool isBinary();

            size_t getState();

        private:
            size_t _state;
            bool _binary;
        };

        namespace rules {

            RuleState *get(const state::State &state);

            RuleState *get(const state::StateBinary &state);
        }

        class Rule {
        public:
            explicit Rule(const std::vector<RuleState *> &parentStates, RuleState *state);

            virtual ~Rule();

            std::vector<RuleState *> &getParentStates();

            RuleState &getChildState();

            size_t nrJointStates();

        private:
            std::vector<RuleState *> _parentStates;
            RuleState &_state;
        };

        class RuleSet {
        public:
            RuleSet();

            explicit RuleSet(const std::vector<Rule *> &rules);

            virtual ~RuleSet();

            void addRule(Rule *rule);

            void setRules(const std::vector<Rule *> &rules);

            std::vector<Rule *> &getRules();

            size_t nrJointStates();

        private:
            std::vector<Rule *> _rules;
        };

        class Controller {
        public:
            Controller(const std::vector<Set *> &set, RuleSet *rules, double tolerance = 0);

            virtual ~Controller();

            CPT inferCPT();

        private:
            RuleSet *_rules;
            std::vector<Set *> _fuzzySet;
            double _nullBeliefTolerance;

            std::vector<double> infer(const std::vector<size_t> &states);
        };

        namespace membershipFunctions {

            class Linear : public MembershipFunction {
            public:
                Linear(double fxMin, double fxMax);

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
                Triangle(double begin, double max, double end);

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
                Trapezoid(double x1, double x2, double x3, double x4);

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
                SShape(double a, double b);

                virtual double fx(double x);

                double getMinPos();

                double getMaxPos();

                virtual double findMaximum();

            private:
                double _a;
                double _b;
            };

            class ZShape : public MembershipFunction {
            public:
                ZShape(double a, double b);

                virtual double fx(double x);

                double getMaxPos();

                double getMinPos();

                virtual double findMaximum();

            private:
                SShape _sShape;
            };

            class PiShape : public MembershipFunction {
            public:
                PiShape(double a, double b, double c, double d);

                virtual double fx(double x);

                virtual double findMaximum();

            private:
                SShape _sShape;
                ZShape _zShape;
            };

            class Sigmoidal : public MembershipFunction {
            public:
                Sigmoidal(double a, double c);

                virtual double fx(double x);

                virtual double findMaximum();

            private:
                double _a;
                double _c;
            };

            class Bell : public MembershipFunction {
            public:
                Bell(double a, double b, double c);

                virtual double fx(double x);

                virtual double findMaximum();

            private:
                double _a;
                double _b;
                double _c;
            };

            class Gaussian : public MembershipFunction {
            public:
                Gaussian(double mean, double deviation);

                virtual double fx(double x);

                double getMean();

                virtual double findMaximum();

            private:
                double _mean;
                double _deviation;
            };

            class Gaussian2 : public MembershipFunction {
            public:
                Gaussian2(double meanLeft, double deviationLeft, double meanRight, double deviationRight);

                virtual double fx(double x);

                virtual double findMaximum();

            private:
                Gaussian _left;
                Gaussian _right;
            };
        }
    }
}

#endif //BAYESNET_FRAMEWORK_FUZZY_H
