/// @file
/// @brief Defines the node and node view gui elements.


#ifndef BAYESNET_FRAMEWORK_GUI_NODE_H
#define BAYESNET_FRAMEWORK_GUI_NODE_H


#include <bayesnet/state.h>
#include <bayesnet/node.h>

#include <QGraphicsItem>
#include <QRectF>
#include <QPainter>
#include <QPainterPath>
#include <QMenu>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneContextMenuEvent>
#include <QWidget>
#include <QFormLayout>
#include <QLabel>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QComboBox>
#include <QLineEdit>


namespace bayesNet {

    namespace gui {

        /// Represents the node gui element in a graphic scene
        class Node : public QGraphicsItem {
        public:
            /// Constructs a new node with @a name as @a sensor, @a binary
            explicit Node(const QString &name, bool sensor = false, bool binary = false);

            /// Enum for qt type
            enum {
                Type = UserType + 1
            };

            /// Returns type
            virtual int type() const;

            /// Returns bounding rect
            virtual QRectF boundingRect() const;

            /// Returns painter path
            virtual QPainterPath shape() const;

            /// Paints the node
            virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

            /// Updates node's beliefs
            void updateBelief(const state::BayesBelief &belief);

            /// Returns the node's name
            const QString &getName() const;

        private:
            /// Stores the name
            QString _name;

            /// Stores sensor flag
            bool _isSensor;

            /// Stores beliefs
            state::BayesBelief _belief;

            /// Stores the height
            qreal _height;

            /// Stores the color
            QColor _nodeColor;

            static QColor colorFromBelief(const state::BayesBelief &belief);
        };

        /// Represents the detailed view of a node
        class NodeView : public QWidget {
        Q_OBJECT
        public:
            /// Constructs a node view from @a node
            explicit NodeView(bayesNet::Node *node, QWidget *parent = NULL);

        signals:
            /// Signal to set new evidence
            void setEvidence(const std::string &name, size_t state);

            /// Signal to clear evidence
            void clearEvidence(const std::string &name);

            /// Signal to observe new sensor value
            void observe(const std::string &name, double x);

        private slots:
            /// Slot to act on changed evidence
            void propertyEvidenceChanged();

            /// Slot to act on observed value
            void propertyObserveChanged();

        private:
            /// Stores the node
            bayesNet::Node *_node = nullptr;

            /// Stores the layout
            QVBoxLayout* _layout;

            /// Stores the properties box
            QGroupBox* _propertiesBox;

            /// Stores the properties box layout
            QFormLayout *_propertiesLayout;

            /// Stores the label: node name
            QLabel* _propertyLabelName;

            /// Stores the value: node name
            QLabel* _propertyValueName;

            /// Stores the label: node type
            QLabel* _propertyLabelType;

            /// Stores the value: node type
            QLabel* _propertyValueType;

            /// Stores the label: node states
            QLabel* _propertyLabelStates;

            /// Stores the value: node states
            QLabel* _propertyValueStates;

            /// Stores the label: node evidence
            QLabel* _propertyLabelEvidence;

            /// Stores the value: node evidence
            QComboBox* _propertyValueEvidence;

            /// Stores the label: node observe
            QLabel* _propertyLabelObserve;

            /// Stores the value: node observe
            QLineEdit* _propertyValueObserve;

            /// Stores the sensor flag
            bool _isSensor;

            /// Creates the property box
            void createProperties();
        };
    }
}


#endif //BAYESNET_FRAMEWORK_GUI_NODE_H
