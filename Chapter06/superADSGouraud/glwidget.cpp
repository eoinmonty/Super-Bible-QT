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
        GLfloat vDiffuseColor[] = { 0.0f, 0.0f, 1.0f, 1.0f };
        GLfloat vSpecularColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };

        glUseProgram(ADSLightShader);
        glUniform4fv(locAmbient, 1, vAmbientColor);
        glUniform4fv(locDiffuse, 1, vDiffuseColor);
        glUniform4fv(locSpecular, 1, vSpecularColor);
        glUniform3fv(locLight, 1, vEyeLight);
        glUniformMatrix4fv(locMVP, 1, GL_FALSE, transformPipeline.GetModelViewProjectionMatrix());
        glUniformMatrix4fv(locMV, 1, GL_FALSE, transformPipeline.GetModelViewMatrix());
        glUniformMatrix3fv(locNM, 1, GL_FALSE, transformPipeline.GetNormalMatrix());
    sphereBatch.Draw();

    modelViewMatrix.PopMatrix();

    // Do the buffer Swap
    swapBuffers();
}

void GLWidget::initializeGL ()
{
    // Background
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f );

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    shaderManager.InitializeStockShaders();
    viewFrame.MoveForward(4.0f);

    // Make the sphere
    gltMakeSphere(sphereBatch, 1.0f, 100, 50);

    ADSLightShader = shaderManager.LoadShaderPairWithAttributes("ADSGouraud.vp", "ADSGouraud.fp", 2, GLT_ATTRIBUTE_VERTEX, "vVertex",
                                                                 GLT_ATTRIBUTE_NORMAL, "vNormal");

    locAmbient = glGetUniformLocation(ADSLightShader, "ambientColor");
    locDiffuse = glGetUniformLocation(ADSLightShader, "diffuseColor");
    locSpecular = glGetUniformLocation(ADSLightShader, "specularColor");
    locLight = glGetUniformLocation(ADSLightShader, "vLightPosition");
    locMVP = glGetUniformLocation(ADSLightShader, "mvpMatrix");
    locMV  = glGetUniformLocation(ADSLightShader, "mvMatrix");
    locNM  = glGetUniformLocation(ADSLightShader, "normalMatrix");
}
