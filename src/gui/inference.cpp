//
// Created by Cedrik Kaufmann on 2019-08-14.
//

#include <sstream>

#include <include/bayesnet/gui/inference.h>
#include <bayesnet/util.h>

#include <QApplication>
#include <QPushButton>
#include <QHBoxLayout>
#include <QWidget>

namespace bayesNet {

    namespace gui {

        InferenceWindow::InferenceWindow(QWidget *parent) : QWidget(parent), _algorithmView(nullptr) {
            // Ein Fenster mit einem Titel erzeugen
            setWindowTitle("Inference algorithm tool");

            _label = new QLabel("Available algorithms");
            _algorithmList = new AlgorithmList("../../algorithms/", this);

            QGroupBox *buttonBox = new QGroupBox();
            QHBoxLayout *buttonLayout = new QHBoxLayout();

            QPushButton *buttonSave = new QPushButton("Save");
            QPushButton *buttonNew = new QPushButton("New");
            QPushButton *buttonDelete = new QPushButton("Delete");

            buttonLayout->addWidget(buttonNew);
            buttonLayout->addWidget(buttonSave);
            buttonLayout->addWidget(buttonDelete);
            buttonBox->setLayout(buttonLayout);

            _layout = new QVBoxLayout;
            _layout->addWidget(_label);
            _layout->addWidget(_algorithmList);
            _layout->addWidget(buttonBox);

            setLayout(_layout);

            QWidget::connect(_algorithmList, SIGNAL(itemClicked(QListWidgetItem * )), this,
                             SLOT(onAlgorithmClicked(QListWidgetItem * )));

            setWindowState(Qt::WindowMaximized);
            show();
        }

        void InferenceWindow::onAlgorithmClicked(QListWidgetItem *item) {
            if (_algorithmView != nullptr) {

                _layout->removeWidget(_algorithmView);
                delete _algorithmView;
            }

            _algorithmView = new AlgorithmView(_algorithmList->getFullFilepath(item->text().toStdString()));
            _layout->insertWidget(2, _algorithmView);
        }

        AlgorithmList::AlgorithmList(const std::string &path, QWidget *parent) : QListWidget(parent) {
            _path = path;
            reload();
        }

        void AlgorithmList::reload() {
            // look for available algorithm files;
            std::vector<std::string> algorithmFile;
            utils::readDirectory(_path, algorithmFile);

            // populate algorithm files
            clear();

            for (size_t i = 0; i < algorithmFile.size(); ++i) {

                addItem(QString(algorithmFile[i].c_str()));
            }
        }

        std::string AlgorithmList::getFullFilepath(const std::string &file) {
            std::stringstream ss;
            ss << _path << file;

            return ss.str();
        }

        AlgorithmView::AlgorithmView(const std::string &file, QWidget *parent) : QGroupBox(parent) {
            inference::Algorithm *algorithm = new inference::Algorithm(file);

            switch (algorithm->getType()) {

                case inference::LOOPY_BELIEF_PROPAGATION:
                    setTitle("LOOPY BELIEF PROPAGATION");
                    _layout = new BeliefPropagationView(algorithm);
                    break;

                case inference::CONDITIONED_BELIEF_PROPAGATION:
                    setTitle("CONDITIONED BELIEF PROPAGATION");
                    _layout = new BeliefPropagationView(algorithm);
                    break;

                case inference::FRACTIONAL_BELIEF_PROPAGATION:
                    setTitle("FRACTIONAL BELIEF PROPAGATION");
                    _layout = new BeliefPropagationView(algorithm);
                    break;

                case inference::JUNCTION_TREE:
                    setTitle("JUNCTION TREE");
                    _layout = new JunctionTreeView(algorithm);
                    break;
            }

            setLayout(_layout);
        }



        BeliefPropagationView::BeliefPropagationView(inference::Algorithm *algorithm, QWidget *parent) : QFormLayout(parent), _algorithm(algorithm) {
            createLabels();
            createInputs();
            initFormLayout();
            populateData();
        }

        void BeliefPropagationView::createLabels() {
            _labelVerbose = new QLabel("Verbosity:");
            _labelMaxIter = new QLabel("Maximum number of iterations:");
            _labelMaxTime = new QLabel("Maximum time (in seconds)");
            _labelTol = new QLabel("Tolerance for convergence test:");
            _labelLogDomain = new QLabel("Logarithmic domain:");
            _labelDamping = new QLabel("Damping constant (0.0 means no damping, 1.0 is maximum damping)");
            _labelUpdates = new QLabel("Updates:");
            _labelInference = new QLabel("Inference variant:");
        }

        void BeliefPropagationView::createInputs() {
            _valueMaxIter = new QSpinBox();
            _valueMaxIter->setMinimum(1);
            _valueMaxIter->setMaximum(1000000);
            _valueMaxIter->setSingleStep(100);

            _valueMaxTime = new QLineEdit();

            _valueTol = new QLineEdit();

            _valueDamping = new QLineEdit();

            _valueUpdates = new QComboBox();
            _valueUpdates->addItem("Parallel", QVariant("PARALL"));
            _valueUpdates->addItem("Sequential (fixed sequence)", QVariant("SEQFIX"));
            _valueUpdates->addItem("Sequential (random sequence)", QVariant("SEQRND"));
            _valueUpdates->addItem("Maximum-residual", QVariant("SEQMAX"));

            _valueInference = new QComboBox();
            _valueInference->addItem("Sum-Product", QVariant("SUMPROD"));
            _valueInference->addItem("Max-Product", QVariant("MAXPROD"));

            _valueLogDomain = new QCheckBox();

            _valueVerbose = new QCheckBox();
        }

