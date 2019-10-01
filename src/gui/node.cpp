#include <bayesnet/gui/node.h>
#include <sstream>
#include <string>


namespace bayesNet {

    namespace gui {

        Node::Node(const QString &name, bool sensor, bool binary) : _name(name), _isSensor(sensor), _belief(binary) {
            setZValue(2);
            setFlag(QGraphicsItem::ItemIsMovable, true);
            setFlag(QGraphicsItem::ItemIsSelectable, true);
            setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

            if (!sensor) {
                _nodeColor = QColor(0x13, 0x85, 0xee);
            } else {
                _nodeColor = QColor(0x1d, 0xcf, 0x79);
            }

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
            // enable anti-aliasing
            painter->setRenderHint(QPainter::Antialiasing);
            painter->setPen(Qt::NoPen);

            // draw node shape
            QBrush nodeBrush;
            nodeBrush.setStyle(Qt::SolidPattern);
            nodeBrush.setColor(_nodeColor);

            if (isSelected()) {
                painter->setPen(QPen(QColor(0xe4, 0x00, 0x4f), 2));
            } else {
                painter->setPen(QPen(Qt::black, 0));
            }

            painter->setBrush(nodeBrush);
            painter->drawPath(shape());

            // draw name
            QFont font = painter->font() ;
            int pointSize = 18;
            font.setPointSize(pointSize);

            // calculate font measurements
            QString str(_name);
            QFontMetrics fm(font);
            int strW = fm.horizontalAdvance(str);
            int height = fm.height();

            while (strW >= boundingRect().width()) {
                pointSize -= 2;
                font.setPointSize(pointSize);
                fm = QFontMetrics(font);
                strW = fm.horizontalAdvance(str);
                height = fm.height();
            }

            painter->setFont(font);

            // write node name
            double x = boundingRect().width() / 2 - static_cast<double>(strW) / 2 + boundingRect().x();
            painter->setPen(QPen(Qt::white, 1));
            painter->drawText(QPointF(x,height + 10), _name);

            font.setPointSize(18);
            painter->setFont(font);
            fm = QFontMetrics(font);
            height = fm.height();

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
                int trunc = currentBelief * 10000;
                currentBelief = trunc / 10000.0;
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
                std::stringstream ss;

                int position = static_cast<int>(static_cast<unsigned long>(fm.height() + 40) + 25 * i);
                
                double currentBelief = _belief.get(i);
                int trunc = currentBelief * 10000;
                currentBelief = trunc / 10000.0;

                ss << currentBelief * 100 << "%";
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
            _nodeColor = colorFromBelief(belief);
            update();
        }

        const QString &Node::getName() const {
            return _name;
        }

        QColor &Node::colorFromBelief(const state::BayesBelief &belief) {
            size_t maxBeliefIndex = 0;
            double maxBelief = belief.get(0);

            for (size_t j = 1; j < belief.nrStates(); ++j) {
                double newBelief = belief.get(j);

                if (newBelief > maxBelief) {
                    maxBelief = newBelief;
                    maxBeliefIndex = j;
                }
            }

            if (belief.isBinary()) {
                switch (maxBeliefIndex) {
                    case 0: {
                        // FALSE --> RED
                        static QColor *color = new QColor(0xfa, 0x00, 0x1a);
                        return *color;
                    }
                    case 1: {
                        // TRUE --> GREEN
                        static QColor *color = new QColor(0x14, 0x66, 0x22);
                        return *color;
                    }
                }
            }

            switch (maxBeliefIndex) {
                case 0: {
                    // GOOD --> GREEN
                    static QColor *color = new QColor(0x14, 0x66, 0x22);
                    return *color;
                }
                case 1: {
                    // PROBABLY_GOOD --> YELLOW-GREEN
                    static QColor *color = new QColor(0xbd, 0xcd, 0x0d);
                    return *color;
                }
                case 2: {
                    // PROBABLY_BAD --> DARK YELLOW
                    static QColor *color = new QColor(0xe9, 0x89, 0x10);
                    return *color;
                }
                case 3: {
                    // BAD --> RED
                    static QColor *color = new QColor(0xfa, 0x00, 0x1a);
                    return *color;
                }
            }


        }

