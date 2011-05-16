#include "glwidget.h"

#define NUM_SPHERES 50
GLFrame spheres[NUM_SPHERES];

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent)
{
    nToggle = 1;
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
    triangleBatch.Draw();

    // Do the buffer Swap
    swapBuffers();
}

void GLWidget::initializeGL ()
{
    // Blue background
    glClearColor(0.0f, 0.0f, 0.5f, 1.0f );

    shaderManager.InitializeStockShaders();

    // Load up a triangle
    GLfloat vVerts[] = { -0.5f, 0.0f, 0.0f,
                         0.5f, 0.0f, 0.0f,
                         0.0f, 0.5f, 0.0f };

    GLfloat vColors [] = { 1.0f, 0.0f, 0.0f, 1.0f,
                           0.0f, 1.0f, 0.0f, 1.0f,
                           0.0f, 0.0f, 1.0f, 1.0f };

    triangleBatch.Begin(GL_TRIANGLES, 3);
    triangleBatch.CopyVertexData3f(vVerts);
    triangleBatch.CopyColorData4f(vColors);
    triangleBatch.End();

    myIdentityShader = gltLoadShaderPairWithAttributes("ProvokingVertex.vp", "ProvokingVertex.fp", 2,
                                                        GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_COLOR, "vColor");

    glProvokingVertex(GL_FIRST_VERTEX_CONVENTION);

}

void GLWidget::keyPressEvent(QKeyEvent *e)
{

    switch( e->key() )
    {
        case Qt::Key_Space:
            nToggle++;

            if(nToggle %2 == 0)
            {
                glProvokingVertex(GL_LAST_VERTEX_CONVENTION);
                setWindowTitle("Provoking Vertex - Last Vertex - Press Space Bars");
            }
            else
            {
                glProvokingVertex(GL_FIRST_VERTEX_CONVENTION);
                setWindowTitle("Provoking Vertex - First Vertex - Press Space Bars");
            }

            break;
        }
}

