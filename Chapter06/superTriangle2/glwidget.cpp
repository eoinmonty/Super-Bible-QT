#include "glwidget.h"

#define NUM_SPHERES 50
GLFrame spheres[NUM_SPHERES];

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
    setMinimumSize(300, 250);
#endif
}

GLWidget::~GLWidget()
{
    glDeleteProgram(myIdentityShader);

}

void GLWidget::resizeGL(int w, int h)
{

    glViewport(0, 0, w, h);
}

void GLWidget::paintGL()
{
    // Clear the window with current clearing color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glUseProgram(myIdentityShader);

    GLint iColorLocation = glGetUniformLocation(myIdentityShader, "vColor");
    GLfloat vColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
    glUniform4fv(iColorLocation, 1, vColor);

    triangleBatch.Draw();


    // Do the buffer Swap

    swapBuffers();
}

void GLWidget::initializeGL ()
{
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f );

    shaderManager.InitializeStockShaders();

    // Load up a triangle
    GLfloat vVerts[] = { -0.5f, 0.0f, 0.0f,
                        0.5f, 0.0f, 0.0f,
                        0.0f, 0.5f, 0.0f };

    triangleBatch.Begin(GL_TRIANGLES, 3);
    triangleBatch.CopyVertexData3f(vVerts);
    triangleBatch.End();

    myIdentityShader = shaderManager.LoadShaderPairWithAttributes("Identity.vp", "Identity.fp", 1, GLT_ATTRIBUTE_VERTEX, "vVertex");
}