        void BeliefPropagationView::initFormLayout() {
            addRow(_labelVerbose, _valueVerbose);
            addRow(_labelMaxIter, _valueMaxIter);
            addRow(_labelMaxTime, _valueMaxTime);
            addRow(_labelTol, _valueTol);
            addRow(_labelLogDomain, _valueLogDomain);
            addRow(_labelDamping, _valueDamping);
            addRow(_labelUpdates, _valueUpdates);
            addRow(_labelInference, _valueInference);
        }

        void BeliefPropagationView::populateData() {
            dai::PropertySet properties = _algorithm->getProperties();

            if (properties.hasKey("maxiter")) {

                _valueMaxIter->setValue(properties.getStringAs<int>("maxiter"));
            }

            if (properties.hasKey("maxtime")) {

                _valueMaxTime->setText(QString(boost::any_cast<std::string>(properties.get("maxtime")).c_str()));
            }

            if (properties.hasKey("tol")) {

                _valueTol->setText(QString(boost::any_cast<std::string>(properties.get("tol")).c_str()));
            }

            if (properties.hasKey("damping")) {

                _valueDamping->setText(QString(boost::any_cast<std::string>(properties.get("damping")).c_str()));
            }

            if (properties.hasKey("updates")) {

                int i = _valueUpdates->findData(QVariant(boost::any_cast<std::string>(properties.get("updates")).c_str()));
                _valueUpdates->setCurrentIndex(i);
            }

            if (properties.hasKey("inference")) {

                int i = _valueInference->findData(QVariant(boost::any_cast<std::string>(properties.get("inference")).c_str()));
                _valueInference->setCurrentIndex(i);
            }

            if (properties.hasKey("logdomain")) {

                _valueLogDomain->setChecked(properties.getStringAs<bool>("logdomain"));
            }

            if (properties.hasKey("verbose")) {

                _valueVerbose->setChecked(properties.getStringAs<bool>("verbose"));
            }
        }

        JunctionTreeView::JunctionTreeView(inference::Algorithm *algorithm, QWidget *parent) : QFormLayout(parent), _algorithm(algorithm) {
            createLabels();
            createInputs();
            initFormLayout();
            populateData();
        }

        void JunctionTreeView::createLabels() {
            _labelVerbose = new QLabel("Verbosity:");
            _labelUpdates = new QLabel("Updates:");
            _labelInference = new QLabel("Inference variant:");
            _labelHeuristic = new QLabel("Heuristic to use for constructing the junction tree");
            _labelMaxMem = new QLabel("Maximum memory to use in bytes (0 means unlimited):");
        }

        void JunctionTreeView::createInputs() {
            _valueMaxMem = new QSpinBox();
            _valueMaxMem->setMinimum(0);
            _valueMaxMem->setMaximum(1000000);
            _valueMaxMem->setSingleStep(1);

            _valueUpdates = new QComboBox();
            _valueUpdates->addItem("HUGIN", QVariant("HUGIN"));
            _valueUpdates->addItem("Shafer-Shenoy", QVariant("SHSH"));

            _valueInference = new QComboBox();
            _valueInference->addItem("Sum-Product", QVariant("SUMPROD"));
            _valueInference->addItem("Max-Product", QVariant("MAXPROD"));

            _valueHeuristic = new QComboBox();
            _valueHeuristic->addItem("MINNEIGHBORS", QVariant("MINNEIGHBORS"));
            _valueHeuristic->addItem("MINWEIGHT", QVariant("MINWEIGHT"));
            _valueHeuristic->addItem("MINFILL", QVariant("MINFILL"));
            _valueHeuristic->addItem("WEIGHTEDMINFILL", QVariant("WEIGHTEDMINFILL"));

            _valueVerbose = new QCheckBox();
        }

        void JunctionTreeView::initFormLayout() {
            addRow(_labelVerbose, _valueVerbose);
            addRow(_labelUpdates, _valueUpdates);
            addRow(_labelInference, _valueInference);
            addRow(_labelHeuristic, _valueHeuristic);
            addRow(_labelMaxMem, _valueMaxMem);
        }

        void JunctionTreeView::populateData() {
            dai::PropertySet properties = _algorithm->getProperties();

            if (properties.hasKey("maxmem")) {

                _valueMaxMem->setValue(properties.getStringAs<int>("maxmem"));
            }

            if (properties.hasKey("updates")) {

                int i = _valueUpdates->findData(QVariant(boost::any_cast<std::string>(properties.get("updates")).c_str()));
                _valueUpdates->setCurrentIndex(i);
            }

            if (properties.hasKey("inference")) {

                int i = _valueInference->findData(QVariant(boost::any_cast<std::string>(properties.get("inference")).c_str()));
                _valueInference->setCurrentIndex(i);
            }

            if (properties.hasKey("heuristic")) {

                int i = _valueHeuristic->findData(QVariant(boost::any_cast<std::string>(properties.get("heuristic")).c_str()));
                _valueHeuristic->setCurrentIndex(i);
            }

            if (properties.hasKey("verbose")) {

                _valueVerbose->setChecked(properties.getStringAs<bool>("verbose"));
            }
        }
    }
}
