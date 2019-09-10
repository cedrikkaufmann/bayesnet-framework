#ifndef BAYESNET_FRAMEWORK_GUI_CONNECTION_H
#define BAYESNET_FRAMEWORK_GUI_CONNECTION_H


#include <bayesnet/gui/node.h>

#include <QGraphicsItem>
#include <QGraphicsLineItem>
#include <QRectF>
#include <QPainter>
#include <QPainterPath>
#include <QStyleOptionGraphicsItem>
#include <QWidget>


namespace bayesNet {

    namespace gui {

        class Connection : public QGraphicsLineItem {
        public:
            enum {
                Type = UserType + 3
            };

            Connection(Node *startNode, Node *endNode, QGraphicsItem *parent = NULL);

            virtual int type() const;

            virtual QRectF boundingRect() const;

            virtual QPainterPath shape() const;

            void setColor(const QColor &color);

            Node *startNode() const;

            Node *endNode() const;

        protected:
            virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = NULL);

        private:
            Node *_startNode;
            Node *_endNode;
            QColor _color;
            QPolygonF _arrowHead;
        };
    }
}


#endif //BAYESNET_FRAMEWORK_GUI_CONNECTION_H
