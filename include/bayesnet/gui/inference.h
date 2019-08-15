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
#include <QFileDialog>
#include <QPushButton>
#include <QInputDialog>

namespace bayesNet {

    namespace gui {

        class AlgorithmForm : public QFormLayout {
        public:
            explicit AlgorithmForm(inference::Algorithm *algorithm, QWidget *parent = nullptr) : QFormLayout(parent), _algorithm(algorithm) { }

            virtual void saveAlgorithm() = 0;

        protected:
            inference::Algorithm *_algorithm;
            QLabel *_labelVerbose;
            QLabel *_labelUpdates;
            QLabel *_labelInference;
            QCheckBox *_valueVerbose;
            QComboBox *_valueUpdates;
            QComboBox *_valueInference;

            virtual void createLabels() = 0;

            virtual void createInputs() = 0;

            virtual void initFormLayout() = 0;

            virtual void populateData() = 0;
        };

        class JunctionTreeView : public AlgorithmForm {
        public:
            explicit JunctionTreeView(inference::Algorithm *algorithm, QWidget *parent = nullptr);

            virtual void saveAlgorithm();

        protected:
            QLabel *_labelHeuristic;
            QLabel *_labelMaxMem;
            QComboBox *_valueHeuristic;
            QSpinBox *_valueMaxMem;

            virtual void createLabels();

            virtual void createInputs();

            virtual void initFormLayout();

            virtual void populateData();
        };

        class BeliefPropagationView : public AlgorithmForm {
        public:
            explicit BeliefPropagationView(inference::Algorithm *algorithm, QWidget *parent = nullptr);

            virtual void saveAlgorithm();

        protected:
            QLabel *_labelMaxIter;
            QLabel *_labelTol;
            QLabel *_labelLogDomain;
            QLabel *_labelDamping;
            QLabel *_labelMaxTime;
            QSpinBox *_valueMaxIter;
            QLineEdit *_valueTol;
            QCheckBox *_valueLogDomain;
            QLineEdit *_valueMaxTime;
            QLineEdit *_valueDamping;

            virtual void createLabels();

            virtual void createInputs();

            virtual void initFormLayout();

            virtual void populateData();
        };

        class AlgorithmList : public QListWidget {
        Q_OBJECT
        public:
            explicit AlgorithmList(const std::string &path, QWidget *parent = nullptr);

            std::string getFullFilepath(const std::string &file);
            std::string getPath() const { return _path; }

            void populateData();

        public slots:

            void deleteAlgorithmFile();

        private:
            std::string _path;
        };

        class AlgorithmView : public QGroupBox {
        Q_OBJECT
        public:
            explicit AlgorithmView(const std::string &file, QWidget *parent = nullptr);

        public slots:
            void saveAlgorithm();

        private:
            AlgorithmForm *_algorithmForm;
        };

        class InferenceWindow : public QWidget {
        Q_OBJECT
        public:
            explicit InferenceWindow(QWidget *parent = nullptr);

        public slots:

            void algorithmSelectionChanged(QListWidgetItem *item);
            void showNewAlgorithmDialog();

        private:
            QVBoxLayout *_layout;
            QLabel *_label;
            AlgorithmList *_algorithmList;
            AlgorithmView *_algorithmView;
            QFileDialog *_dirPrompt;
            QInputDialog *_newPrompt;
            QGroupBox *_actionButtons;
            QHBoxLayout *_actionButtonLayout;
            QPushButton *_actionButtonSave;
            QPushButton *_actionButtonNew;
            QPushButton *_actionButtonDelete;

            void connectElements();
            void createElements();
        };
    }
}

#endif //BAYESNET_FRAMEWORK_GUI_INFERENCE_H
