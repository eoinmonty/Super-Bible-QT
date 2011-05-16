#include "glwidget.h"
#include <QTimer>


GLfloat vGreen[] = { 0.0f, 1.0f, 0.0f, 1.0f };
GLfloat vBlack[] = { 0.0f, 0.0f, 0.0f, 1.0f };

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent)
{
    // Keep track of effects step
    nStep = 0;

    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);

    QTimer *timer = new QTimer(this);
    timer->setInterval(10);

    //QObject::connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
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
    viewFrustum.SetPerspective(60.0f, float(width()) / float(height()), 1.0f, 500.0f);
    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    modelViewMatrix.LoadIdentity();

}

void GLWidget::paintGL()
{
    // Clear the window with current clearing color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    modelViewMatrix.PushMatrix();
    M3DMatrix44f mCamera;
    cameraFrame.GetCameraMatrix(mCamera);
    modelViewMatrix.MultMatrix(mCamera);

    M3DMatrix44f mObjectFrame;
    objectFrame.GetMatrix(mObjectFrame);
    modelViewMatrix.MultMatrix(mObjectFrame);

    shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vBlack);

    switch(nStep)
    {
        case 0:
            DrawWireFramedBatch(&sphereBatch);
            break;
        case 1:
            DrawWireFramedBatch(&torusBatch);
            break;
        case 2:
            DrawWireFramedBatch(&cylinderBatch);
            break;
        case 3:
            DrawWireFramedBatch(&coneBatch);
            break;
        case 4:
            DrawWireFramedBatch(&diskBatch);
            break;
    }

    modelViewMatrix.PopMatrix();

    swapBuffers();
}

void GLWidget::initializeGL ()
{

    // Black background
    glClearColor(0.7f, 0.7f, 0.7f, 1.0f );

    shaderManager.InitializeStockShaders();
    glEnable(GL_DEPTH_TEST);

    transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);

    cameraFrame.MoveForward(-15.0f);


    // Sphere
    gltMakeSphere(sphereBatch, 3.0, 10, 20);

    // Torus
    gltMakeTorus(torusBatch, 3.0f, 0.75f, 15, 15);

    // Cylinder
    gltMakeCylinder(cylinderBatch, 2.0f, 2.0f, 3.0f, 13, 2);

    // Cone
    gltMakeCylinder(coneBatch, 2.0f, 0.0f, 3.0f, 13, 2);

    // Disk
    gltMakeDisk(diskBatch, 1.5f, 3.0f, 13, 3);

}

void GLWidget::DrawWireFramedBatch(GLTriangleBatch* pBatch)
{
    shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vGreen);
    pBatch->Draw();

    // Draw black outline
    glPolygonOffset(-1.0f, -1.0f);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_POLYGON_OFFSET_LINE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(2.5f);
    shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vBlack);
    pBatch->Draw();

    // Restore polygon mode and depht testing
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_POLYGON_OFFSET_LINE);
    glLineWidth(1.0f);
    glDisable(GL_BLEND);
    glDisable(GL_LINE_SMOOTH);
}


void GLWidget::keyPressEvent(QKeyEvent *e)
{

    switch( e->key() )
    {
        case Qt::Key_Down:
            objectFrame.RotateWorld(m3dDegToRad(5.0f), 1.0f, 0.0f, 0.0f);
            break;

        case Qt::Key_Up:
            objectFrame.RotateWorld(m3dDegToRad(-5.0f), 1.0f, 0.0f, 0.0f);
            break;

        case Qt::Key_Left:
            objectFrame.RotateWorld(m3dDegToRad(-5.0f), 0.0f, 1.0f, 0.0f);
            break;

        case Qt::Key_Right:
            objectFrame.RotateWorld(m3dDegToRad(5.0f), 0.0f, 1.0f, 0.0f);
            break;

        case Qt::Key_Space:

            nStep++;

            if(nStep > 4)
                nStep = 0;

            switch(nStep)
            {
                case 0:
                    setWindowTitle("Sphere");
                    break;
                case 1:
                    setWindowTitle("Torus");
                    break;
                case 2:
                    setWindowTitle("Cylinder");
                    break;
                case 3:
                    setWindowTitle("Cone");
                    break;
                case 4:
                    setWindowTitle("Disk");
                    break;
             }

    }

    updateGL();
}
