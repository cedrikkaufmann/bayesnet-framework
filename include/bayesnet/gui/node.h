//
// Created by Cedrik Kaufmann on 09.09.19.
//

#ifndef BAYESNET_FRAMEWORK_GUI_NODE_H
#define BAYESNET_FRAMEWORK_GUI_NODE_H


#include <bayesnet/state.h>

#include <QGraphicsItem>
#include <QRectF>
#include <QPainter>
#include <QPainterPath>
#include <QMenu>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneContextMenuEvent>


namespace bayesNet {

    namespace gui {

        class Node : public QGraphicsItem {
        public:
            explicit Node(const QString &name, bool binary = false);

            enum {
                Type = UserType + 1
            };

            virtual int type() const;

            virtual QRectF boundingRect() const;

            virtual QPainterPath shape() const;

            virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

            void updateBelief(const state::BayesBelief &belief);

        private:
            QString _name;
            state::BayesBelief _belief;
            qreal _height;
        };
    }
}


#endif //BAYESNET_FRAMEWORK_GUI_NODE_H
