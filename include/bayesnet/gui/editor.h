/// @file
/// @brief Defines the main gui elements to visualize bayesian networks based on libBayesNet.


#ifndef BAYESNET_FRAMEWORK_GUI_EDITOR_H
#define BAYESNET_FRAMEWORK_GUI_EDITOR_H

#include <unordered_map>
#include <string>

#include <bayesnet/file.h>
#include <bayesnet/network.h>
#include <bayesnet/gui/node.h>
#include <bayesnet/gui/connection.h>
#include <bayesnet/gui/diagramscene.h>

#include <QObject>
#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QAction>
#include <QToolBar>


namespace bayesNet {

    namespace gui {

        /// Editor represents the main window for the network gui
        class Editor : public QMainWindow {
        Q_OBJECT
        public:
            /// Constructor
            Editor();

            /// Constructs a new editor instances by loading network from @a file
            Editor(const std::string &file);

            /// Creates a new node with name @a name at @a pos as @a sensor, @a binary
            void newNode(const QString &name, QPointF pos, bool sensor = false, bool binary = false);

            /// Creates a new connection between nodes @a start and @a end
            void newConnection(const QString &start, const QString &end);

        public slots:

            /// Resets the network and graphics scene
            void reset();

            /// Updates the bayesian network and apply inference
            void updateBayesNet();

            /// Sets evidence on node @a name with @a state
            void setEvidence(const std::string &name, size_t state);

            /// Clears the evidence on node @a name
            void clearEvidence(const std::string &name);

            /// Passes through a new observation value @a x for node @a name
            void observe(const std::string &name, double x);

            /// Shows an open dialog to choose network file
            void openNetworkFileDialog();

            /// Shows an open dialog to choose algorithm file
            void setAlgorithmFileDialog();

            /// Zooms in the network
            void zoomIn();

            /// Zooms out the network
            void zoomOut();

        private slots:

            /// Updates the node view
            void updateNodeView();

        private:
            /// Stores graphics scene
            DiagramScene *_scene;

            /// Stores the main view
            QGraphicsView *_view;

            /// Stores the node view
            NodeView *_nodeView;

            /// Stores toolbar for network actions
            QToolBar *_networkAlgorithmToolbar;

            /// Stores toolbar for zoom actions
            QToolBar *_networkZoomToolbar;

            /// Stores open network action
            QAction *_openNetworkAction;

            /// Stores set inference algorithm action
            QAction *_setInferenceAlgorithm;

            /// Stores update network action
            QAction *_inferNetworkAction;

            /// Stores zoom out action
            QAction *_zoomOutAction;

            /// Stores zoom in action
            QAction *_zoomInAction;

            /// Stores gui nodes
            std::unordered_map<std::string, Node *> _nodes;

            /// Stores gui connections
            std::vector<Connection *> _connections;

            /// Stores the initialization vector
            file::InitializationVector *_initializationVector;

            /// Stores bayesian network instance
            Network *_network;

            /// Initializes the basic ui
            void init();

            /// Creates the toolbar actions
            void createActions();

            /// Creates the toolbars
            void createToolbars();

            /// Loads a network from initialization vector and populates its data to the ui
            void load();

            /// Updates the ui with the network's current beliefs
            void populateBeliefs();
        };
    }
}


#endif //BAYESNET_FRAMEWORK_GUI_EDITOR_H