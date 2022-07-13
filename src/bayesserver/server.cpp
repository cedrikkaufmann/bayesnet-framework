#include <bayesserver/server.h>

#include <QWebSocket>
#include <QJsonDocument>
#include <QJsonObject>

namespace bayesServer {

    Server::Server(quint16 port, QObject* parent) : _network(nullptr), QObject(parent) {
        _socket = new QWebSocketServer("BayesServer", QWebSocketServer::NonSecureMode, this);

        if (_socket->listen(QHostAddress::Any, port)) {
            connect(_socket, &QWebSocketServer::newConnection, this, &Server::onNewConnection);
            connect(_socket, &QWebSocketServer::closed, this, &Server::closed);
            
            emit listening();
        }
    }

    Server::~Server() {
        if (_network != nullptr) {
            delete _network;
        }
    }

    void Server::onNewConnection() {
        // accept new incoming connection
        QWebSocket* socket = _socket->nextPendingConnection();
	    connect(socket, &QWebSocket::textMessageReceived, this, &Server::processTextMessage);
        connect(socket, &QWebSocket::disconnected, this, &Server::socketDisconnected);
    }

    void Server::processTextMessage(const QString& message) {
        // parse message as json
	    QJsonDocument jsonDoc = QJsonDocument::fromJson(message.toUtf8());

        // parse as json object
	    QJsonObject json = jsonDoc.object();

        // read action
        QString action = json["action"].toString();
        // read payload
        QJsonObject payload = json["payload"].toObject(); 

        // handle actions
        if (action == "load_network") {
            QString file = payload["file"].toString();

            // delete old network if exists
            if (_network != nullptr) {
                delete _network;
            }

            // create network instance from given file
            try {
                _network = new bayesNet::Network(file.toStdString());
                _network->init();
                _network->run();
                _networkChanged = false;
            } catch(const std::exception& e) {
                QWebSocket* client = qobject_cast<QWebSocket*>(sender());
                client->sendTextMessage(e.what());
            }

            return;
        }

        if (action == "set_evidence") {
            QString node = payload["node"].toString();
            size_t state = payload["state"].toInt();
            
            if (_network != nullptr) {
                // create network instance from given file
                try {
                    _network->setEvidence(node.toStdString(), state);
                    networkChanged();
                } catch(const std::exception& e) {
                    QWebSocket* client = qobject_cast<QWebSocket*>(sender());
                    client->sendTextMessage(e.what());
                }
            }

            return;
        }

        if (action == "clear_evidence") {
            QString node = payload["node"].toString();
            
            if (_network != nullptr) {
                // create network instance from given file
                try {
                    _network->clearEvidence(node.toStdString());
                    networkChanged();
                } catch(const std::exception& e) {
                    QWebSocket* client = qobject_cast<QWebSocket*>(sender());
                    client->sendTextMessage(e.what());
                }
            }

            return;
        }

        if (action == "observe") {
            QString node = payload["node"].toString();
            double value = payload["value"].toDouble();
            
            if (_network != nullptr) {
                // create network instance from given file
                try {
                    _network->observe(node.toStdString(), value);
                    networkChanged();
                } catch(const std::exception& e) {
                    QWebSocket* client = qobject_cast<QWebSocket*>(sender());
                    client->sendTextMessage(e.what());
                }
            }

            return;
        }

        if (action == "get_belief") {
            std::string node = payload["node"].toString().toStdString();
            
            if (_network != nullptr) {
                // create network instance from given file
                try {
                    // apply inference if network has changed
                    if (_networkChanged) {
                        _network->run();
                        _networkChanged = false;
                    }

                    // read belief
                    auto belief = _network->getBelief(node);
                    QWebSocket* client = qobject_cast<QWebSocket*>(sender());
                    client->sendTextMessage(belief.toString().c_str());
                } catch(const std::exception& e) {
                    QWebSocket* client = qobject_cast<QWebSocket*>(sender());
                    client->sendTextMessage(e.what());
                }
            }

            return;
        }


    }

    void Server::socketDisconnected() {
        // receive websocket client disconnecting
        QWebSocket* client = qobject_cast<QWebSocket*>(sender());

        // remove client from list and delete from memory
        if (client != nullptr) {
            _clients.removeAll(client);
            client->deleteLater();
        }
    }

    void Server::networkChanged() {
        _networkChanged = true;
    }
}

