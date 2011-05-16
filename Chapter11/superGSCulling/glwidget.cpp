#include "glwidget.h"

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent)
{
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);

    QTimer *timer = new QTimer(this);
    timer->setInterval(1);

    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
    setAutoBufferSwap(false);


    timer->start();
#ifndef Q_WS_QWS
    setMinimumSize(300, 250);
#endif
}

GLWidget::~GLWidget()
{
    glDeleteProgram(cullingShader);
}

void GLWidget::resizeGL(int w, int h)
{
    // Prevent a divide by zero
    if(h == 0)
        h = 1;

    // Set Viewport to window dimensions
    glViewport(0, 0, w, h);

    viewFrustum.SetPerspective(35.0f, float(w)/float(h), 1.0f, 100.0f);

    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
}

void GLWidget::paintGL()
{
    static CStopWatch rotTimer;

    // Clear the window and the depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    modelViewMatrix.PushMatrix(viewFrame);
    modelViewMatrix.Rotate(40.0f, 0.0f, 1.0f, 0.0f);
    modelViewMatrix.Rotate(20.0f, 1.0f, 0.0f, 0.0f);

    float f = (float)rotTimer.GetElapsedSeconds();
    GLfloat vViewpoint[] = { sinf(f * 3.1f) * 30.0f, cosf(f * 2.4f) * 30.0f, sinf(f * 1.7f) * 30.0f };

    glUseProgram(cullingShader);
    glUniformMatrix4fv(locMVP, 1, GL_FALSE, transformPipeline.GetModelViewProjectionMatrix());
    glUniformMatrix4fv(locMV, 1, GL_FALSE, transformPipeline.GetModelViewMatrix());
    glUniformMatrix3fv(locNM, 1, GL_FALSE, transformPipeline.GetNormalMatrix());
    glUniform3fv(locViewpoint, 1, vViewpoint);

    torusBatch.Draw();

    modelViewMatrix.PopMatrix();

    // Do the buffer Swap
    swapBuffers();
}

void GLWidget::initializeGL ()
{
    // Background
    glClearColor(0.2f, 0.2f, 0.3f, 1.0f );

    glEnable(GL_DEPTH_TEST);

    shaderManager.InitializeStockShaders();
    viewFrame.MoveForward(4.0f);

    // Make the torus
    gltMakeTorus(torusBatch, .70f, 0.30f, 32, 16);

    cullingShader = gltLoadShaderTripletWithAttributes("GSCulling.vs",
                                                       "GSCulling.gs",
                                                       "GSCulling.fs",
                                                       2,
                                                       GLT_ATTRIBUTE_VERTEX, "vVertex",
                                                       GLT_ATTRIBUTE_NORMAL, "vNormal");

    locMVP = glGetUniformLocation(cullingShader, "mvpMatrix");
    locMV  = glGetUniformLocation(cullingShader, "mvMatrix");
    locNM  = glGetUniformLocation(cullingShader, "normalMatrix");
    locViewpoint = glGetUniformLocation(cullingShader, "viewpoint");

}
