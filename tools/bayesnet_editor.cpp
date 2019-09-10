//
// Created by Cedrik Kaufmann on 09.09.19.
//

#include <bayesnet/gui/editor.h>

#include <iostream>
#include <QApplication>

int main(int argc, char **argv) {
    // prepare qt application
    QApplication app(argc, argv);

    // create editor instance
    bayesNet::gui::Editor editor("./../../networks/sprinkler.bayesnet");
    editor.showMaximized();

    // infer and update editor beliefs
    editor.updateBayesNet();

    return app.exec();
}