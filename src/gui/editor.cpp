#include <bayesnet/gui/editor.h>
#include <bayesnet/network.h>

#include <QHBoxLayout>
#include <QList>
#include <QFileDialog>
#include <QMessageBox>


namespace bayesNet {

    namespace gui {

        Editor::Editor() : _nodeView(NULL), _initializationVector(NULL), _network(NULL) {
            init();
        }

        Editor::Editor(const std::string &file) : _nodeView(NULL), _initializationVector(NULL), _network(NULL) {
            init();

            // load data into editor
            _initializationVector = file::InitializationVector::parse(file);
            load();

            // infer and update beliefs
            updateBayesNet();
        }

        void Editor::populateBeliefs() {
            // iterate over nodes and read current beliefs
            for (std::unordered_map<std::string, Node *>::const_iterator it = _nodes.begin(); it != _nodes.end(); it++) {
                state::BayesBelief belief = _network->getBelief((*it).first);
                (*it).second->updateBelief(belief);
            }
        }

        void Editor::newNode(const QString &name, QPointF pos, bool sensor, bool binary) {
            Node *node = new Node(name, sensor, binary);
            node->setPos(pos);
            _scene->addItem(node);
            _nodes[name.toStdString()] = node;
        }

        void Editor::newConnection(const QString &start, const QString &end) {
            Node *startNode = _nodes[start.toStdString()];
            Node *endNode = _nodes[end.toStdString()];

            if (startNode == NULL || endNode == NULL) {
                return;
            }

            Connection *connection = new Connection(startNode, endNode);
            _scene->addItem(connection);
            _connections.push_back(connection);
        }

        void Editor::load() {
            reset();

            std::vector<file::Node *> &nodes = _initializationVector->getNodes();

            for (size_t i = 0; i < nodes.size(); i++) {
                newNode(QString(nodes[i]->getName().c_str()), QPointF(i*10, 100), nodes[i]->isSensor(), nodes[i]->isBinary());
            }

            // add connections for nodes to network
            std::unordered_map<std::string, std::vector<std::string> > &connections = _initializationVector->getConnections();

            for (std::unordered_map<std::string, std::vector<std::string> >::const_iterator it = connections.begin(); it != connections.end(); it++) {
                for (size_t i = 0; i < (*it).second.size(); ++i) {
                    newConnection(QString((*it).first.c_str()), QString((*it).second[i].c_str()));
                }
            }

            _network = new Network();
            _network->load(_initializationVector);
            _network->init();
        }

        void Editor::updateBayesNet() {
            if (_network == NULL) {
                QMessageBox msgBox;
                msgBox.setIcon(QMessageBox::Information);
                msgBox.setText("No network loaded.");
                msgBox.setInformativeText("In order to apply the inference algorithm, a network has to be loaded first.");
                msgBox.exec();
                return;
            }

            _network->run();
            populateBeliefs();
        }

        void Editor::updateNodeView() {
            // delete old node view
            if (_nodeView != NULL) {
                _nodeView->hide();
                centralWidget()->layout()->removeWidget(_nodeView);
                delete _nodeView;
                _nodeView = NULL;
            }

            // get current selection
            QList<QGraphicsItem *> selection = _scene->selectedItems();

            if (selection.length() > 0) {
                // check if selection is node
                Node *selectedNode = dynamic_cast<Node *>(selection[0]);

                if (selectedNode != NULL) {
                    // create node view
                    const QString& nodeName = selectedNode->getName();
                    bayesNet::Node &node = _network->getNode(nodeName.toStdString());
                    bool isSensor = (dynamic_cast<bayesNet::SensorNode *>(&node) != NULL);
                    _nodeView = new NodeView(&node, this);
                    centralWidget()->layout()->addWidget(_nodeView);
                    _nodeView->show();

                    // connect node view to editor to handle changes
                    if (!isSensor) {
                        connect(_nodeView, SIGNAL(setEvidence(const std::string&, size_t)), this, SLOT(setEvidence(const std::string&, size_t)));
                        connect(_nodeView, SIGNAL(clearEvidence(const std::string&)), this, SLOT(clearEvidence(const std::string&)));
                    } else {
                        connect(_nodeView, SIGNAL(observe(const std::string&, double)), this, SLOT(observe(const std::string&, double)));
                    }
                }
            }
        }

        void Editor::setEvidence(const std::string &name, size_t state) {
            // set evidence
            _network->setEvidence(name, state);
        }

        void Editor::clearEvidence(const std::string &name) {
            // clear evidence
            _network->clearEvidence(name);
        }

        void Editor::observe(const std::string &name, double x) {
            // observe sensor data
            _network->observe(name, x);
        }

