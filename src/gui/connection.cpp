#include <bayesnet/gui/connection.h>

#include <qmath.h>
#include <QSizeF>
#include <QPen>

namespace bayesNet {

    namespace gui {

        Connection::Connection(Node *startNode, Node *endNode, QGraphicsItem *parent) {
            setZValue(1);
            _startNode = startNode;
            _endNode = endNode;
            setFlag(QGraphicsItem::ItemIsSelectable, true);
            _color = Qt::white;
            setPen(QPen(_color, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        }

        int Connection::type() const {
            return Type;
        }

        QRectF Connection::boundingRect() const {
            qreal extra = (pen().width() + 20) / 2.0;

            return QRectF(line().p1(), QSizeF(line().p2().x() - line().p1().x(), line().p2().y() - line().p1().y()))
                    .normalized()
                    .adjusted(-extra, -extra, extra, extra);
        }

        QPainterPath Connection::shape() const {
            QPainterPath path = QGraphicsLineItem::shape();
            path.addPolygon(_arrowHead);
            return path;
        }

        void Connection::setColor(const QColor &color) {
            _color = color;
        }

        Node *Connection::startNode() const {
            return _startNode;
        }

        Node *Connection::endNode() const {
            return _endNode;
        }

        void Connection::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
            if (_startNode->collidesWithItem(_endNode))
                return;

            QPen p = pen();
            p.setColor(_color);
            qreal arrowSize = 10;
            painter->setPen(p);
            painter->setBrush(_color);

            qreal offsetX = _startNode->shape().boundingRect().center().x();
            qreal offsetY = _startNode->shape().boundingRect().center().y();
            qreal width = _startNode->shape().boundingRect().width();
            qreal height = _startNode->shape().boundingRect().height();

            QLineF centerLine(mapFromItem(_startNode, offsetX, offsetY), mapFromItem(_endNode, offsetX, offsetY));
            qreal slope = centerLine.dy() / centerLine.dx();

            QPointF destPoint = centerLine.p2();
            QPointF startPoint = centerLine.p1();

            // TODO get intersecting pointØ
            setLine(QLineF(startPoint, destPoint));

            double angle = std::atan2(-line().dy(), line().dx());

            QPointF arrowP1 = destPoint + QPointF(sin(angle - M_PI / 3) * arrowSize,
                                                    cos(angle - M_PI / 3) * arrowSize);
            QPointF arrowP2 = destPoint + QPointF(sin(angle - M_PI + M_PI / 3) * arrowSize,
                                                    cos(angle - M_PI + M_PI / 3) * arrowSize);

            _arrowHead.clear();
            _arrowHead << line().p2() << arrowP1 << arrowP2;

            painter->drawLine(line());
            painter->drawPolygon(_arrowHead);
            if (isSelected()) {
                painter->setPen(QPen(_color, 1, Qt::DashLine));
                QLineF l = line();
                l.translate(0, 4.0);
                painter->drawLine(l);
                l.translate(0,-8.0);
                painter->drawLine(l);
            }
        }
    }
}