#pragma once

#include <QObject>
#include <QWebSocketServer>
#include <QList>

#include <bayesnet/network.h>

namespace bayesServer {

    class Server : public QObject {
        Q_OBJECT

    public:
        explicit Server(quint16 port, QObject* parent = nullptr);
        virtual ~Server();

    signals:
        void closed();
        void listening();

    private slots:
        void onNewConnection();
        void processTextMessage(const QString& message);
        void socketDisconnected();
        void networkChanged();

    private:
        QWebSocketServer* _socket;
        QList<QWebSocket*> _clients;
        bayesNet::Network* _network;
        bool _networkChanged;
    };
}

