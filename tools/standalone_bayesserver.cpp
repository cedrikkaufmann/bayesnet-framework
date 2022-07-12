/// @file
/// @brief Standalone BayesServer, used to run a network through a websocket api

#include <iostream>
#include <QtCore/QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>

#include <bayesserver/server.h>


int main(int argc, char **argv) {
    QCoreApplication app(argc, argv);

    // setup command line parser
    QCommandLineParser parser;
    // command line option for port
    QCommandLineOption portOption({"port", "p"}, "port to listen on", "websocket_port", "8000");
    // set options for parser
    parser.addHelpOption();
    parser.addOption(portOption);
    // parse arguments
    parser.process(app);

    // read port
    uint port = parser.value(portOption).toUInt();

    // create server instance
    bayesServer::Server srv(port, &app);

    // print program info
    std::cout << ">> Standalone BayesServer\n" << ">> Compiled: " << __TIMESTAMP__ << "\n" ">> Listening on port " << parser.value(portOption).toStdString() << std::endl;

    // run main event loop
    return app.exec();
}