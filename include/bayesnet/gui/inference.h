//
// Created by Cedrik Kaufmann on 2019-08-14.
//

#ifndef BAYESNET_FRAMEWORK_GUI_INFERENCE_H
#define BAYESNET_FRAMEWORK_GUI_INFERENCE_H

#include <vector>
#include <string>

#include <bayesnet/inference.h>

#include <QLabel>
#include <QFormLayout>
#include <QSpinBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QListWidget>
#include <QListWidgetItem>
#include <QGroupBox>

namespace bayesNet {

    namespace gui {

        class JunctionTreeView : public QFormLayout {
        public:
            explicit JunctionTreeView(inference::Algorithm *algorithm, QWidget *parent = nullptr);

        protected:
            inference::Algorithm *_algorithm;
            QLabel *_labelVerbose;
            QLabel *_labelUpdates;
            QLabel *_labelInference;
            QLabel *_labelHeuristic;
            QLabel *_labelMaxMem;
            QCheckBox *_valueVerbose;
            QComboBox *_valueUpdates;
            QComboBox *_valueInference;
            QComboBox *_valueHeuristic;
            QSpinBox *_valueMaxMem;

            void createLabels();

            void createInputs();

            void initFormLayout();

            void populateData();
        };

        class BeliefPropagationView : public QFormLayout {
        public:
            explicit BeliefPropagationView(inference::Algorithm *algorithm, QWidget *parent = nullptr);

        protected:
            inference::Algorithm *_algorithm;
            QLabel *_labelMaxIter;
            QLabel *_labelTol;
            QLabel *_labelVerbose;
            QLabel *_labelUpdates;
            QLabel *_labelInference;
            QLabel *_labelLogDomain;
            QLabel *_labelDamping;
            QLabel *_labelMaxTime;
            QSpinBox *_valueMaxIter;
            QLineEdit *_valueTol;
            QCheckBox *_valueVerbose;
            QComboBox *_valueUpdates;
            QComboBox *_valueInference;
            QCheckBox *_valueLogDomain;
            QLineEdit *_valueMaxTime;
            QLineEdit *_valueDamping;

            void createLabels();

            void createInputs();

            void initFormLayout();

            void populateData();
        };

        class AlgorithmList : public QListWidget {
        public:
            explicit AlgorithmList(const std::string &path, QWidget *parent = nullptr);

            std::string getFullFilepath(const std::string &file);

            void reload();

        private:
            std::string _path;
        };

        class AlgorithmView : public QGroupBox {
        public:
            explicit AlgorithmView(const std::string &file, QWidget *parent = nullptr);

        private:
            QLayout *_layout;
        };

        class InferenceWindow : public QWidget {
        Q_OBJECT
        public:
            explicit InferenceWindow(QWidget *parent = nullptr);

        public slots:

            void onAlgorithmClicked(QListWidgetItem *item);

        private:
            QVBoxLayout *_layout;
            QLabel *_label;
            AlgorithmList *_algorithmList;
            AlgorithmView *_algorithmView;
        };
    }
}

#endif //BAYESNET_FRAMEWORK_GUI_INFERENCE_H
