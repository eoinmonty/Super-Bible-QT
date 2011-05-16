#include "glwidget.h"

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent)
{
    usePerspective = 1;
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
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteVertexArrays(1, &vao);

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
    modelViewMatrix.Rotate(rotTimer.GetElapsedSeconds() * 30.0f, 0.0f, 1.0f, 0.0f);

    glUseProgram(perspectiveShader);
    glUniformMatrix4fv(locMVP, 1, GL_FALSE, transformPipeline.GetModelViewProjectionMatrix());
    glUniform1i(locUsePerspective, usePerspective);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    modelViewMatrix.PopMatrix();

    // Do the buffer Swap
    swapBuffers();
}

void GLWidget::initializeGL ()
{
    // Background
    glClearColor(0.2f, 0.2f, 0.3f, 1.0f );

    glEnable(GL_DEPTH_TEST);

    viewFrame.MoveForward(5.0f);

    perspectiveShader = gltLoadShaderPairWithAttributes("Perspective.vs",
                                                        "Perspective.fs",
                                                        2,
                                                        GLT_ATTRIBUTE_VERTEX, "vVertex",
                                                        GLT_ATTRIBUTE_TEXTURE0, "vTexCoord");

    locMVP = glGetUniformLocation(perspectiveShader, "mvpMatrix");
    locUsePerspective = glGetUniformLocation(perspectiveShader, "usePerspective");

    // Geometry for a simple quad
    static const GLfloat quad[] =
    {
        -1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
    };

    static const GLfloat texcoords[] =
    {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };

    // A checkerboard texture
    static const GLubyte texture_data[] =
    {
        0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255,
        255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0,
        0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255,
        255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0,
        0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255,
        255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0,
        0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255,
        255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0,
        0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255,
        255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0,
        0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255,
        255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0,
        0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255,
        255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0,
        0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255,
        255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0
    };

    // Greate a vertex array object and a vertex buffer for the quad
    // including position and texture coordinates
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad) + sizeof(texcoords), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quad), quad);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(quad), sizeof(texcoords), texcoords);
    glVertexAttribPointer(GLT_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glVertexAttribPointer(GLT_ATTRIBUTE_TEXTURE0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *)sizeof(quad));
    glEnableVertexAttribArray(GLT_ATTRIBUTE_VERTEX);
    glEnableVertexAttribArray(GLT_ATTRIBUTE_TEXTURE0);

    // Create the checkerboard texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 16, 16, 0, GL_RED, GL_UNSIGNED_BYTE, texture_data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

}

void GLWidget::keyPressEvent(QKeyEvent *e)
{

    switch( e->key() )
    {
        case Qt::Key_Space:
            usePerspective ^= 1;
            if (usePerspective)
                setWindowTitle("Perspective Correction (Perspective Correct) - SPACE Toggles");
            else
                setWindowTitle("Perspective Correction (noperspective) - SPACE Toggles");
            break;
        default:
            break;
    }
    updateGL();
}
