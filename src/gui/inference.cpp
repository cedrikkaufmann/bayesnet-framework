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
                algorithm = new inference::Algorithm(inference::Algorithm::LOOPY_BELIEF_PROPAGATION, DEFAULT_LOOPY_BELIEF_PROPAGATION_PROPERTIES);
            } else if (type == "FRACTIONAL BELIEF PROPAGATION") {
                algorithm = new inference::Algorithm(inference::Algorithm::FRACTIONAL_BELIEF_PROPAGATION, DEFAULT_FRACTIONAL_BELIEF_PROPAGATION_PROPERTIES);
            } else if (type == "CONDITIONED BELIEF PROPAGATION") {
                algorithm = new inference::Algorithm(inference::Algorithm::CONDITIONED_BELIEF_PROPAGATION, DEFAULT_CONDITIONED_BELIEF_PROPAGATION_PROPERTIES);
            } else {
                algorithm = new inference::Algorithm(inference::Algorithm::JUNCTION_TREE, DEFAULT_JUNCTION_TREE_PROPERTIES);
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
                _algorithmList = new AlgorithmList(_dirPrompt->directory().absolutePath().toStdString().append("/"), this);
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
                case inference::Algorithm::LOOPY_BELIEF_PROPAGATION:
                    setTitle("LOOPY BELIEF PROPAGATION");
                    _algorithmForm = new BeliefPropagationView(algorithm);
                    break;

                case inference::Algorithm::CONDITIONED_BELIEF_PROPAGATION:
                    setTitle("CONDITIONED BELIEF PROPAGATION");
                    _algorithmForm = new ConditionedBeliefPropagationView(algorithm);
                    break;

                case inference::Algorithm::FRACTIONAL_BELIEF_PROPAGATION:
                    setTitle("FRACTIONAL BELIEF PROPAGATION");
                    _algorithmForm = new BeliefPropagationView(algorithm);
                    break;

                case inference::Algorithm::JUNCTION_TREE:
                    setTitle("JUNCTION TREE");
                    _algorithmForm = new JunctionTreeView(algorithm);
                    break;
            }

            setLayout(_algorithmForm);
        }

        void AlgorithmView::saveAlgorithm() {
            _algorithmForm->saveAlgorithm();
        }


        BeliefPropagationView::BeliefPropagationView(inference::Algorithm *algorithm, QWidget *parent) : AlgorithmForm(algorithm, parent) {
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

        void BeliefPropagationView::saveAlgorithm() {
            dai::PropertySet &properties = _algorithm->getProperties();

            properties.set("verbose", _valueVerbose->isChecked());
            properties.set("logdomain", _valueLogDomain->isChecked());
            properties.set("updates", _valueUpdates->currentData().toString().toStdString());
            properties.set("inference", _valueInference->currentData().toString().toStdString());
            properties.set("maxiter", size_t(_valueMaxIter->value()));

            QString maxTime = _valueMaxTime->text();

            if (maxTime != "") {
                properties.set("maxTime", size_t(maxTime.toInt()));
            } else {
                properties.erase("maxTime");
            }

            QString tol = _valueTol->text();

            if (tol != "") {
                properties.set("tol", dai::Real(tol.toDouble()));
            } else {
                properties.erase("tol");
            }

            QString damping = _valueDamping->text();

            if (damping != "") {
                properties.set("damping", damping.toDouble());
            } else {
                properties.erase("tol");
            }

            _algorithm->save();
        }

        ConditionedBeliefPropagationView::ConditionedBeliefPropagationView(inference::Algorithm *algorithm, QWidget *parent) : AlgorithmForm(algorithm, parent) {
            createLabels();
            createInputs();
            initFormLayout();
            populateData();
        }

        void ConditionedBeliefPropagationView::createLabels() {
            _labelVerbose = new QLabel("Verbosity:");
            _labelMaxIter = new QLabel("Maximum number of iterations:");
            _labelTol = new QLabel("Tolerance for convergence test:");
            _labelRecTol = new QLabel("Tolerance used for controlling recursion depth:");
            _labelUpdates = new QLabel("Updates:");
            _labelClamping = new QLabel("Clamp variables or factors");
            _labelMinMaxAdj = new QLabel("If choose==CHOOSE_BBP and maximum adjoint is less than this value, don't recurse");
            _labelRecursion = new QLabel("Method for deciding when to stop recursing");
            _labelMaxLevels = new QLabel("Maximum number of levels of recursion (recursion fixed)");
            _labelChoose = new QLabel("Heuristic for choosing clamping variable");
            _labelCostFn = new QLabel("Cost function to use for BBP");
            _labelCostFnMaxIter = new QLabel("Cost function maximum iterations");
            _labelCostFnTol = new QLabel("Cost function tolerance");
            _labelCostFnDamping = new QLabel("Cost function damping constant");
            _labelCostFnUpdates = new QLabel("Cost function updates:");
            _labelCostFnVerbose = new QLabel("Cost function verbosity:");
        }

        void ConditionedBeliefPropagationView::createInputs() {
            _valueMaxIter = new QSpinBox();
            _valueMaxIter->setMinimum(1);
            _valueMaxIter->setMaximum(1000000);
            _valueMaxIter->setSingleStep(100);

            _valueMaxLevels = new QSpinBox();
            _valueMaxLevels->setMinimum(1);
            _valueMaxLevels->setMaximum(1000000);
            _valueMaxLevels->setSingleStep(100);

            _valueMinMaxAdj = new QLineEdit();

            _valueCostFnMaxIter = new QSpinBox();
            _valueCostFnMaxIter->setMinimum(1);
            _valueCostFnMaxIter->setMaximum(1000000);
            _valueCostFnMaxIter->setSingleStep(100);

            _valueTol = new QLineEdit();

            _valueRecTol = new QLineEdit();

            _valueCostFnTol = new QLineEdit();

            _valueCostFnDamping = new QLineEdit();

            _valueUpdates = new QComboBox();
            _valueUpdates->addItem("Parallel", QVariant("PARALL"));
            _valueUpdates->addItem("Sequential (fixed sequence)", QVariant("SEQFIX"));
            _valueUpdates->addItem("Sequential (random sequence)", QVariant("SEQRND"));
            _valueUpdates->addItem("Maximum-residual", QVariant("SEQMAX"));

            _valueRecursion = new QComboBox();
            _valueRecursion->addItem("Fixed", QVariant("REC_FIXED"));
            _valueRecursion->addItem("Log-Z", QVariant("REC_LOGZ"));
            _valueRecursion->addItem("BDiff", QVariant("REC_BDIFF"));

            _valueChoose = new QComboBox();
            _valueChoose->addItem("RANDOM", QVariant("CHOOSE_RANDOM"));
            _valueChoose->addItem("MAXENT", QVariant("CHOOSE_MAXENT"));
            _valueChoose->addItem("BBP", QVariant("CHOOSE_BBP"));
            _valueChoose->addItem("BP L1", QVariant("CHOOSE_BP_L1"));
            _valueChoose->addItem("BP CFN", QVariant("CHOOSE_BP_CFN"));

            _valueClamping = new QComboBox();
            _valueClamping->addItem("Var", QVariant("CLAMP_VAR"));
            _valueClamping->addItem("Factor", QVariant("CLAMP_FACTOR"));

            _valueCostFn = new QComboBox();
            _valueCostFn->addItem("GIBBS_B", QVariant("CFN_GIBBS_B"));
            _valueCostFn->addItem("GIBBS_B2", QVariant("CFN_GIBBS_B2"));
            _valueCostFn->addItem("GIBBS_EXP", QVariant("CFN_GIBBS_EXP"));
            _valueCostFn->addItem("GIBBS_B_FACTOR", QVariant("CFN_GIBBS_B_FACTOR"));
            _valueCostFn->addItem("GIBBS_B2_FACTOR", QVariant("CFN_GIBBS_B2_FACTOR"));
            _valueCostFn->addItem("GIBBS_EXP_FACTOR", QVariant("CFN_GIBBS_EXP_FACTOR"));
            _valueCostFn->addItem("MAXENT", QVariant("CFN_VAR_ENT"));
            _valueCostFn->addItem("FACTOR_ENT", QVariant("CFN_FACTOR_ENT"));
            _valueCostFn->addItem("BETHE_ENT", QVariant("CFN_BETHE_ENT"));

            _valueCostFnUpdates = new QComboBox();
            _valueCostFnUpdates->addItem("Fixed sequential updates", QVariant("SEQ_FIX"));
            _valueCostFnUpdates->addItem("Maximum residual updates", QVariant("SEQ_MAX"));
            _valueCostFnUpdates->addItem("Schedule used by BP, but reversed", QVariant("SEQ_BP_REV"));
            _valueCostFnUpdates->addItem("Schedule used by BP", QVariant("SEQ_BP_FWD"));
            _valueCostFnUpdates->addItem("Parallel updates", QVariant("PAR"));

            _valueVerbose = new QCheckBox();

            _valueCostFnVerbose = new QCheckBox();
        }

        void ConditionedBeliefPropagationView::initFormLayout() {
            addRow(_labelVerbose, _valueVerbose);
            addRow(_labelMaxIter, _valueMaxIter);
            addRow(_labelTol, _valueTol);
            addRow(_labelRecTol, _valueRecTol);
            addRow(_labelMaxLevels, _valueMaxLevels);
            addRow(_labelMinMaxAdj, _valueMinMaxAdj);
            addRow(_labelUpdates, _valueUpdates);
            addRow(_labelChoose, _valueChoose);
            addRow(_labelRecursion, _valueRecursion);
            addRow(_labelClamping, _valueClamping);
            addRow(_labelCostFn, _valueCostFn);
            addRow(_labelCostFnVerbose, _valueCostFnVerbose);
            addRow(_labelCostFnMaxIter, _valueCostFnMaxIter);
            addRow(_labelCostFnTol, _valueCostFnTol);
            addRow(_labelCostFnDamping, _valueCostFnDamping);
            addRow(_labelCostFnUpdates, _valueCostFnUpdates);
        }

        void ConditionedBeliefPropagationView::populateData() {
            dai::PropertySet properties = _algorithm->getProperties();

            if (properties.hasKey("maxiter")) {
                _valueMaxIter->setValue(properties.getStringAs<int>("maxiter"));
            }

            if (properties.hasKey("tol")) {
                _valueTol->setText(QString(boost::any_cast<std::string>(properties.get("tol")).c_str()));
            }

            if (properties.hasKey("rec_tol")) {
                _valueRecTol->setText(QString(boost::any_cast<std::string>(properties.get("rec_tol")).c_str()));
            }

            if (properties.hasKey("max_levels")) {
                _valueMaxLevels->setValue(properties.getStringAs<int>("max_levels"));
            }

            if (properties.hasKey("min_max_adj")) {
                _valueMinMaxAdj->setText(QString(boost::any_cast<std::string>(properties.get("min_max_adj")).c_str()));
            }

            if (properties.hasKey("clamp")) {
                int i = _valueClamping->findData(QVariant(boost::any_cast<std::string>(properties.get("clamp")).c_str()));
                _valueUpdates->setCurrentIndex(i);
            }

            if (properties.hasKey("updates")) {
                int i = _valueUpdates->findData(QVariant(boost::any_cast<std::string>(properties.get("updates")).c_str()));
                _valueUpdates->setCurrentIndex(i);
            }

            if (properties.hasKey("choose")) {
                int i = _valueChoose->findData(QVariant(boost::any_cast<std::string>(properties.get("choose")).c_str()));
                _valueChoose->setCurrentIndex(i);
            }

            if (properties.hasKey("recursion")) {
                int i = _valueRecursion->findData(QVariant(boost::any_cast<std::string>(properties.get("recursion")).c_str()));
                _valueRecursion->setCurrentIndex(i);
            }

            if (properties.hasKey("bbp_cfn")) {
                int i = _valueCostFn->findData(QVariant(boost::any_cast<std::string>(properties.get("bbp_cfn")).c_str()));
                _valueCostFn->setCurrentIndex(i);
            }

            if (properties.hasKey("verbose")) {
                _valueVerbose->setChecked(properties.getStringAs<bool>("verbose"));
            }

            if (properties.hasKey("bbp_props")) {
                dai::PropertySet bbpProps(boost::any_cast<std::string>(properties.get("bbp_props")));

                if (bbpProps.hasKey("maxiter")) {
                     _valueCostFnMaxIter->setValue(bbpProps.getStringAs<int>("maxiter"));
                }

                if (bbpProps.hasKey("damping")) {
                     _valueCostFnDamping->setText(QString(boost::any_cast<std::string>(bbpProps.get("damping")).c_str()));
                }

                if (bbpProps.hasKey("tol")) {
                     _valueCostFnTol->setText(QString(boost::any_cast<std::string>(bbpProps.get("tol")).c_str()));
                }

                if (bbpProps.hasKey("updates")) {
                    int i = _valueCostFnUpdates->findData(QVariant(boost::any_cast<std::string>(bbpProps.get("updates")).c_str()));
                    _valueCostFnUpdates->setCurrentIndex(i);
                }

                if (bbpProps.hasKey("verbose")) {
                    _valueCostFnVerbose->setChecked(bbpProps.getStringAs<bool>("verbose"));
                }
            }

        }

        void ConditionedBeliefPropagationView::saveAlgorithm() {
            dai::PropertySet &properties = _algorithm->getProperties();

            properties.set("verbose", _valueVerbose->isChecked());
            properties.set("updates", _valueUpdates->currentData().toString().toStdString());
            properties.set("recursion", _valueRecursion->currentData().toString().toStdString());
            properties.set("choose", _valueChoose->currentData().toString().toStdString());
            properties.set("clamp", _valueClamping->currentData().toString().toStdString());
            properties.set("bbp_cfn", _valueCostFn->currentData().toString().toStdString());
            properties.set("maxiter", size_t(_valueMaxIter->value()));
            properties.set("max_levels", size_t(_valueMaxLevels->value()));

            QString tol = _valueTol->text();

            if (tol != "") {
                properties.set("tol", dai::Real(tol.toDouble()));
            } else {
                properties.erase("tol");
            }

            QString recTol = _valueRecTol->text();

            if (recTol != "") {
                properties.set("rec_tol", dai::Real(recTol.toDouble()));
            } else {
                properties.erase("rec_tol");
            }

            QString minMaxAdj = _valueMinMaxAdj->text();

            if (minMaxAdj != "") {
                properties.set("min_max_adj", dai::Real(minMaxAdj.toDouble()));
            } else {
                properties.erase("min_max_adj");
            }

            dai::PropertySet bbpProps;

            bbpProps.set("maxiter", size_t(_valueCostFnMaxIter->value()));
            bbpProps.set("updates", _valueCostFnUpdates->currentData().toString().toStdString());
            bbpProps.set("verbose", _valueCostFnVerbose->isChecked());
            
            QString bbpTol = _valueCostFnTol->text();

            if (tol != "") {
                bbpProps.set("tol", dai::Real(bbpTol.toDouble()));
            } else {
                bbpProps.erase("tol");
            }

            QString bbpDamping = _valueCostFnDamping->text();

            if (bbpDamping != "") {
                bbpProps.set("damping", dai::Real(bbpDamping.toDouble()));
            }else {
                bbpProps.erase("damping");
            }

            properties.set("bbp_props", bbpProps);

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
