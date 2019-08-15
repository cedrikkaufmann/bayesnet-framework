//
// Created by Cedrik Kaufmann on 2019-08-15.
//

#include <bayesnet/gui/inference.h>

#include <QApplication>

int main(int argc, char **argv) {
    // prepare qt application
    QApplication app(argc, argv);

    // create window instance
    bayesNet::gui::InferenceWindow inferenceWindow;

    return QApplication::exec();
}