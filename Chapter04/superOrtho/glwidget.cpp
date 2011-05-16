#include "glwidget.h"
#include <QTimer>



GLfloat vGreen[] = { 0.0f, 1.0f, 0.0f, 1.0f };
GLfloat vBlack[] = { 0.0f, 0.0f, 0.0f, 1.0f };




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
}

void GLWidget::resizeGL(int w, int h)
{
    // Prevent a divide by zero
    if(h == 0)
        h = 1;

    // Set Viewport to window dimensions
    glViewport(0, 0, width(), height());

    viewFrustum.SetOrthographic(-130.0f, 130.0f, -130.0f, 130.0f, -130.0f, 130.0f);

    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    transformPipeline.SetMatrixStacks(modelViewMatix, projectionMatrix);
}

void GLWidget::paintGL()
{
    // Clear the window and the depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //  glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);


    modelViewMatix.PushMatrix(viewFrame);

    GLfloat vRed[] = { 1.0f, 0.0f, 0.0f, 1.0f };
    GLfloat vGray[] = { 0.75f, 0.75f, 0.75f, 1.0f };
    shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, transformPipeline.GetModelViewMatrix(), transformPipeline.GetProjectionMatrix(), vRed);
    tubeBatch.Draw();


    shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, transformPipeline.GetModelViewMatrix(), transformPipeline.GetProjectionMatrix(), vGray);
    innerBatch.Draw();

    modelViewMatix.PopMatrix();

    swapBuffers();
}

