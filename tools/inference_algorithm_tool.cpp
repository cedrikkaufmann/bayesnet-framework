/// @file
/// @brief BayesNet GUI, used to manage inference algorithms used by the engine

#include <bayesnet/gui/inference.h>

#include <QApplication>

int main(int argc, char **argv) {
    // prepare qt application
    QApplication app(argc, argv);

    // create window instance
    bayesNet::gui::InferenceWindow inferenceWindow;

    return QApplication::exec();
}