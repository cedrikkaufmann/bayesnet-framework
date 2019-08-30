//
// Created by Cedrik Kaufmann on 2019-08-14.
//

#include <sstream>
#include <cstdio>

#include <bayesnet/gui/inference.h>
#include <bayesnet/util.h>

#include <QApplication>

namespace bayesNet {

    namespace gui {

        InferenceWindow::InferenceWindow(QWidget *parent) : QWidget(parent), _algorithmView(NULL) {
            setWindowTitle("Inference algorithm tool");
            setFixedSize(800, 800);

            createElements();
            connectElements();

            show();

            _algorithmList->setCurrentRow(0);
        }

        void InferenceWindow::algorithmSelectionChanged(QListWidgetItem *item) {
            if (_algorithmView != NULL) {

                _layout->removeWidget(_algorithmView);
                delete _algorithmView;
            }

            _algorithmView = new AlgorithmView(_algorithmList->getFullFilepath(item->text().toStdString()));
            _layout->insertWidget(2, _algorithmView);

            QWidget::connect(_actionButtonSave, SIGNAL(clicked()), _algorithmView, SLOT(saveAlgorithm()));
        }

        void InferenceWindow::showNewAlgorithmDialog() {
            _newPrompt = new QInputDialog();
            _newPrompt->setWindowTitle("New");
            _newPrompt->setLabelText("Algorithm:");
            QStringList list;
            list.append("JUNCTION TREE");
            list.append("LOOPY BELIEF PROPAGATION");
            list.append("FRACTIONAL BELIEF PROPAGATION");
            list.append("CONDITIONED BELIEF PROPAGATION");
            _newPrompt->setComboBoxItems(list);

            if (!_newPrompt->exec()) {

                delete _newPrompt;
                return;
            }

            std::string type = _newPrompt->textValue().toStdString();
            delete _newPrompt;

            _newPrompt = new QInputDialog();
            _newPrompt->setWindowTitle("New");
            _newPrompt->setLabelText("Filename (without \".algorithm\"):");

            if (!_newPrompt->exec()) {

                delete _newPrompt;
                return;
            }

            std::string filename = _newPrompt->textValue().toStdString();
            delete _newPrompt;

            inference::Algorithm *algorithm;

            if (type == "LOOPY BELIEF PROPAGATION") {

                algorithm = new inference::Algorithm(inference::LOOPY_BELIEF_PROPAGATION,
                                                     DEFAULT_LOOPY_BELIEF_PROPAGATION_PROPERTIES);
            } else if (type == "FRACTIONAL BELIEF PROPAGATION") {

                algorithm = new inference::Algorithm(inference::FRACTIONAL_BELIEF_PROPAGATION,
                                                     DEFAULT_FRACTIONAL_BELIEF_PROPAGATION_PROPERTIES);
            } else if (type == "CONDITIONED BELIEF PROPAGATION") {

                algorithm = new inference::Algorithm(inference::CONDITIONED_BELIEF_PROPAGATION,
                                                     DEFAULT_CONDITIONED_BELIEF_PROPAGATION_PROPERTIES);
            } else {

                algorithm = new inference::Algorithm(inference::JUNCTION_TREE, DEFAULT_JUNCTION_TREE_PROPERTIES);
            }

            algorithm->save(_algorithmList->getFullFilepath(filename.append(".algorithm")));
            delete algorithm;


            std::string path = _algorithmList->getPath();

            _layout->removeWidget(_algorithmList);
            delete _algorithmList;

            _algorithmList = new AlgorithmList(path);
            _layout->insertWidget(1, _algorithmList);

            connectElements();
        }

        void InferenceWindow::connectElements() {
            QWidget::connect(_algorithmList, SIGNAL(currentItemChanged(QListWidgetItem * , QListWidgetItem * )), this,
                             SLOT(algorithmSelectionChanged(QListWidgetItem * )));

            QWidget::connect(_actionButtonDelete, SIGNAL(clicked()), _algorithmList, SLOT(deleteAlgorithmFile()));

            QWidget::connect(_actionButtonNew, SIGNAL(clicked()), this, SLOT(showNewAlgorithmDialog()));
        }

        void InferenceWindow::createElements() {
            _dirPrompt = new QFileDialog();
            _dirPrompt->setWindowTitle("Select inference algorithm file");
            _dirPrompt->setNameFilter("*.algorithm");
            _dirPrompt->setFileMode(QFileDialog::ExistingFile);

            if (_dirPrompt->exec()) {

                _algorithmList = new AlgorithmList(_dirPrompt->directory().absolutePath().toStdString().append("/"),
                                                   this);
                delete _dirPrompt;
            } else {

                exit(1);
            }

            _label = new QLabel("Available algorithms");

            _actionButtons = new QGroupBox();
            _actionButtonLayout = new QHBoxLayout();

            _actionButtonSave = new QPushButton("Save");
            _actionButtonNew = new QPushButton("New");
            _actionButtonDelete = new QPushButton("Delete");

            _actionButtonLayout->addWidget(_actionButtonNew);
            _actionButtonLayout->addWidget(_actionButtonSave);
            _actionButtonLayout->addWidget(_actionButtonDelete);
            _actionButtons->setLayout(_actionButtonLayout);

            _layout = new QVBoxLayout;
            _layout->addWidget(_label);
            _layout->addWidget(_algorithmList);
            _layout->addWidget(_actionButtons);

            setLayout(_layout);
        }

