//
// Created by Cedrik Kaufmann on 09.09.19.
//

#include <string>

#include <bayesnet/gui/editor.h>

#include <QApplication>


int main(int argc, char **argv) {
    // prepare qt application
    QApplication app(argc, argv);

    // create editor instance
    bayesNet::gui::Editor *editor;

    if (argc > 1) {
        std::string file = std::string(argv[1]);
        editor = new bayesNet::gui::Editor(file);
    } else {
        editor = new bayesNet::gui::Editor();
    }

    editor->showMaximized();

    return QApplication::exec();
}