#include <bayesnet/gui/editor.h>
#include <bayesnet/network.h>

#include <QHBoxLayout>


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
            _scene->setSceneRect(QRectF(0,0,5000,5000));

            QHBoxLayout *layout = new QHBoxLayout();
            _view = new QGraphicsView(_scene);
            layout->addWidget(_view);

            QWidget *widget = new QWidget();
            widget->setLayout(layout);

            setCentralWidget(widget);
            setWindowTitle(tr("BayesNet Editor"));
            setUnifiedTitleAndToolBarOnMac(true);
        }

        void Editor::newNode(const QString &name, QPointF pos, bool binary) {
            Node *node = new Node(name, binary);
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

                newNode(QString(nodes[i]->getName().c_str()), QPointF(i*10, 100), nodes[i]->isBinary());
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
    }
}