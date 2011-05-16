#include "glwidget.h"
#include <math.h>
#include <QTimer>
#include <iostream>

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
    glDeleteTextures(3, uiTextures);
}

void GLWidget::resizeGL(int w, int h)
{

    glViewport(0, 0, w, h);
    transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);

    viewFrustum.SetPerspective(35.0f, float(w)/float(h), 1.0f, 100.0f);
    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    modelViewMatrix.LoadIdentity();

}

void GLWidget::paintGL()
{

    static CStopWatch  rotTimer;
    float yRot = rotTimer.GetElapsedSeconds() * 60.0f;


    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    modelViewMatrix.PushMatrix();
    M3DMatrix44f mCamera;
    cameraFrame.GetCameraMatrix(mCamera);
    modelViewMatrix.MultMatrix(mCamera);

    // Draw the world upside down
    modelViewMatrix.PushMatrix();
    modelViewMatrix.Scale(1.0f, -1.0f, 1.0f); // Flips the Y Axis
    modelViewMatrix.Translate(0.0f, 0.8f, 0.0f); // Scootch the world down a bit...
    glFrontFace(GL_CW);
    DrawSongAndDance(yRot);
    glFrontFace(GL_CCW);
    modelViewMatrix.PopMatrix();

    // Draw the solid ground
    glEnable(GL_BLEND);
    glBindTexture(GL_TEXTURE_2D, uiTextures[0]);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    static GLfloat vFloorColor[] = { 1.0f, 1.0f, 1.0f, 0.75f};
    shaderManager.UseStockShader(GLT_SHADER_TEXTURE_MODULATE,
                                 transformPipeline.GetModelViewProjectionMatrix(),
                                 vFloorColor,
                                 0);

    floorBatch.Draw();
    glDisable(GL_BLEND);


    DrawSongAndDance(yRot);

    modelViewMatrix.PopMatrix();

    // Do the buffer Swap

    swapBuffers();
}

void GLWidget::initializeGL ()
{
    // Make sure OpenGL entry points are set
    /*GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        std::cout << "error: glewInit() failed\n";
    }*/
    // Initialze Shader Manager
    shaderManager.InitializeStockShaders();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // This makes a torus
    gltMakeTorus(torusBatch, 0.4f, 0.15f, 40, 20);

    // This makes a sphere
    gltMakeSphere(sphereBatch, 0.1f, 26, 13);


    // Make the solid ground
    GLfloat texSize = 10.0f;
    floorBatch.Begin(GL_TRIANGLE_FAN, 4, 1);
    floorBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
    floorBatch.Vertex3f(-20.0f, -0.41f, 20.0f);

    floorBatch.MultiTexCoord2f(0, texSize, 0.0f);
    floorBatch.Vertex3f(20.0f, -0.41f, 20.0f);

    floorBatch.MultiTexCoord2f(0, texSize, texSize);
    floorBatch.Vertex3f(20.0f, -0.41f, -20.0f);

    floorBatch.MultiTexCoord2f(0, 0.0f, texSize);
    floorBatch.Vertex3f(-20.0f, -0.41f, -20.0f);
    floorBatch.End();

    // Make 3 texture objects
    glGenTextures(3, uiTextures);

    // Load the Marble
    glBindTexture(GL_TEXTURE_2D, uiTextures[0]);
    LoadTGATexture("Marble.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT);

    // Load Mars
    glBindTexture(GL_TEXTURE_2D, uiTextures[1]);
    LoadTGATexture("Marslike.tga", GL_LINEAR_MIPMAP_LINEAR,
                   GL_LINEAR, GL_CLAMP_TO_EDGE);

    // Load Moon
    glBindTexture(GL_TEXTURE_2D, uiTextures[2]);
    LoadTGATexture("MoonLike.tga", GL_LINEAR_MIPMAP_LINEAR,
                   GL_LINEAR, GL_CLAMP_TO_EDGE);

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

void GLWidget::DrawSongAndDance(GLfloat yRot)   // Called to draw dancing objects
{
    static GLfloat vWhite[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    static GLfloat vLightPos[] = { 0.0f, 3.0f, 0.0f, 1.0f };

    // Get the light position in eye space
    M3DVector4f vLightTransformed;
    M3DMatrix44f mCamera;
    modelViewMatrix.GetMatrix(mCamera);
    m3dTransformVector4(vLightTransformed, vLightPos, mCamera);

    // Draw the light source
    modelViewMatrix.PushMatrix();
    modelViewMatrix.Translatev(vLightPos);
    shaderManager.UseStockShader(GLT_SHADER_FLAT,
                                 transformPipeline.GetModelViewProjectionMatrix(),
                                 vWhite);
    sphereBatch.Draw();
    modelViewMatrix.PopMatrix();

    glBindTexture(GL_TEXTURE_2D, uiTextures[2]);
    for(int i = 0; i < NUM_SPHERES; i++) {
        modelViewMatrix.PushMatrix();
        modelViewMatrix.MultMatrix(spheres[i]);
        shaderManager.UseStockShader(GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF,
                                     modelViewMatrix.GetMatrix(),
                                     transformPipeline.GetProjectionMatrix(),
                                     vLightTransformed,
                                     vWhite,
                                     0);
        sphereBatch.Draw();
        modelViewMatrix.PopMatrix();
    }

    // Song and dance
    modelViewMatrix.Translate(0.0f, 0.2f, -2.5f);
    modelViewMatrix.PushMatrix();  // Saves the translated origin
    modelViewMatrix.Rotate(yRot, 0.0f, 1.0f, 0.0f);

    // Draw stuff relative to the camera
    glBindTexture(GL_TEXTURE_2D, uiTextures[1]);
    shaderManager.UseStockShader(GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF,
                                 modelViewMatrix.GetMatrix(),
                                 transformPipeline.GetProjectionMatrix(),
                                 vLightTransformed,
                                 vWhite,
                                 0);
    torusBatch.Draw();
    modelViewMatrix.PopMatrix(); // Erased the rotate

    modelViewMatrix.Rotate(yRot * -2.0f, 0.0f, 1.0f, 0.0f);
    modelViewMatrix.Translate(0.8f, 0.0f, 0.0f);

    glBindTexture(GL_TEXTURE_2D, uiTextures[2]);
    shaderManager.UseStockShader(GLT_SHADER_TEXTURE_POINT_LIGHT_DIFF,
                                 modelViewMatrix.GetMatrix(),
                                 transformPipeline.GetProjectionMatrix(),
                                 vLightTransformed,
                                 vWhite,
                                 0);
    sphereBatch.Draw();
}


bool GLWidget::LoadTGATexture(const char *szFileName, GLenum minFilter, GLenum magFilter, GLenum wrapMode)
{
    GLbyte *pBits;
    int nWidth, nHeight, nComponents;
    GLenum eFormat;

    // Read the texture bits
    pBits = gltReadTGABits(szFileName, &nWidth, &nHeight, &nComponents, &eFormat);
    if(pBits == NULL)
        return false;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB, nWidth, nHeight, 0,
                 eFormat, GL_UNSIGNED_BYTE, pBits);

    free(pBits);

    if(minFilter == GL_LINEAR_MIPMAP_LINEAR ||
       minFilter == GL_LINEAR_MIPMAP_NEAREST ||
       minFilter == GL_NEAREST_MIPMAP_LINEAR ||
       minFilter == GL_NEAREST_MIPMAP_NEAREST)
        glGenerateMipmap(GL_TEXTURE_2D);

    return true;
}

