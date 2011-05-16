#include "glwidget.h"
#include <QTimer>

GLfloat vGreen[] = { 0.0f, 1.0f, 0.0f, 1.0f };


GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent)
{
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);

    QTimer *timer = new QTimer(this);
    timer->setInterval(5);

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
    glViewport(0, 0, width(), height());

    // Create the projection matrix, and load it on the projection matrix stack
    viewFrustum.SetPerspective(60.0f, float(width())/float(height()), 1.0f, 100.0f);
    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());

    // Set the transformation pipeline to use the two matrix stacks·
    transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
}

void GLWidget::paintGL()
{
    // Color values
    static GLfloat vFloorColor[] = { 0.0f, 1.0f, 0.0f, 1.0f};
    static GLfloat vTorusColor[] = { 1.0f, 0.0f, 0.0f, 1.0f };

    // Time Based animation
    static CStopWatch   rotTimer;
    float yRot = rotTimer.GetElapsedSeconds() * 60.0f;

    // Clear the color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    // Save the current modelview matrix (the identity matrix)
    modelViewMatrix.PushMatrix();

    // Draw the ground
    shaderManager.UseStockShader(GLT_SHADER_FLAT,
                                transformPipeline.GetModelViewProjectionMatrix(),
                                vFloorColor);
    floorBatch.Draw();

    // Draw the spinning Torus
    modelViewMatrix.Translate(0.0f, 0.0f, -2.5f);
    modelViewMatrix.Rotate(yRot, 0.0f, 1.0f, 0.0f);
    shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(),
                                 vTorusColor);
    torusBatch.Draw();

    // Restore the previous modleview matrix (the idenity matrix)
    modelViewMatrix.PopMatrix();

    swapBuffers();
}

void GLWidget::initializeGL ()
{
    // Initialze Shader Manager
    shaderManager.InitializeStockShaders();

    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // This makes a torus
    gltMakeTorus(torusBatch, 0.4f, 0.15f, 30, 30);


    floorBatch.Begin(GL_LINES, 324);
    for(GLfloat x = -20.0; x <= 20.0f; x+= 0.5) {
        floorBatch.Vertex3f(x, -0.55f, 20.0f);
        floorBatch.Vertex3f(x, -0.55f, -20.0f);

        floorBatch.Vertex3f(20.0f, -0.55f, x);
        floorBatch.Vertex3f(-20.0f, -0.55f, x);
        }
    floorBatch.End();
}



