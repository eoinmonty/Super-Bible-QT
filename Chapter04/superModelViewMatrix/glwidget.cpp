#include "glwidget.h"
#include <QTimer>



GLfloat vGreen[] = { 0.0f, 1.0f, 0.0f, 1.0f };


GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent)
{
    // Keep track of effects step
    nStep = 0;

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

    // Prevent a divide by zero
    if(h == 0)
        h = 1;

    // Set Viewport to window dimensions
    glViewport(0, 0, w, h);

    viewFrustum.SetPerspective(60.0f, float(w)/float(h), 1.0f, 1000.0f);

}

void GLWidget::paintGL()
{
    // Set up time based animation
    static CStopWatch rotTimer;
    float yRot = rotTimer.GetElapsedSeconds() * 60.0f;

    // Clear the window and the depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Matrix Variables
    M3DMatrix44f mTranslate, mRotate, mModelview, mModelViewProjection;

    // Create a translation matrix to move the torus back and into sight
    m3dTranslationMatrix44(mTranslate, 0.0f, 0.0f, -2.5f);

    // Create a rotation matrix based on the current value of yRot
    m3dRotationMatrix44(mRotate, m3dDegToRad(yRot), 0.0f, 1.0f, 0.0f);

    // Add the rotation to the translation, store the result in mModelView
    m3dMatrixMultiply44(mModelview, mTranslate, mRotate);

    // Add the modelview matrix to the projection matrix,·
    // the final matrix is the ModelViewProjection matrix.
    m3dMatrixMultiply44(mModelViewProjection, viewFrustum.GetProjectionMatrix(),mModelview);

    // Pass this completed matrix to the shader, and render the torus
    GLfloat vBlack[] = { 0.0f, 0.0f, 0.0f, 1.0f };

    shaderManager.UseStockShader(GLT_SHADER_FLAT, mModelViewProjection, vBlack);

    torusBatch.Draw();
    swapBuffers();
}

void GLWidget::initializeGL ()
{

    // Black background
    glClearColor(0.8f, 0.8f, 0.8f, 1.0f );

    shaderManager.InitializeStockShaders();
    glEnable(GL_DEPTH_TEST);


    gltMakeTorus(torusBatch, 0.4f, 0.15f, 30, 30);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

}



