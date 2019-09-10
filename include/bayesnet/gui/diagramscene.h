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

        class DiagramScene : public QGraphicsScene {
        Q_OBJECT
        public:
            explicit DiagramScene(QObject *parent = 0);

        protected:
            virtual void drawBackground(QPainter *painter, const QRectF &rect);
           //virtual void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);

        private:

            static qreal round(qreal val, int step);
        };
    }
}

#endif //BAYESNET_FRAMEWORK_DIAGRAMSCENE_H