        NodeView::NodeView(bayesNet::Node *node, QWidget *parent) : _node(node), QWidget(parent) {
            _layout = new QVBoxLayout();

            createProperties();

            _layout->addWidget(_propertiesBox);
            setLayout(_layout);

            _isSensor = (dynamic_cast<bayesNet::SensorNode *>(_node) != NULL);

            if (!_isSensor) {
                connect(_propertyValueEvidence, SIGNAL(currentIndexChanged(int)), this, SLOT(propertyEvidenceChanged()));
            } else {
                connect(_propertyValueObserve, SIGNAL(returnPressed()), this, SLOT(propertyObserveChanged()));
            }
        }

        void NodeView::createProperties() {
            _propertiesLayout = new QFormLayout();
            _propertiesBox = new QGroupBox("Properties");
            _propertiesBox->setLayout(_propertiesLayout);

            _propertyLabelName = new QLabel("Name:");
            _propertyValueName = new QLabel(QString(_node->getName().c_str()));

            _propertyLabelType = new QLabel("Type:");
            _propertyValueType = new QLabel();

            bool isSensor = (dynamic_cast<bayesNet::SensorNode *>(_node) != NULL);

            if (!isSensor) {
                _propertyValueType->setText("Node");

                _propertyLabelEvidence = new QLabel("Evidence:");
                _propertyValueEvidence = new QComboBox();
            } else {
                _propertyValueType->setText("Sensor");

                _propertyLabelObserve = new QLabel("Observe:");
                _propertyValueObserve = new QLineEdit();
            }

            _propertyLabelStates = new QLabel("States:");
            std::string states = std::to_string(_node->nrStates());
            _propertyValueStates = new QLabel(QString(states.c_str()));


            if (!isSensor && _node->isBinary()) {
                _propertyValueStates->setText("2");

                _propertyValueEvidence->addItem("None", QVariant(-1));
                _propertyValueEvidence->addItem("FALSE", QVariant(0));
                _propertyValueEvidence->addItem("TRUE", QVariant(1));
            } else if(!isSensor) {
                _propertyValueStates->setText("4");

                _propertyValueEvidence->addItem("None", QVariant(-1));
                _propertyValueEvidence->addItem("GOOD", QVariant(0));
                _propertyValueEvidence->addItem("PROBABLY_GOOD", QVariant(1));
                _propertyValueEvidence->addItem("PROBABLY_BAD", QVariant(2));
                _propertyValueEvidence->addItem("BAD", QVariant(3));
            }

            int index = 0;

            if (!isSensor && _node->isEvidence()) {
                 index = static_cast<int>(_node->evidenceState() + 1);
            }

            if (!isSensor) {
                _propertyValueEvidence->setCurrentIndex(index);
            }

            _propertiesLayout->addRow(_propertyLabelName, _propertyValueName);
            _propertiesLayout->addRow(_propertyLabelType, _propertyValueType);
            _propertiesLayout->addRow(_propertyLabelStates, _propertyValueStates);

            if (!isSensor) {
                _propertiesLayout->addRow(_propertyLabelEvidence, _propertyValueEvidence);
            } else {
                _propertiesLayout->addRow(_propertyLabelObserve, _propertyValueObserve);
            }
        }

        void NodeView::propertyEvidenceChanged() {
            bool ok;
            int data = _propertyValueEvidence->currentData().toInt(&ok);

            if (ok) {
                if (data == -1) {
                    emit clearEvidence(_node->getName());
                } else {
                    size_t state = static_cast<size_t>(data);
                    emit setEvidence(_node->getName(), state);
                }
            }
        }

        void NodeView::propertyObserveChanged() {
            bool ok;
            double observeValue = _propertyValueObserve->text().toDouble(&ok);

            if (ok) {
                emit observe(_node->getName(), observeValue);
            }
        }
    }
}