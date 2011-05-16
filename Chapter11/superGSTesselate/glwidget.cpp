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
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &element_buffer);
    glDeleteBuffers(1, &vertex_buffer);

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
    modelViewMatrix.Rotate(rotTimer.GetElapsedSeconds() * 20.0f, 0.0f, 1.0f, 0.0f);
    modelViewMatrix.Rotate(rotTimer.GetElapsedSeconds() * 17.0f, 1.0f, 0.0f, 0.0f);

    /*GLfloat vEyeLight[] = { -100.0f, 100.0f, 100.0f };
    GLfloat vAmbientColor[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    GLfloat vDiffuseColor[] = { 0.1f, 1.0f, 0.1f, 1.0f };
    GLfloat vSpecularColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };*/

    glUseProgram(tessellateProgram);
    glUniformMatrix4fv(locMVP, 1, GL_FALSE, transformPipeline.GetModelViewProjectionMatrix());
    glUniformMatrix4fv(locMV, 1, GL_FALSE, transformPipeline.GetModelViewMatrix());
    glUniformMatrix3fv(locNM, 1, GL_FALSE, transformPipeline.GetNormalMatrix());

    glDisable(GL_CULL_FACE);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, NULL);

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

    tessellateProgram = gltLoadShaderTripletWithAttributes("GSTessellate.vs",
                                                           "GSTessellate.gs",
                                                           "GSTessellate.fs",
                                                           2,
                                                           GLT_ATTRIBUTE_VERTEX, "vVertex",
                                                           GLT_ATTRIBUTE_NORMAL, "vNormal");

    locMVP = glGetUniformLocation(tessellateProgram, "mvpMatrix");
    locMV  = glGetUniformLocation(tessellateProgram, "mvMatrix");
    locNM  = glGetUniformLocation(tessellateProgram, "normalMatrix");

    static const GLfloat vertices[] =
    {
        -1.0f, -1.0f, -1.0f,        // A
        -1.0f, -1.0f,  1.0f,        // B
        -1.0f,  1.0f, -1.0f,        // C
        -1.0f,  1.0f,  1.0f,        // D
         1.0f, -1.0f, -1.0f,        // E
         1.0f, -1.0f,  1.0f,        // F
         1.0f,  1.0f, -1.0f,        // G
         1.0f,  1.0f,  1.0f         // H
    };

    static const GLshort indices[] =
    {
        0, 1, 2,
        3, 2, 1,
        1, 5, 3,
        7, 3, 5,
        5, 4, 7,
        6, 7, 4,
        4, 0, 6,
        2, 6, 0,
        4, 5, 0,
        1, 0, 5,
        3, 7, 2,
        6, 2, 7
    };

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);
    glGenBuffers(1, &element_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


}
