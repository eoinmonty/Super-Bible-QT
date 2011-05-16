#include "glwidget.h"

#define NUM_SPHERES 50
GLFrame spheres[NUM_SPHERES];

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent)
{
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);

    QTimer *timer = new QTimer(this);
    timer->setInterval(5);

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
    glViewport(0, 0, width(), height());

    // Create the projection matrix, and load it on the projection matrix stack
    viewFrustum.SetPerspective(60.0f, float(width())/float(height()), 1.0f, 100.0f);
    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());

    // Set the transformation pipeline to use the two matrix stacks·
    transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
}

void GLWidget::paintGL()
{

    // Color values
    static GLfloat vFloorColor[] = { 0.0f, 1.0f, 0.0f, 1.0f};
    static GLfloat vTorusColor[] = { 1.0f, 0.0f, 0.0f, 1.0f };
    static GLfloat vSphereColor[] = { 0.0f, 0.0f, 1.0f, 1.0f };

    // Time Based animation
    static CStopWatch  rotTimer;
    float yRot = rotTimer.GetElapsedSeconds() * 60.0f;

    // Clear the color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    // Save the current modelview matrix (the identity matrix)
    modelViewMatrix.PushMatrix();

    M3DMatrix44f mCamera;
    cameraFrame.GetCameraMatrix(mCamera);
    modelViewMatrix.PushMatrix(mCamera);

    // Draw the ground
    shaderManager.UseStockShader(GLT_SHADER_FLAT,
    transformPipeline.GetModelViewProjectionMatrix(),vFloorColor);
    floorBatch.Draw();

    for(int i = 0; i < NUM_SPHERES; i++)
    {
        modelViewMatrix.PushMatrix();
        modelViewMatrix.MultMatrix(spheres[i]);
        shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(),
                                vSphereColor);
        sphereBatch.Draw();
        modelViewMatrix.PopMatrix();
    }


    // Draw the spinning Torus
    modelViewMatrix.Translate(0.0f, 0.0f, -2.5f);

    // Save the Translation
    modelViewMatrix.PushMatrix();

    // Apply a rotation and draw the torus
    modelViewMatrix.Rotate(yRot, 0.0f, 1.0f, 0.0f);
    shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(),
                                vTorusColor);
    torusBatch.Draw();
    modelViewMatrix.PopMatrix(); // "Erase" the Rotation from before

    // Apply another rotation, followed by a translation, then draw the sphere
    modelViewMatrix.Rotate(yRot * -1.5f, 0.5f, 1.0f, 0.0f);
    modelViewMatrix.Translate(0.8f, 0.0f, 0.0f);
    shaderManager.UseStockShader(GLT_SHADER_FLAT, transformPipeline.GetModelViewProjectionMatrix(),
                                     vSphereColor);
    sphereBatch.Draw();

    // Restore the previous modleview matrix (the identity matrix)
    modelViewMatrix.PopMatrix();
    modelViewMatrix.PopMatrix();
    // Do the buffer Swap

    swapBuffers();
}

void GLWidget::initializeGL ()
{
    // Initialze Shader Manager
    shaderManager.InitializeStockShaders();

    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // This makes a torus
    gltMakeTorus(torusBatch, 0.4f, 0.15f, 30, 30);

    // This make a sphere
    gltMakeSphere(sphereBatch, 0.1f, 26, 13);

    floorBatch.Begin(GL_LINES, 324);
    for(GLfloat x = -20.0; x <= 20.0f; x+= 0.5)
    {
        floorBatch.Vertex3f(x, -0.55f, 20.0f);
        floorBatch.Vertex3f(x, -0.55f, -20.0f);

        floorBatch.Vertex3f(20.0f, -0.55f, x);
        floorBatch.Vertex3f(-20.0f, -0.55f, x);
    }
    floorBatch.End();


    // Randomly place the spheres
    for(int i = 0; i < NUM_SPHERES; i++)
    {
        GLfloat x = ((GLfloat)((rand() % 400) - 200) * 0.1f);
        GLfloat z = ((GLfloat)((rand() % 400) - 200) * 0.1f);
        spheres[i].SetOrigin(x, 0.0f, z);
    }

}

void GLWidget::keyPressEvent(QKeyEvent *e)
{
    float linear = 0.1f;
    float angular = float(m3dDegToRad(5.0f));

    switch( e->key() )
    {
        case Qt::Key_Down:
            cameraFrame.MoveForward(-linear);
            break;

        case Qt::Key_Up:
            cameraFrame.MoveForward(linear);
            break;

        case Qt::Key_Left:
            cameraFrame.RotateWorld(angular, 0.0f, 1.0f, 0.0f);
            break;

        case Qt::Key_Right:
            cameraFrame.RotateWorld(-angular, 0.0f, 1.0f, 0.0f);
            break;
        }
}


