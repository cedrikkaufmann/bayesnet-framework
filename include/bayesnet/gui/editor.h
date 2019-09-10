/// @file
/// @brief Defines gui elements to visualize bayesian networks based on libBayesNet.


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


namespace bayesNet {

    namespace gui {

        class Editor : public QMainWindow {
        Q_OBJECT
        public:
            Editor();

            Editor(const std::string &file);

            void newNode(const QString &name, QPointF pos, bool binary = false);

            void newConnection(const QString &start, const QString &end);

            void updateBayesNet();

        private:
            DiagramScene *_scene;
            QGraphicsView *_view;
            std::unordered_map<std::string, Node *> _nodes;
            std::vector<Connection *> _connections;
            Network _network;

            void load(file::InitializationVector *iv);

            void populateBeliefs();

            void init();

            QAction *_addAction();
        };
    }
}

#endif //BAYESNET_FRAMEWORK_GUI_EDITOR_H