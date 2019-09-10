#include <bayesnet/gui/node.h>
#include <sstream>


namespace bayesNet {

    namespace gui {

        Node::Node(const QString &name, bool binary) : _name(name), _belief(binary) {
            setZValue(2);
            setFlag(QGraphicsItem::ItemIsMovable, true);
            setFlag(QGraphicsItem::ItemIsSelectable, true);
            setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

            if (binary) {
                _height = 100;
            } else {
                _height = 150;
            }
        }

        QRectF Node::boundingRect() const {
            return QRectF(0, 0, 200, _height);
        }

        QPainterPath Node::shape() const {
            QPainterPath path;
            path.addRoundedRect(QRectF(0, 0, 200, _height), 10, 10);
            return path;
        }

        void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
            painter->setRenderHint(QPainter::Antialiasing);
            painter->setPen(Qt::NoPen);

            QBrush nodeBrush;
            nodeBrush.setStyle(Qt::SolidPattern);

            // draw node shape
            if (option->state & QStyle::State_Sunken) {
                nodeBrush.setColor(QColor(0x13, 0x85, 0xee));
                painter->setOpacity(0.9);
                painter->setPen(QPen(QColor(0xe4, 0x00, 0x4f), 2));
            } else {
                painter->setOpacity(1);
                nodeBrush.setColor(QColor(0x13, 0x85, 0xee));
                painter->setPen(QPen(Qt::black, 0));
            }

            painter->setBrush(nodeBrush);
            painter->drawPath(shape());

            // draw name
            QFont font = painter->font() ;
            font.setPointSize(18);
            painter->setFont(font);

            // calculate font measurements
            QString str(_name);
            QFontMetrics fm(font);
            int strW = fm.horizontalAdvance(str);
            int height = fm.height();

            // write node name
            double x = boundingRect().width() / 2 - static_cast<double>(strW) / 2 + boundingRect().x();
            painter->setPen(QPen(Qt::white, 1));
            painter->drawText(QPointF(x,height + 10), _name);

            // draw beliefs bars
            nodeBrush.setColor(Qt::lightGray);
            painter->setPen(QPen(QColor(0xe7, 0x59, 0x4e), 1));
            painter->setBrush(nodeBrush);

            size_t states = _belief.nrStates();
            qreal barSize = boundingRect().width() - 40;

            for (size_t i = 0; i < states; ++i) {
                int position = static_cast<int>(static_cast<unsigned long>(fm.height() + 20) + 25 * i);
                painter->drawRect(20, position, barSize, 20);
            }

            nodeBrush.setColor(QColor(0xfe, 0xc1, 0x50));
            painter->setBrush(nodeBrush);

            for (size_t i = 0; i < states; ++i) {
                double currentBelief = _belief.get(i);
                int indicatorSize = static_cast<int>(currentBelief * barSize);
                int position = static_cast<int>(static_cast<unsigned long>(fm.height() + 20) + 25 * i);
                painter->drawRect(20, position, indicatorSize, 20);
            }

            // draw belief text
            font = painter->font() ;
            font.setPointSize(12);
            painter->setFont(font);
            fm = QFontMetrics(font);
            height = fm.height();

            // calculate font measurements
            for (size_t i = 0; i < states; ++i) {
                int position = static_cast<int>(static_cast<unsigned long>(fm.height() + 40) + 25 * i);
                std::stringstream ss;
                ss << _belief.get(i) * 100 << "%";
                str = QString(ss.str().c_str());
                int strW = fm.horizontalAdvance(str);

                double x = boundingRect().width() / 2 - static_cast<double>(strW) / 2 + boundingRect().x();
                painter->setPen(QPen(Qt::black, 1));
                painter->drawText(QPointF(x, position), str);
            }

        }

        int Node::type() const {
            return Type;
        }

        void Node::updateBelief(const state::BayesBelief &belief) {
            _belief = belief;
            update();
        }
    }
}