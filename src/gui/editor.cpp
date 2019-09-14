#include <bayesnet/gui/editor.h>
#include <bayesnet/network.h>

#include <QHBoxLayout>
#include <QList>


namespace bayesNet {

    namespace gui {

        Editor::Editor() {
            init();
        }

        Editor::Editor(const std::string &file) {
            init();

            // load data into editor
            file::InitializationVector *iv = file::InitializationVector::parse(file);
            load(iv);

            // load data into bayesian network
            _network.load(iv);
        }

        void Editor::populateBeliefs() {
            for (std::unordered_map<std::string, Node *>::const_iterator it = _nodes.begin(); it != _nodes.end(); it++) {
                state::BayesBelief belief = _network.getBelief((*it).first);
                (*it).second->updateBelief(belief);
            }
        }

        void Editor::init() {
            _network = Network();
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

        void Editor::load(bayesNet::file::InitializationVector *iv) {
            std::vector<file::Node *> &nodes = iv->getNodes();

            for (size_t i = 0; i < nodes.size(); i++) {

                newNode(QString(nodes[i]->getName().c_str()), QPointF(i*10, 100), nodes[i]->isSensor(), nodes[i]->isBinary());
            }

            // add connections for nodes to network
            std::unordered_map<std::string, std::vector<std::string> > &connections = iv->getConnections();

            for (std::unordered_map<std::string, std::vector<std::string> >::const_iterator it = connections.begin(); it != connections.end(); it++) {
                for (size_t i = 0; i < (*it).second.size(); ++i) {
                    newConnection(QString((*it).first.c_str()), QString((*it).second[i].c_str()));
                }
            }
        }

        void Editor::updateBayesNet() {
            _network.doInference();
            populateBeliefs();
        }

        void Editor::updateNodeView() {

            if (_nodeView != NULL) {
                _nodeView->hide();
                centralWidget()->layout()->removeWidget(_nodeView);
                delete _nodeView;
                _nodeView = NULL;
            }

            QList<QGraphicsItem *> selection = _scene->selectedItems();

            if (selection.length() > 0) {
                Node *selectedNode = dynamic_cast<Node *>(selection[0]);

                if (selectedNode != NULL) {
                    const QString& nodeName = selectedNode->getName();
                    bayesNet::Node *node = _network.getNode(nodeName.toStdString());
                    bool isSensor = (dynamic_cast<bayesNet::SensorNode *>(node) != NULL);
                    _nodeView = new NodeView(node, this);
                    centralWidget()->layout()->addWidget(_nodeView);
                    _nodeView->show();

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
            _network.setEvidence(name, state);
            updateBayesNet();
        }

        void Editor::clearEvidence(const std::string &name) {
            _network.clearEvidence(name);
            updateBayesNet();
        }

        void Editor::observe(const std::string &name, double x) {
            _network.observe(name, x);
            updateBayesNet();
        }
    }
}