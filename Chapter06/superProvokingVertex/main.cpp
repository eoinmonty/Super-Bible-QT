#include <QtGui/QApplication>
#include "glwidget.h"

int main(int argc, char *argv[])
{
    QApplication a( argc, argv );
    GLWidget glwidget;
    QGLFormat fmt;
    fmt.setSwapInterval(1);
    QGLFormat::setDefaultFormat(fmt);
    glwidget.showMaximized();

    return a.exec();
}

