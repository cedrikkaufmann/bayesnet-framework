/// @file
/// @brief BayesNet GUI, used to visualize a network from a given network file

#include <string>

#include <bayesnet/gui/editor.h>

#include <QApplication>


int main(int argc, char **argv) {
    // prepare qt application
    QApplication app(argc, argv);

    // create editor instance
    bayesNet::gui::Editor *editor;

    // if program started with params, try to use first as network filename
    if (argc > 1) {
        std::string file(argv[1]);
        editor = new bayesNet::gui::Editor(file);
    } else {
        editor = new bayesNet::gui::Editor();
    }

    editor->showMaximized();

    return QApplication::exec();
}