void GLWidget::initializeGL ()
{

    // Black background
    glClearColor(0.0f, 0.0f, 0.75f, 1.0f );

    //    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    shaderManager.InitializeStockShaders();


    tubeBatch.Begin(GL_QUADS, 200);

    float fZ = 100.0f;
    float bZ = -100.0f;

    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(0.0f, 0.0f, 1.0f);
    tubeBatch.Vertex3f(-50.0f, 50.0f, 100.0f);

    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(0.0f, 0.0f, 1.0f);
    tubeBatch.Vertex3f(-50.0f, -50.0f, fZ);

    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(0.0f, 0.0f, 1.0f);
    tubeBatch.Vertex3f(-35.0f, -50.0f, fZ);

    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(0.0f, 0.0f, 1.0f);
    tubeBatch.Vertex3f(-35.0f,50.0f,fZ);

    // Right Panel
    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(0.0f, 0.0f, 1.0f);
    tubeBatch.Vertex3f(50.0f, 50.0f, fZ);

    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(0.0f, 0.0f, 1.0f);
    tubeBatch.Vertex3f(35.0f, 50.0f, fZ);

    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(0.0f, 0.0f, 1.0f);
    tubeBatch.Vertex3f(35.0f, -50.0f, fZ);

    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(0.0f, 0.0f, 1.0f);
    tubeBatch.Vertex3f(50.0f,-50.0f,fZ);

    // Top Panel
    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(0.0f, 0.0f, 1.0f);
    tubeBatch.Vertex3f(-35.0f, 50.0f, fZ);

    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(0.0f, 0.0f, 1.0f);
    tubeBatch.Vertex3f(-35.0f, 35.0f, fZ);

    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(0.0f, 0.0f, 1.0f);
    tubeBatch.Vertex3f(35.0f, 35.0f, fZ);

    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(0.0f, 0.0f, 1.0f);
    tubeBatch.Vertex3f(35.0f, 50.0f,fZ);

    // Bottom Panel
    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(0.0f, 0.0f, 1.0f);
    tubeBatch.Vertex3f(-35.0f, -35.0f, fZ);

    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(0.0f, 0.0f, 1.0f);
    tubeBatch.Vertex3f(-35.0f, -50.0f, fZ);

    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(0.0f, 0.0f, 1.0f);
    tubeBatch.Vertex3f(35.0f, -50.0f, fZ);

    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(0.0f, 0.0f, 1.0f);
    tubeBatch.Vertex3f(35.0f, -35.0f,fZ);

    // Top length section ////////////////////////////
    // Normal points up Y axis
    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(0.0f, 1.0f, 0.0f);
    tubeBatch.Vertex3f(-50.0f, 50.0f, fZ);

    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(0.0f, 1.0f, 0.0f);
    tubeBatch.Vertex3f(50.0f, 50.0f, fZ);

    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(0.0f, 1.0f, 0.0f);
    tubeBatch.Vertex3f(50.0f, 50.0f, bZ);

    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(0.0f, 1.0f, 0.0f);
    tubeBatch.Vertex3f(-50.0f,50.0f,bZ);

    // Bottom section
    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(0.0f, -1.0f, 0.0f);
    tubeBatch.Vertex3f(-50.0f, -50.0f, fZ);

    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(0.0f, -1.0f, 0.0f);
    tubeBatch.Vertex3f(-50.0f, -50.0f, bZ);

    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(0.0f, -1.0f, 0.0f);
    tubeBatch.Vertex3f(50.0f, -50.0f, bZ);

    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(0.0f, -1.0f, 0.0f);
    tubeBatch.Vertex3f(50.0f, -50.0f, fZ);


    // Left section
    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(1.0f, 0.0f, 0.0f);
    tubeBatch.Vertex3f(50.0f, 50.0f, fZ);

    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(1.0f, 0.0f, 0.0f);
    tubeBatch.Vertex3f(50.0f, -50.0f, fZ);

    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(1.0f, 0.0f, 0.0f);
    tubeBatch.Vertex3f(50.0f, -50.0f, bZ);

    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(1.0f, 0.0f, 0.0f);
    tubeBatch.Vertex3f(50.0f, 50.0f, bZ);

    // Right Section
    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(-1.0f, 0.0f, 0.0f);
    tubeBatch.Vertex3f(-50.0f, 50.0f, fZ);

    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(-1.0f, 0.0f, 0.0f);
    tubeBatch.Vertex3f(-50.0f, 50.0f, bZ);

    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(-1.0f, 0.0f, 0.0f);
    tubeBatch.Vertex3f(-50.0f, -50.0f, bZ);

    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(-1.0f, 0.0f, 0.0f);
    tubeBatch.Vertex3f(-50.0f, -50.0f, fZ);

    // Pointing straight out Z········
    // Left Panel
    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(0.0f, 0.0f, 1.0f);
    tubeBatch.Vertex3f(-50.0f, 50.0f, fZ);

    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(0.0f, 0.0f, 1.0f);
    tubeBatch.Vertex3f(-50.0f, -50.0f, fZ);

    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(0.0f, 0.0f, 1.0f);
    tubeBatch.Vertex3f(-35.0f, -50.0f, fZ);

    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(0.0f, 0.0f, 1.0f);
    tubeBatch.Vertex3f(-35.0f,50.0f,fZ);

    // Right Panel
    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(0.0f, 0.0f, 1.0f);
    tubeBatch.Vertex3f(50.0f, 50.0f, fZ);

    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(0.0f, 0.0f, 1.0f);
    tubeBatch.Vertex3f(35.0f, 50.0f, fZ);

    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(0.0f, 0.0f, 1.0f);
    tubeBatch.Vertex3f(35.0f, -50.0f, fZ);

    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(0.0f, 0.0f, 1.0f);
    tubeBatch.Vertex3f(50.0f,-50.0f,fZ);

    // Top Panel
    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(0.0f, 0.0f, 1.0f);
    tubeBatch.Vertex3f(-35.0f, 50.0f, fZ);

    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(0.0f, 0.0f, 1.0f);
    tubeBatch.Vertex3f(-35.0f, 35.0f, fZ);

    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(0.0f, 0.0f, 1.0f);
    tubeBatch.Vertex3f(35.0f, 35.0f, fZ);

    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(0.0f, 0.0f, 1.0f);
    tubeBatch.Vertex3f(35.0f, 50.0f,fZ);

    // Bottom Panel
    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(0.0f, 0.0f, 1.0f);
    tubeBatch.Vertex3f(-35.0f, -35.0f, fZ);

    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(0.0f, 0.0f, 1.0f);
    tubeBatch.Vertex3f(-35.0f, -50.0f, fZ);

    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(0.0f, 0.0f, 1.0f);
    tubeBatch.Vertex3f(35.0f, -50.0f, fZ);

    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(0.0f, 0.0f, 1.0f);
    tubeBatch.Vertex3f(35.0f, -35.0f,fZ);

    // Top length section ////////////////////////////
    // Normal points up Y axis
    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(0.0f, 1.0f, 0.0f);
    tubeBatch.Vertex3f(-50.0f, 50.0f, fZ);

    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(0.0f, 1.0f, 0.0f);
    tubeBatch.Vertex3f(50.0f, 50.0f, fZ);

    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(0.0f, 1.0f, 0.0f);
    tubeBatch.Vertex3f(50.0f, 50.0f, bZ);

    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(0.0f, 1.0f, 0.0f);
    tubeBatch.Vertex3f(-50.0f,50.0f,bZ);

    // Bottom section
    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(0.0f, -1.0f, 0.0f);
    tubeBatch.Vertex3f(-50.0f, -50.0f, fZ);

    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(0.0f, -1.0f, 0.0f);
    tubeBatch.Vertex3f(-50.0f, -50.0f, bZ);

    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(0.0f, -1.0f, 0.0f);
    tubeBatch.Vertex3f(50.0f, -50.0f, bZ);

    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(0.0f, -1.0f, 0.0f);
    tubeBatch.Vertex3f(50.0f, -50.0f, fZ);

    // Left section
    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(1.0f, 0.0f, 0.0f);
    tubeBatch.Vertex3f(50.0f, 50.0f, fZ);

    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(1.0f, 0.0f, 0.0f);
    tubeBatch.Vertex3f(50.0f, -50.0f, fZ);

    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(1.0f, 0.0f, 0.0f);
    tubeBatch.Vertex3f(50.0f, -50.0f, bZ);

    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(1.0f, 0.0f, 0.0f);
    tubeBatch.Vertex3f(50.0f, 50.0f, bZ);

    // Right Section
    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(-1.0f, 0.0f, 0.0f);
    tubeBatch.Vertex3f(-50.0f, 50.0f, fZ);

    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(-1.0f, 0.0f, 0.0f);
    tubeBatch.Vertex3f(-50.0f, 50.0f, bZ);

    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(-1.0f, 0.0f, 0.0f);
    tubeBatch.Vertex3f(-50.0f, -50.0f, bZ);

    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Normal3f(-1.0f, 0.0f, 0.0f);
    tubeBatch.Vertex3f(-50.0f, -50.0f, fZ);

    // Left Panel
    tubeBatch.Normal3f(0.0f, 0.0f, -1.0f);
    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Vertex3f(-35.0f,50.0f,bZ);

    tubeBatch.Normal3f(0.0f, 0.0f, -1.0f);
    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Vertex3f(-35.0f, -50.0f, bZ);

    tubeBatch.Normal3f(0.0f, 0.0f, -1.0f);
    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Vertex3f(-50.0f, -50.0f, bZ);

    tubeBatch.Normal3f(0.0f, 0.0f, -1.0f);
    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Vertex3f(-50.0f, 50.0f, bZ);

    // Right Panel
    tubeBatch.Normal3f(0.0f, 0.0f, -1.0f);
    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);

    tubeBatch.Vertex3f(50.0f,-50.0f,bZ);

    tubeBatch.Normal3f(0.0f, 0.0f, -1.0f);
    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);

    tubeBatch.Vertex3f(35.0f, -50.0f, bZ);

    tubeBatch.Normal3f(0.0f, 0.0f, -1.0f);
    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);

    tubeBatch.Vertex3f(35.0f, 50.0f, bZ);

    tubeBatch.Normal3f(0.0f, 0.0f, -1.0f);
    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);

    tubeBatch.Vertex3f(50.0f, 50.0f, bZ);

    // Top Panel
    tubeBatch.Normal3f(0.0f, 0.0f, -1.0f);
    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Vertex3f(35.0f, 50.0f, bZ);
    tubeBatch.Normal3f(0.0f, 0.0f, -1.0f);
    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Vertex3f(35.0f, 35.0f, bZ);
    tubeBatch.Normal3f(0.0f, 0.0f, -1.0f);
    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Vertex3f(-35.0f, 35.0f, bZ);


    tubeBatch.Normal3f(0.0f, 0.0f, -1.0f);
    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Vertex3f(-35.0f, 50.0f, bZ);

    // Bottom Panel
    tubeBatch.Normal3f(0.0f, 0.0f, -1.0f);
    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Vertex3f(35.0f, -35.0f,bZ);
    tubeBatch.Normal3f(0.0f, 0.0f, -1.0f);
    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Vertex3f(35.0f, -50.0f, bZ);
    tubeBatch.Normal3f(0.0f, 0.0f, -1.0f);
    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Vertex3f(-35.0f, -50.0f, bZ);


    tubeBatch.Normal3f(0.0f, 0.0f, -1.0f);
    tubeBatch.Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    tubeBatch.Vertex3f(-35.0f, -35.0f, bZ);

    tubeBatch.End();


    innerBatch.Begin(GL_QUADS, 40);

    // Insides /////////////////////////////
    // Normal points up Y axis
    innerBatch.Color4f(0.75f, 0.75f, 0.75f, 1.0f);
    innerBatch.Normal3f(0.0f, 1.0f, 0.0f);
    innerBatch.Vertex3f(-35.0f, 35.0f, fZ);
    innerBatch.Color4f(0.75f, 0.75f, 0.75f, 1.0f);
    innerBatch.Normal3f(0.0f, 1.0f, 0.0f);
    innerBatch.Vertex3f(35.0f, 35.0f, fZ);
    innerBatch.Color4f(0.75f, 0.75f, 0.75f, 1.0f);
    innerBatch.Normal3f(0.0f, 1.0f, 0.0f);
    innerBatch.Vertex3f(35.0f, 35.0f, bZ);
    innerBatch.Color4f(0.75f, 0.75f, 0.75f, 1.0f);
    innerBatch.Normal3f(0.0f, 1.0f, 0.0f);
    innerBatch.Vertex3f(-35.0f,35.0f,bZ);

    // Bottom section
    innerBatch.Color4f(0.75f, 0.75f, 0.75f, 1.0f);
    innerBatch.Normal3f(0.0f, 1.0f, 0.0f);
    innerBatch.Vertex3f(-35.0f, -35.0f, fZ);
    innerBatch.Color4f(0.75f, 0.75f, 0.75f, 1.0f);
    innerBatch.Normal3f(0.0f, 1.0f, 0.0f);
    innerBatch.Vertex3f(-35.0f, -35.0f, bZ);
    innerBatch.Color4f(0.75f, 0.75f, 0.75f, 1.0f);
    innerBatch.Normal3f(0.0f, 1.0f, 0.0f);
    innerBatch.Vertex3f(35.0f, -35.0f, bZ);
    innerBatch.Color4f(0.75f, 0.75f, 0.75f, 1.0f);
    innerBatch.Normal3f(0.0f, 1.0f, 0.0f);
    innerBatch.Vertex3f(35.0f, -35.0f, fZ);

    // Left section
    innerBatch.Color4f(0.75f, 0.75f, 0.75f, 1.0f);
    innerBatch.Normal3f(1.0f, 0.0f, 0.0f);
    innerBatch.Vertex3f(-35.0f, 35.0f, fZ);
    innerBatch.Color4f(0.75f, 0.75f, 0.75f, 1.0f);
    innerBatch.Normal3f(1.0f, 0.0f, 0.0f);
    innerBatch.Vertex3f(-35.0f, 35.0f, bZ);
    innerBatch.Color4f(0.75f, 0.75f, 0.75f, 1.0f);
    innerBatch.Normal3f(1.0f, 0.0f, 0.0f);
    innerBatch.Vertex3f(-35.0f, -35.0f, bZ);
    innerBatch.Color4f(0.75f, 0.75f, 0.75f, 1.0f);
    innerBatch.Normal3f(1.0f, 0.0f, 0.0f);
    innerBatch.Vertex3f(-35.0f, -35.0f, fZ);

    // Right Section
    innerBatch.Color4f(0.75f, 0.75f, 0.75f, 1.0f);
    innerBatch.Normal3f(-1.0f, 0.0f, 0.0f);
    innerBatch.Vertex3f(35.0f, 35.0f, fZ);
    innerBatch.Color4f(0.75f, 0.75f, 0.75f, 1.0f);
    innerBatch.Normal3f(-1.0f, 0.0f, 0.0f);
    innerBatch.Vertex3f(35.0f, -35.0f, fZ);
    innerBatch.Color4f(0.75f, 0.75f, 0.75f, 1.0f);
    innerBatch.Normal3f(-1.0f, 0.0f, 0.0f);
    innerBatch.Vertex3f(35.0f, -35.0f, bZ);
    innerBatch.Color4f(0.75f, 0.75f, 0.75f, 1.0f);
    innerBatch.Normal3f(-1.0f, 0.0f, 0.0f);
    innerBatch.Vertex3f(35.0f, 35.0f, bZ);

    innerBatch.End();


}

void GLWidget::keyPressEvent(QKeyEvent *e)
{

    switch( e->key() )
    {
        case Qt::Key_Down:
            viewFrame.RotateWorld(m3dDegToRad(5.0), 1.0f, 0.0f, 0.0f);
            break;

        case Qt::Key_Up:
            viewFrame.RotateWorld(m3dDegToRad(-5.0), 1.0f, 0.0f, 0.0f);
            break;

        case Qt::Key_Left:
            viewFrame.RotateWorld(m3dDegToRad(-5.0), 0.0f, 1.0f, 0.0f);
            break;

        case Qt::Key_Right:
            viewFrame.RotateWorld(m3dDegToRad(5.0), 0.0f, 1.0f, 0.0f);
            break;
    }

    updateGL();
}
