#include <QApplication>
#include "mainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow mainWin;
    mainWin.setMaximumSize(400,200);
    mainWin.resize(400,200);
    mainWin.show();

    return app.exec();
}

//------------------------------------------------------------------------------
