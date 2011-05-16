#include "glwidget.h"

#define NUM_SPHERES 50
GLFrame spheres[NUM_SPHERES];

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
    glDeleteTextures(1, &texture);
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

        GLfloat vEyeLight[] = { -100.0f, 100.0f, 100.0f };
        GLfloat vAmbientColor[] = { 0.1f, 0.1f, 0.1f, 1.0f };
        GLfloat vDiffuseColor[] = { 0.1f, 1.0f, 0.1f, 1.0f };
        GLfloat vSpecularColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };

        glUseProgram(toonShader);
        glUniform3fv(locLight, 1, vEyeLight);
        glUniformMatrix4fv(locMVP, 1, GL_FALSE, transformPipeline.GetModelViewProjectionMatrix());
        glUniformMatrix4fv(locMV, 1, GL_FALSE, transformPipeline.GetModelViewMatrix());
        glUniformMatrix3fv(locNM, 1, GL_FALSE, transformPipeline.GetNormalMatrix());
        glUniform1i(locColorTable, 0);
        torusBatch.Draw();

    modelViewMatrix.PopMatrix();

    // Do the buffer Swap
    swapBuffers();
}

void GLWidget::initializeGL ()
{
    // Background
    glClearColor(0.025f, 0.25f, 0.25f, 1.0f );

    glEnable(GL_DEPTH_TEST);

    shaderManager.InitializeStockShaders();
    viewFrame.MoveForward(4.0f);

    // Make the torus
    gltMakeTorus(torusBatch, .80f, 0.25f, 52, 26);

    toonShader = gltLoadShaderPairWithAttributes("ToonShader.vp", "ToonShader.fp", 2, GLT_ATTRIBUTE_VERTEX, "vVertex",
                                                  GLT_ATTRIBUTE_NORMAL, "vNormal");

    locLight = glGetUniformLocation(toonShader, "vLightPosition");
    locMVP = glGetUniformLocation(toonShader, "mvpMatrix");
    locMV  = glGetUniformLocation(toonShader, "mvMatrix");
    locNM  = glGetUniformLocation(toonShader, "normalMatrix");
    locColorTable = glGetUniformLocation(toonShader, "colorTable");

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_1D, texture);
    GLubyte textureData[4][3] = { 32,  0, 0,
                                  64,  0, 0,
                                  128, 0, 0,
                                  255, 0, 0};

    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, 4, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
}
