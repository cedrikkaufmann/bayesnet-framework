//
// Created by Cedrik Kaufmann on 09.09.19.
//

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

        class Node : public QGraphicsItem {
        public:
            explicit Node(const QString &name, bool sensor = false, bool binary = false);

            enum {
                Type = UserType + 1
            };

            virtual int type() const;

            virtual QRectF boundingRect() const;

            virtual QPainterPath shape() const;

            virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

            void updateBelief(const state::BayesBelief &belief);

            const QString &getName() const;

        private:
            QString _name;
            bool _isSensor;
            state::BayesBelief _belief;
            qreal _height;
            QColor _nodeColor;
        };

        class NodeView : public QWidget {
        Q_OBJECT
        public:
            explicit NodeView(bayesNet::Node *node, QWidget *parent = NULL);

            void createProperties();

        signals:
            void setEvidence(const std::string &name, size_t state);
            void clearEvidence(const std::string &name);
            void observe(const std::string &name, double x);

        private slots:
            void propertyEvidenceChanged();
            void propertyObserveChanged();

        private:
            QVBoxLayout* _layout;
            QGroupBox* _propertiesBox;
            QFormLayout *_propertiesLayout;
            QLabel* _propertyLabelName;
            QLabel* _propertyValueName;
            QLabel* _propertyLabelType;
            QLabel* _propertyValueType;
            QLabel* _propertyLabelStates;
            QLabel* _propertyValueStates;
            QLabel* _propertyLabelEvidence;
            QComboBox* _propertyValueEvidence;
            QLabel* _propertyLabelObserve;
            QLineEdit* _propertyValueObserve;

            bayesNet::Node *_node;
            bool _isSensor;
        };
    }
}


#endif //BAYESNET_FRAMEWORK_GUI_NODE_H
