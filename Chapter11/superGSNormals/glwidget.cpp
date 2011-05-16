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
    glDeleteProgram(flatProgram);
    glDeleteProgram(normalsProgram);
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
    modelViewMatrix.Rotate(rotTimer.GetElapsedSeconds() * 10.0f, 0.0f, 1.0f, 0.0f);
    modelViewMatrix.Rotate(rotTimer.GetElapsedSeconds() * 13.0f, 1.0f, 0.0f, 0.0f);

//    GLfloat vEyeLight[] = { -100.0f, 100.0f, 100.0f };
//    GLfloat vAmbientColor[] = { 0.1f, 0.1f, 0.1f, 1.0f };
//    GLfloat vDiffuseColor[] = { 0.1f, 1.0f, 0.1f, 1.0f };
//    GLfloat vSpecularColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    glUseProgram(flatProgram);
    glUniformMatrix4fv(locMVP2, 1, GL_FALSE, transformPipeline.GetModelViewProjectionMatrix());
    glUniformMatrix4fv(locMV2, 1, GL_FALSE, transformPipeline.GetModelViewMatrix());
    glUniformMatrix3fv(locNM2, 1, GL_FALSE, transformPipeline.GetNormalMatrix());

    torusBatch.Draw();

    glUseProgram(normalsProgram);
    glUniformMatrix4fv(locMVP, 1, GL_FALSE, transformPipeline.GetModelViewProjectionMatrix());
    glUniformMatrix4fv(locMV, 1, GL_FALSE, transformPipeline.GetModelViewMatrix());
    glUniformMatrix3fv(locNM, 1, GL_FALSE, transformPipeline.GetNormalMatrix());

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
    gltMakeTorus(torusBatch, .80f, 0.25f, 14, 12);

    normalsProgram = gltLoadShaderTripletWithAttributes("GSNormals.vs",
                                                        "GSNormals.gs",
                                                        "GSNormals.fs",
                                                        2,
                                                        GLT_ATTRIBUTE_VERTEX, "vVertex",
                                                        GLT_ATTRIBUTE_NORMAL, "vNormal");

    locMVP = glGetUniformLocation(normalsProgram, "mvpMatrix");
    locMV  = glGetUniformLocation(normalsProgram, "mvMatrix");
    locNM  = glGetUniformLocation(normalsProgram, "normalMatrix");

    flatProgram = gltLoadShaderTripletWithAttributes("FlatShader.vs",
                                                     NULL,
                                                     "FlatShader.fs",
                                                     2,
                                                     GLT_ATTRIBUTE_VERTEX, "vVertex",
                                                     GLT_ATTRIBUTE_NORMAL, "vNormal");

    locMVP2 = glGetUniformLocation(flatProgram, "mvpMatrix");
    locMV2  = glGetUniformLocation(flatProgram, "mvMatrix");
    locNM2  = glGetUniformLocation(flatProgram, "normalMatrix");

}
