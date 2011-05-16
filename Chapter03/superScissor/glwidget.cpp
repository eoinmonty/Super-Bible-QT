#include "glwidget.h"
#include <QTimer>

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent)
{

    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);

    QTimer *timer = new QTimer(this);
    timer->setInterval(10);

    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
    setAutoBufferSwap(false);

    timer->start();
#ifndef Q_WS_QWS
    setMinimumSize(800, 600);
#endif
}

GLWidget::~GLWidget()
{
}

void GLWidget::resizeGL(int w, int h)
{
    // Prevent a divide by zero
    if(h == 0)
        h = 1;

    // Set Viewport to window dimensions
    glViewport(0, 0, w, h);

}

void GLWidget::paintGL()
{

    // Clear blue window
    glClearColor(0.0f, 0.0f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Now set scissor to smaller red sub region
    glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
    glScissor(100, 100, 600, 400);
    glEnable(GL_SCISSOR_TEST);
    glClear(GL_COLOR_BUFFER_BIT);

    // Finally, an even smaller green rectangle
    glClearColor(0.0f, 1.0f, 0.0f, 0.0f);
    glScissor(200, 200, 400, 200);
    glClear(GL_COLOR_BUFFER_BIT);

    // Turn scissor back off for next render
    glDisable(GL_SCISSOR_TEST);


    swapBuffers();
}

