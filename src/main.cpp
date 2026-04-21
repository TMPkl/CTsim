#include <QApplication>
#include "tomograph_window.hpp"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    TomographWindow window;
    window.show();

    return app.exec();
}
