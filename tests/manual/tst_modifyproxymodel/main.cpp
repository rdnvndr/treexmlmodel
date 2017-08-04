#include <QtGui>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(modifyproxymodel);

    QApplication app(argc, argv);
    MainWindow window;
#if defined(Q_OS_SYMBIAN)
    window.showMaximized();
#else
    window.show();
#endif
    return app.exec();
}
