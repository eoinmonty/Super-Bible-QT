#include <QtGui/QApplication>
#include "glwidget.h"

int main(int argc, char *argv[])
{
    QApplication a( argc, argv );
    GLWidget glwidget;
    glwidget.showMaximized();

    return a.exec();
}

