/// @file
/// @brief Defines gui elements to visualize bayesian networks based on libBayesNet.


#ifndef BAYESNET_FRAMEWORK_GUI_DIAGRAMSCENE_H
#define BAYESNET_FRAMEWORK_GUI_DIAGRAMSCENE_H

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QGraphicsSceneContextMenuEvent>
#include <QPainter>

#define GRID_STEP 15


namespace bayesNet {

    namespace gui {

        /// Represents the graphics scene
        class DiagramScene : public QGraphicsScene {
        Q_OBJECT
        public:
            /// Constructor
            explicit DiagramScene(QObject *parent = 0);

        protected:
            /// Draws the scene's background grid
            virtual void drawBackground(QPainter *painter, const QRectF &rect);

        private:

            /// Rounds the @a val using @a step
            static qreal round(qreal val, int step);
        };
    }
}

#endif //BAYESNET_FRAMEWORK_DIAGRAMSCENE_H
