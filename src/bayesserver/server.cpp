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

                QJsonObject json;
                QJsonObject payload;
                json["action"] = "response";
                payload["status"] = "success";
                payload["response_to"] = action;
                json["payload"] = payload;

                QWebSocket* client = qobject_cast<QWebSocket*>(sender());
                client->sendTextMessage(QJsonDocument(json).toJson());
            } catch(const std::exception& e) {
                QJsonObject json;
                QJsonObject payload;
                json["action"] = "response";
                payload["status"] = "error";
                payload["error"] = e.what();
                json["payload"] = payload;
                
                QWebSocket* client = qobject_cast<QWebSocket*>(sender());
                client->sendTextMessage(QJsonDocument(json).toJson());
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

                    QJsonObject json;
                    QJsonObject payload;
                    json["action"] = "response";
                    payload["status"] = "success";
                    payload["response_to"] = action;
                    json["payload"] = payload;

                    QWebSocket* client = qobject_cast<QWebSocket*>(sender());
                    client->sendTextMessage(QJsonDocument(json).toJson());
                } catch(const std::exception& e) {
                    QJsonObject json;
                    QJsonObject payload;
                    json["action"] = "response";
                    payload["status"] = "error";
                    payload["error"] = e.what();
                    json["payload"] = payload;
                    
                    QWebSocket* client = qobject_cast<QWebSocket*>(sender());
                    client->sendTextMessage(QJsonDocument(json).toJson());
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

                    QJsonObject json;
                    QJsonObject payload;
                    json["action"] = "response";
                    payload["status"] = "success";
                    payload["response_to"] = action;
                    json["payload"] = payload;

                    QWebSocket* client = qobject_cast<QWebSocket*>(sender());
                    client->sendTextMessage(QJsonDocument(json).toJson());
                } catch(const std::exception& e) {
                    QJsonObject json;
                    QJsonObject payload;
                    json["action"] = "response";
                    payload["status"] = "error";
                    payload["error"] = e.what();
                    json["payload"] = payload;
                    
                    QWebSocket* client = qobject_cast<QWebSocket*>(sender());
                    client->sendTextMessage(QJsonDocument(json).toJson());
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

                    QJsonObject json;
                    QJsonObject payload;
                    json["action"] = "response";
                    payload["status"] = "success";
                    payload["response_to"] = action;
                    json["payload"] = payload;
                    
                    QWebSocket* client = qobject_cast<QWebSocket*>(sender());
                    client->sendTextMessage(QJsonDocument(json).toJson());
                } catch(const std::exception& e) {
                    QJsonObject json;
                    QJsonObject payload;
                    json["action"] = "response";
                    payload["status"] = "error";
                    payload["error"] = e.what();
                    json["payload"] = payload;
                    
                    QWebSocket* client = qobject_cast<QWebSocket*>(sender());
                    client->sendTextMessage(QJsonDocument(json).toJson());
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

                    QJsonObject json;
                    QJsonObject payload;
                    json["action"] = "response";
                    payload["status"] = "success";
                    payload["response_to"] = action;

                    // read belief
                    QJsonObject jsonBelief;
                    auto belief = _network->getBelief(node);
                    auto continiousBelief = _network->getContinousBelief(node);

                    if (belief.nrStates() == 2) {
                        jsonBelief["TRUE"] = belief[bayesNet::state::TRUE];
                        jsonBelief["FALSE"] = belief[bayesNet::state::FALSE];
                    } else {
                        jsonBelief["GOOD"] = belief[bayesNet::state::GOOD];
                        jsonBelief["PROBABLY_GOOD"] = belief[bayesNet::state::PROBABLY_GOOD];
                        jsonBelief["PROBABLY_BAD"] = belief[bayesNet::state::PROBABLY_BAD];
                        jsonBelief["BAD"] = belief[bayesNet::state::BAD];
                    }
                    
                    jsonBelief["continious_belief"] = std::to_string(continiousBelief).c_str();
                    payload[node.c_str()] = jsonBelief;
                    
                    json["payload"] = payload;

                    QWebSocket* client = qobject_cast<QWebSocket*>(sender());
                    client->sendTextMessage(QJsonDocument(json).toJson());
                } catch(const std::exception& e) {
                    QJsonObject json;
                    QJsonObject payload;
                    json["action"] = "response";
                    payload["status"] = "error";
                    payload["error"] = e.what();
                    json["payload"] = payload;
                    
                    QWebSocket* client = qobject_cast<QWebSocket*>(sender());
                    client->sendTextMessage(QJsonDocument(json).toJson());
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

