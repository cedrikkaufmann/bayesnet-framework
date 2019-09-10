#include <bayesnet/gui/diagramscene.h>

#include <QGraphicsItem>
#include <iostream>
#include <QAction>
#include <QTransform>
#include <QActionEvent>
#include <iostream>


namespace bayesNet {

    namespace gui {

        DiagramScene::DiagramScene(QObject *parent) : QGraphicsScene(parent) {
            QColor bgColor(0x01, 0x11,0x1e);
            setBackgroundBrush(QBrush(bgColor, Qt::SolidPattern));
        }

        qreal DiagramScene::round(qreal val, int step) {
            int tmp = int(val) + step /2;
            tmp -= tmp % step;
            return qreal(tmp);
        }

        void DiagramScene::drawBackground(QPainter *painter, const QRectF &rect) {
            QColor bgColor(0x0a, 0x0b,0x0e);
            painter->fillRect(rect, QBrush(bgColor, Qt::SolidPattern));

            int step = GRID_STEP;
            painter->setPen(QPen(QColor(0x35, 0x3a, 0x53)));
            // draw horizontal grid
            qreal start = round(rect.top(), step);
            if (start > rect.top()) {
                start -= step;
            }
            for (qreal y = start - step; y < rect.bottom(); ) {
                y += step;
                painter->drawLine(rect.left(), y, rect.right(), y);
            }
            // now draw vertical grid
            start = round(rect.left(), step);
            if (start > rect.left()) {
                start -= step;
            }
            for (qreal x = start - step; x < rect.right(); ) {
                x += step;
                painter->drawLine(x, rect.top(), x, rect.bottom());
            }
        }
    }
}