#include "glwidget.h"
#include <QPainter>
#include <QPaintEngine>
#include <math.h>
#include <QTimer>



GLfloat vGreen[] = { 0.0f, 1.0f, 0.0f, 1.0f };
GLfloat vBlack[] = { 0.0f, 0.0f, 0.0f, 1.0f };




GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent)
{
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
    glViewport(0, 0, w, h);
    viewFrustum.SetPerspective(70.0f, w / h, 1.0f, 500.0f);
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

    switch(nStep) {
        case 0:
            glPointSize(10.0f);
            pointBatch.Draw();
            glPointSize(1.0f);
            break;
        case 1:
            glLineWidth(2.0f);
            lineBatch.Draw();
            glLineWidth(1.0f);
            break;
        case 2:
            glLineWidth(2.0f);
            lineStripBatch.Draw();
            glLineWidth(1.0f);
            break;
        case 3:
            glLineWidth(2.0f);
            lineLoopBatch.Draw();
            glLineWidth(1.0f);
            break;
        case 4:
            DrawWireFramedBatch(&triangleBatch);
            break;
        case 5:
            DrawWireFramedBatch(&triangleStripBatch);
            break;
        case 6:
            DrawWireFramedBatch(&triangleFanBatch);
            break;
    }

    modelViewMatrix.PopMatrix();
    // Flush drawing commands
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

    shaderManager.InitializeStockShaders();

    GLfloat vCoast[24][3] = {{2.80, 1.20, 0.0 }, {2.0,  1.20, 0.0 },
                             {2.0,  1.08, 0.0 },  {2.0,  1.08, 0.0 },
                             {0.0,  0.80, 0.0 },  {-.32, 0.40, 0.0 },
                             {-.48, 0.2, 0.0 },   {-.40, 0.0, 0.0 },
                             {-.60, -.40, 0.0 },  {-.80, -.80, 0.0 },
                             {-.80, -1.4, 0.0 },  {-.40, -1.60, 0.0 },
                             {0.0, -1.20, 0.0 },  { .2, -.80, 0.0 },
                             {.48, -.40, 0.0 },   {.52, -.20, 0.0 },
                             {.48,  .20, 0.0 },   {.80,  .40, 0.0 },
                             {1.20, .80, 0.0 },   {1.60, .60, 0.0 },
                             {2.0, .60, 0.0 },    {2.2, .80, 0.0 },
                             {2.40, 1.0, 0.0 },   {2.80, 1.0, 0.0 }};
    // Load up a triangle
    lineBatch.Begin(GL_LINES, 24);
    //squareBatch.Begin(GL_POINTS, 4);
    lineBatch.CopyVertexData3f(vCoast);
    lineBatch.End();

    // Load as a single line segment
    lineStripBatch.Begin(GL_LINE_STRIP, 24);
    lineStripBatch.CopyVertexData3f(vCoast);
    lineStripBatch.End();

    // Single line, connect first and last points
    lineLoopBatch.Begin(GL_LINE_LOOP, 24);
    lineLoopBatch.CopyVertexData3f(vCoast);
    lineLoopBatch.End();

    // For Triangles, we'll make a Pyramid
    GLfloat vPyramid[12][3] = { {-2.0f, 0.0f, -2.0f},
                                {2.0f, 0.0f, -2.0f},
                                {0.0f, 4.0f, 0.0f},

                                {2.0f, 0.0f, -2.0f},
                                {2.0f, 0.0f, 2.0f},
                                {0.0f, 4.0f, 0.0f},

                                {2.0f, 0.0f, 2.0f},
                                {-2.0f, 0.0f, 2.0f},
                                {0.0f, 4.0f, 0.0f},

                                {-2.0f, 0.0f, 2.0f},
                                {-2.0f, 0.0f, -2.0f},
                                {0.0f, 4.0f, 0.0f}};

    triangleBatch.Begin(GL_TRIANGLES, 12);
    triangleBatch.CopyVertexData3f(vPyramid);
    triangleBatch.End();

    // For a Triangle fan, just a 6 sided hex. Raise the center up a bit
    GLfloat vPoints[100][3];    // Scratch array, more than we need
    int nVerts = 0;
    GLfloat r = 3.0f;
    vPoints[nVerts][0] = 0.0f;
    vPoints[nVerts][1] = 0.0f;
    vPoints[nVerts][2] = 0.0f;

    for(GLfloat angle = 0; angle < M3D_2PI; angle += M3D_2PI / 6.0f) {
        nVerts++;
        vPoints[nVerts][0] = float(cos(angle)) * r;
        vPoints[nVerts][1] = float(sin(angle)) * r;
        vPoints[nVerts][2] = -0.5f;
        }

    // Close the fan
    nVerts++;
    vPoints[nVerts][0] = r;
    vPoints[nVerts][1] = 0;
    vPoints[nVerts][2] = 0.0f;

    // Load it up
    triangleFanBatch.Begin(GL_TRIANGLE_FAN, 8);
    triangleFanBatch.CopyVertexData3f(vPoints);
    triangleFanBatch.End();

    // For triangle strips, a little ring or cylinder segment
    int iCounter = 0;
    GLfloat radius = 3.0f;
    for(GLfloat angle = 0.0f; angle <= (2.0f*M3D_PI); angle += 0.3f)
    {
        GLfloat x = radius * sin(angle);
        GLfloat y = radius * cos(angle);

        // Specify the point and move the Z value up a little·
        vPoints[iCounter][0] = x;
        vPoints[iCounter][1] = y;
        vPoints[iCounter][2] = -0.5;
        iCounter++;

        vPoints[iCounter][0] = x;
        vPoints[iCounter][1] = y;
        vPoints[iCounter][2] = 0.5;
        iCounter++;
    }

    // Close up the loop
    vPoints[iCounter][0] = vPoints[0][0];
    vPoints[iCounter][1] = vPoints[0][1];
    vPoints[iCounter][2] = -0.5;
    iCounter++;

    vPoints[iCounter][0] = vPoints[1][0];
    vPoints[iCounter][1] = vPoints[1][1];
    vPoints[iCounter][2] = 0.5;
    iCounter++;

    // Load the triangle strip
    triangleStripBatch.Begin(GL_TRIANGLE_STRIP, iCounter);
    triangleStripBatch.CopyVertexData3f(vPoints);
    triangleStripBatch.End();
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

            if(nStep > 6)
              nStep = 0;

            switch(nStep)
            {
                case 0:
                    setWindowTitle("GL_POINTS");
                    break;
                case 1:
                    setWindowTitle("GL_LINES");
                    break;
                case 2:
                    setWindowTitle("GL_LINE_STRIP");
                    break;
                case 3:
                    setWindowTitle("GL_LINE_LOOP");
                    break;
                case 4:
                    setWindowTitle("GL_TRIANGLES");
                    break;
                case 5:
                    setWindowTitle("GL_TRIANGLE_STRIP");
                    break;
                case 6:
                    setWindowTitle("GL_TRIANGLE_FAN");
                    break;
           }
    }

    updateGL();
}

void GLWidget::DrawWireFramedBatch(GLBatch* pBatch)
{
    // Draw the batch solid green
    shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vGreen);
    pBatch->Draw();

    // Draw black outline
    glPolygonOffset(-1.0f, -1.0f);      // Shift depth values
    glEnable(GL_POLYGON_OFFSET_LINE);

    // Draw lines antialiased
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Draw black wireframe version of geometry
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(2.5f);
    shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(), vBlack);
    pBatch->Draw();

    // Put everything back the way we found it
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_POLYGON_OFFSET_LINE);
    glLineWidth(1.0f);
    glDisable(GL_BLEND);
    glDisable(GL_LINE_SMOOTH);
}