        void Editor::createActions() {
            // create open network action
            _openNetworkAction = new QAction(tr("Open"), this);
            _openNetworkAction->setStatusTip(tr("Load network from file"));
            _openNetworkAction->setIcon(QIcon("./../../images/open_network.svg"));
            _openNetworkAction->setIconVisibleInMenu(true);
            connect(_openNetworkAction, SIGNAL(triggered()), this, SLOT(openNetworkFileDialog()));

            // create set inference algorithm action
            _setInferenceAlgorithm = new QAction(tr("Algorithm"), this);
            _setInferenceAlgorithm->setStatusTip(tr("FuzzySet inference algorithm"));
            _setInferenceAlgorithm->setIcon(QIcon("./../../images/set_algorithm.svg"));
            _setInferenceAlgorithm->setIconVisibleInMenu(true);
            connect(_setInferenceAlgorithm, SIGNAL(triggered()), this, SLOT(setAlgorithmFileDialog()));

            // create update network action
            _inferNetworkAction = new QAction(tr("Update network"), this);
            _inferNetworkAction->setStatusTip(tr("Apply inference algorithm on network"));
            _inferNetworkAction->setIcon(QIcon("./../../images/update_network.svg"));
            _inferNetworkAction->setIconVisibleInMenu(true);
            connect(_inferNetworkAction, SIGNAL(triggered()), this, SLOT(updateBayesNet()));

            // create zoom in action
            _zoomInAction = new QAction(tr("Zoom in"), this);
            _zoomInAction->setStatusTip(tr("Zoom in"));
            _zoomInAction->setIcon(QIcon("./../../images/zoom_in.svg"));
            _zoomInAction->setIconVisibleInMenu(true);
            connect(_zoomInAction, SIGNAL(triggered()), this, SLOT(zoomIn()));

            // create zoom out action
            _zoomOutAction = new QAction(tr("Zoom out"), this);
            _zoomOutAction->setStatusTip(tr("Zoom out"));
            _zoomOutAction->setIcon(QIcon("./../../images/zoom_out.svg"));
            _zoomOutAction->setIconVisibleInMenu(true);
            connect(_zoomOutAction, SIGNAL(triggered()), this, SLOT(zoomOut()));
        }

        void Editor::createToolbars() {
            // add network toolbar
            _networkAlgorithmToolbar = addToolBar(tr("Network"));
            _networkAlgorithmToolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
            _networkAlgorithmToolbar->addAction(_openNetworkAction);
            _networkAlgorithmToolbar->addAction(_setInferenceAlgorithm);
            _networkAlgorithmToolbar->addAction(_inferNetworkAction);

            // add zoom toolbar
            _networkZoomToolbar = addToolBar(tr("Zoom"));
            _networkZoomToolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
            _networkZoomToolbar->addAction(_zoomInAction);
            _networkZoomToolbar->addAction(_zoomOutAction);
        }

        void Editor::zoomIn() {
            _view->scale(1.2, 1.2);
        }

        void Editor::zoomOut() {
            _view->scale(0.8, 0.8);
        }

        void Editor::init() {
            // create ui and prepare scene
            createActions();
            createToolbars();

            _scene = new DiagramScene(this);
            _scene->setSceneRect(QRectF(0,0, 5000, 5000));

            QHBoxLayout *layout = new QHBoxLayout();
            _view = new QGraphicsView(_scene);
            layout->addWidget(_view);

            QWidget *widget = new QWidget();
            widget->setLayout(layout);

            connect(_scene, SIGNAL(selectionChanged()), this, SLOT(updateNodeView()));

            setCentralWidget(widget);
            setWindowTitle(tr("BayesNet Editor"));
            setUnifiedTitleAndToolBarOnMac(true);
        }

        void Editor::openNetworkFileDialog() {
            QFileDialog prompt;
            prompt.setWindowTitle("Select bayesian network file");
            prompt.setNameFilter("*.bayesnet");
            prompt.setFileMode(QFileDialog::ExistingFile);

            if (prompt.exec()) {
                QStringList files = prompt.selectedFiles();

                if (files.length() > 0) {
                    std::string networkFile = files[0].toStdString();

                    if (_initializationVector != NULL) {
                        delete _initializationVector;
                    }

                    _initializationVector = file::InitializationVector::parse(networkFile);
                    load();
                    updateBayesNet();
                }
            }
        }

        void Editor::setAlgorithmFileDialog() {
            if (_initializationVector != NULL && _network != NULL) {
                QFileDialog prompt;
                prompt.setWindowTitle("Select inference algorithm file");
                prompt.setNameFilter("*.algorithm");
                prompt.setFileMode(QFileDialog::ExistingFile);

                if (prompt.exec()) {
                    QStringList files = prompt.selectedFiles();

                    if (files.length() > 0) {
                        std::string algorithmFile = files[0].toStdString();
                        _initializationVector->setInferenceAlgorithm(algorithmFile);
                        load();
                        updateBayesNet();
                    }
                }
            } else {
                QMessageBox msgBox;
                msgBox.setIcon(QMessageBox::Information);
                msgBox.setText("No network loaded.");
                msgBox.setInformativeText("In order to load a algorithm, a network has to be loaded first.");
                msgBox.exec();
            }
        }

        void Editor::reset() {
            if (_network != NULL) {
                delete _network;

                // remove connections from scene
                for (size_t i = 0; i < _connections.size(); i++) {
                    _scene->removeItem(_connections[i]);
                    delete _connections[i];
                }

                _connections.clear();

                // remove nodes from scene
                for (std::unordered_map<std::string, Node *>::const_iterator it = _nodes.begin(); it != _nodes.end(); it++) {
                    Node *node = it->second;
                    _scene->removeItem(node);
                    delete node;
                }

                _nodes.clear();
            }
        }
    }
}