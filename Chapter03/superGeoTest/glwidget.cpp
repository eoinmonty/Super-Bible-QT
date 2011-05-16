#include "glwidget.h"
#include <QTimer>


GLfloat vGreen[] = { 0.0f, 1.0f, 0.0f, 1.0f };
GLfloat vBlack[] = { 0.0f, 0.0f, 0.0f, 1.0f };


GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent)
{
    // Flags for effects
    iCull = 0;
    iDepth = 0;

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
    transformPipeline.SetMatrixStacks(modelViewMatix, projectionMatrix);
}

void GLWidget::paintGL()
{
    // Clear the window and the depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Turn culling on if flag is set
    if(iCull)
        glEnable(GL_CULL_FACE);
    else
        glDisable(GL_CULL_FACE);

    // Enable depth testing if flag is set
    if(iDepth)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);

    modelViewMatix.PushMatrix(viewFrame);

    GLfloat vRed[] = { 1.0f, 0.0f, 0.0f, 1.0f };
    //shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vRed);
    shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, transformPipeline.GetModelViewMatrix(), transformPipeline.GetProjectionMatrix(), vRed);

    torusBatch.Draw();

    modelViewMatix.PopMatrix();

    swapBuffers();
}

void GLWidget::initializeGL ()
{
    // Black background
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f );

    shaderManager.InitializeStockShaders();
    viewFrame.MoveForward(7.0f);

    // Make the torus
    gltMakeTorus(torusBatch, 1.0f, 0.3f, 52, 26);

    glPointSize(4.0f);

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

void GLWidget::togDepth()
{
    iDepth = !iDepth;
    updateGL();
}

void GLWidget::togCull()
{
    iCull = !iCull;
    updateGL();
}

void GLWidget::setFill()
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    updateGL();
}

void GLWidget::setLine()
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    updateGL();
}

void GLWidget::setPoint()
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    updateGL();
}

void GLWidget::mouseReleaseEvent ( QMouseEvent * event )
{
    if(event->button() == Qt::RightButton)
    {
        QMenu menu;

        QAction* toggleDepth = new QAction("Toggle Depth Test", this);
        QAction* toggleCull = new QAction("Toggle Cull Backface", this);
        QAction* setFillMode = new QAction("Set Fill Mode", this);
        QAction* setLineMode = new QAction("Set Line Mode", this);
        QAction* setPointMode = new QAction("Set Point Mode", this);

        menu.addAction(toggleDepth);
        menu.addAction(toggleCull);
        menu.addAction(setFillMode);
        menu.addAction(setLineMode);
        menu.addAction(setPointMode);

        menu.addSeparator();

        QAction* pItem = menu.exec(mapToGlobal(event->pos()));

        if(pItem == toggleDepth)
        {
            togDepth();
        }
        else if(pItem == toggleCull)
        {
            togCull();
        }
        else if(pItem == setFillMode)
        {
            setFill();
        }
        else if(pItem == setLineMode)
        {
            setLine();
        }
        else if(pItem == setPointMode)
        {
            setPoint();
        }
    }
}
