#include "glwidget.h"
#include <QTimer>

GLBatch squareBatch;
GLShaderManager shaderManager;

GLfloat blockSize = 0.1f;
GLfloat vVerts[] = { -blockSize, -blockSize, 0.0f,
                     blockSize, -blockSize, 0.0f,
                     blockSize,  blockSize, 0.0f,
                    -blockSize,  blockSize, 0.0f};


GLfloat xPos = 0.0f;
GLfloat yPos = 0.0f;



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
    glViewport(0, 0, w, h);
}

void GLWidget::paintGL()
{
    // Clear the window with current clearing color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    GLfloat vRed[] = { 1.0f, 0.0f, 0.0f, 1.0f };

    M3DMatrix44f mFinalTransform, mTranslationMatrix, mRotationMatrix;

    // Just Translate
    m3dTranslationMatrix44(mTranslationMatrix, xPos, yPos, 0.0f);

    // Rotate 5 degrees evertyime we redraw
    static float yRot = 0.0f;
    yRot += 5.0f;
    m3dRotationMatrix44(mRotationMatrix, m3dDegToRad(yRot), 0.0f, 0.0f, 1.0f);

    m3dMatrixMultiply44(mFinalTransform, mTranslationMatrix, mRotationMatrix);


    shaderManager.UseStockShader(GLT_SHADER_FLAT, mFinalTransform, vRed);
    squareBatch.Draw();

    // Flush drawing commands
    swapBuffers();
}

void GLWidget::initializeGL ()
{
    // Black background
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f );

    shaderManager.InitializeStockShaders();

    // Load up a triangle
    squareBatch.Begin(GL_TRIANGLE_FAN, 4);
    squareBatch.CopyVertexData3f(vVerts);
    squareBatch.End();

}

void GLWidget::keyPressEvent(QKeyEvent *e)
{

    GLfloat stepSize = 0.025f;

    switch( e->key() )
    {
        case Qt::Key_Down:
            yPos -= stepSize;
            break;

        case Qt::Key_Up:
            yPos += stepSize;
            break;

        case Qt::Key_Left:
            xPos -= stepSize;
            break;

        case Qt::Key_Right:
            xPos += stepSize;
            break;
    }

    // Collision detection
    if(xPos < (-1.0f + blockSize)) xPos = -1.0f + blockSize;

    if(xPos > (1.0f - blockSize)) xPos = 1.0f - blockSize;

    if(yPos < (-1.0f + blockSize))  yPos = -1.0f + blockSize;

    if(yPos > (1.0f - blockSize)) yPos = 1.0f - blockSize;
    updateGL();
}
