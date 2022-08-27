#include <bayesnet/gui/connection.h>

#include <qmath.h>
#include <QSizeF>
#include <QPen>

namespace bayesNet {

    namespace gui {

        Connection::Connection(Node *startNode, Node *endNode, QGraphicsItem *parent) : QGraphicsLineItem(parent) {
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
            Q_UNUSED(option);
            Q_UNUSED(widget);

            if (_startNode->collidesWithItem(_endNode))
                return;

            // enable anti-aliasing
            painter->setRenderHint(QPainter::Antialiasing);

            QPen p = pen();
            p.setColor(_color);
            qreal arrowSize = 10;
            painter->setPen(p);
            painter->setBrush(_color);

            qreal offsetXStart = _startNode->shape().boundingRect().center().x();
            qreal offsetYStart = _startNode->shape().boundingRect().center().y();
            qreal offsetXEnd = _endNode->shape().boundingRect().center().x();
            qreal offsetYEnd = _endNode->shape().boundingRect().center().y();
            qreal widthStart = _startNode->shape().boundingRect().width();
            qreal heightStart = _startNode->shape().boundingRect().height();
            qreal widthEnd = _endNode->shape().boundingRect().width();
            qreal heightEnd = _endNode->shape().boundingRect().height();

            // calculate connection line from center to center
            QLineF centerLine(mapFromItem(_startNode, offsetXStart, offsetYStart), mapFromItem(_endNode, offsetXEnd, offsetYEnd));
            qreal slope = centerLine.dy() / centerLine.dx();
            qreal invSlope = 1 / slope;

            // calculate intersection point between line and start/end rectangle
            QPointF destPoint;
            QPointF startPoint;

            qreal tmp1 = heightEnd / 2.0;
            qreal tmp2 = slope * (widthEnd / 2.0);

            if (-tmp1 <= tmp2 && tmp2 <= tmp1) {
                if (centerLine.p1().x() > centerLine.p2().x()) {
                    // dest rect right edge
                    destPoint.setX(_endNode->pos().x() + widthEnd + 1);
                    destPoint.setY(centerLine.p2().y() + slope * offsetXEnd);
                } else {
                    // dest rect left edge
                    destPoint.setX(_endNode->pos().x() - 1);
                    destPoint.setY(centerLine.p2().y() - slope * offsetXEnd);
                }
            }

            tmp1 = widthEnd / 2.0;
            tmp2 = (heightEnd / 2.0) / slope;

            if (-tmp1 <= tmp2 && tmp2 <= tmp1) {
                if (centerLine.p1().y() < centerLine.p2().y()) {
                    // dest rect top edge
                    destPoint.setX(centerLine.p2().x() - invSlope * offsetYEnd);
                    destPoint.setY(_endNode->pos().y() - 1);
                } else {
                    // dest rect bottom edge
                    destPoint.setX(centerLine.p2().x() + invSlope * offsetYEnd);
                    destPoint.setY(_endNode->pos().y() + heightEnd + 1);
                }
            }

            tmp1 = heightStart / 2.0;
            tmp2 = slope * (widthStart / 2.0);

            if (-tmp1 <= tmp2 && tmp2 <= tmp1) {
                if (centerLine.p1().x() < centerLine.p2().x()) {
                    // start rect right edge
                    startPoint.setX(_startNode->pos().x() + widthStart);
                    startPoint.setY(centerLine.p1().y() + slope * offsetXStart);
                } else {
                    // start rect left edge
                    startPoint.setX(_startNode->pos().x());
                    startPoint.setY(centerLine.p1().y() - slope * offsetXStart);
                }
            }

            tmp1 = widthStart / 2.0;
            tmp2 = (heightStart / 2.0) / slope;

            if (-tmp1 <= tmp2 && tmp2 <= tmp1) {
                if (centerLine.p1().y() > centerLine.p2().y()) {
                    // start rect top edge
                    startPoint.setX(centerLine.p1().x() - invSlope * offsetYStart);
                    startPoint.setY(_startNode->pos().y());
                } else {
                    // start rect bottom edge
                    startPoint.setX(centerLine.p1().x() + invSlope * offsetYStart);
                    startPoint.setY(_startNode->pos().y() + heightStart);
                }
            }

            // set new line with using intersection points
            setLine(QLineF(startPoint, destPoint));

            // setup arrow for dest
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