        AlgorithmList::AlgorithmList(const std::string &path, QWidget *parent) : QListWidget(parent) {
            _path = path;
            populateData();
        }

        void AlgorithmList::populateData() {
            // look for available algorithm files;
            std::vector<std::string> algorithmFile;
            utils::readDirectory(_path, algorithmFile);

            // populate algorithm files

            for (size_t i = 0; i < algorithmFile.size(); ++i) {

                addItem(QString(algorithmFile[i].c_str()));
            }
        }

        std::string AlgorithmList::getFullFilepath(const std::string &file) {
            std::stringstream ss;
            ss << _path << file;

            return ss.str();
        }

        void AlgorithmList::deleteAlgorithmFile() {
            int i = currentRow();
            QListWidgetItem *item = currentItem();

            std::string file = getFullFilepath(item->text().toStdString());
            int res = remove(file.c_str());

            if (res == 0) {
                takeItem(i);
            }
        }

        AlgorithmView::AlgorithmView(const std::string &file, QWidget *parent) : QGroupBox(parent) {
            inference::Algorithm *algorithm = new inference::Algorithm(file);

            switch (algorithm->getType()) {

                case inference::LOOPY_BELIEF_PROPAGATION:
                    setTitle("LOOPY BELIEF PROPAGATION");
                    _algorithmForm = new BeliefPropagationView(algorithm);
                    break;

                case inference::CONDITIONED_BELIEF_PROPAGATION:
                    setTitle("CONDITIONED BELIEF PROPAGATION");
                    _algorithmForm = new BeliefPropagationView(algorithm);
                    break;

                case inference::FRACTIONAL_BELIEF_PROPAGATION:
                    setTitle("FRACTIONAL BELIEF PROPAGATION");
                    _algorithmForm = new BeliefPropagationView(algorithm);
                    break;

                case inference::JUNCTION_TREE:
                    setTitle("JUNCTION TREE");
                    _algorithmForm = new JunctionTreeView(algorithm);
                    break;
            }

            setLayout(_algorithmForm);
        }

        void AlgorithmView::saveAlgorithm() {
            std::cout << "Save algorithm" << std::endl;
            _algorithmForm->saveAlgorithm();
        }


        BeliefPropagationView::BeliefPropagationView(inference::Algorithm *algorithm, QWidget *parent) : AlgorithmForm(
                algorithm, parent) {
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

                int i = _valueUpdates->findData(
                        QVariant(boost::any_cast<std::string>(properties.get("updates")).c_str()));
                _valueUpdates->setCurrentIndex(i);
            }

            if (properties.hasKey("inference")) {

                int i = _valueInference->findData(
                        QVariant(boost::any_cast<std::string>(properties.get("inference")).c_str()));
                _valueInference->setCurrentIndex(i);
            }

            if (properties.hasKey("logdomain")) {

                _valueLogDomain->setChecked(properties.getStringAs<bool>("logdomain"));
            }

            if (properties.hasKey("verbose")) {

                _valueVerbose->setChecked(properties.getStringAs<bool>("verbose"));
            }
        }

        void BeliefPropagationView::saveAlgorithm() {
            dai::PropertySet &properties = _algorithm->getProperties();

            properties.set("verbose", _valueVerbose->isChecked());
            properties.set("logdomain", _valueLogDomain->isChecked());
            properties.set("updates", _valueUpdates->currentData().toString().toStdString());
            properties.set("inference", _valueInference->currentData().toString().toStdString());
            properties.set("maxiter", size_t(_valueMaxIter->value()));

            QString maxTime = _valueMaxTime->text();

            if (maxTime != "") {

                properties.set("tol", size_t(maxTime.toInt()));
            }

            QString tol = _valueTol->text();

            if (tol != "") {

                properties.set("tol", dai::Real(tol.toDouble()));
            }

            QString damping = _valueDamping->text();

            if (damping != "") {

                properties.set("damping", damping.toDouble());
            }

            _algorithm->save();
        }

        JunctionTreeView::JunctionTreeView(inference::Algorithm *algorithm, QWidget *parent) : AlgorithmForm(algorithm,
                                                                                                             parent) {
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

                int i = _valueUpdates->findData(
                        QVariant(boost::any_cast<std::string>(properties.get("updates")).c_str()));
                _valueUpdates->setCurrentIndex(i);
            }

            if (properties.hasKey("inference")) {

                int i = _valueInference->findData(
                        QVariant(boost::any_cast<std::string>(properties.get("inference")).c_str()));
                _valueInference->setCurrentIndex(i);
            }

            if (properties.hasKey("heuristic")) {

                int i = _valueHeuristic->findData(
                        QVariant(boost::any_cast<std::string>(properties.get("heuristic")).c_str()));
                _valueHeuristic->setCurrentIndex(i);
            }

            if (properties.hasKey("verbose")) {

                _valueVerbose->setChecked(properties.getStringAs<bool>("verbose"));
            }
        }

        void JunctionTreeView::saveAlgorithm() {
            dai::PropertySet &properties = _algorithm->getProperties();

            properties.set("verbose", _valueVerbose->isChecked());
            properties.set("updates", _valueUpdates->currentData().toString().toStdString());
            properties.set("inference", _valueInference->currentData().toString().toStdString());
            properties.set("heuristic", _valueHeuristic->currentData().toString().toStdString());
            properties.set("maxmem", _valueMaxMem->text().toInt());

            _algorithm->save();
        }
    }
}
