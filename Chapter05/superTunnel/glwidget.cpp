#include "glwidget.h"
#include <math.h>
#include <QTimer>

// Texture objects^M
#define TEXTURE_BRICK   0
#define TEXTURE_FLOOR   1
#define TEXTURE_CEILING 2
#define TEXTURE_COUNT   3

GLuint  textures[TEXTURE_COUNT];
const char *szTextureFiles[TEXTURE_COUNT] = { "brick.tga", "floor.tga", "ceiling.tga" };


GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent)
{
    viewZ = -65.0f;
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);

    QTimer *timer = new QTimer(this);
    timer->setInterval(10);

    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
    setAutoBufferSwap(false);

    createMenus();
    timer->start();
#ifndef Q_WS_QWS
    setMinimumSize(300, 250);
#endif
}

GLWidget::~GLWidget()
{
    glDeleteTextures(TEXTURE_COUNT, textures);
}

void GLWidget::resizeGL(int w, int h)
{
    GLfloat fAspect;

    // Prevent a divide by zero^M
    if(h == 0)
        h = 1;

    // Set Viewport to window dimensions^M
    glViewport(0, 0, w, h);

    fAspect = (GLfloat)w/(GLfloat)h;

    // Produce the perspective projection^M
    viewFrustum.SetPerspective(80.0f,fAspect,1.0,120.0);
    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);

}

void GLWidget::paintGL()
{
    // Clear the window with current clearing color
    glClear(GL_COLOR_BUFFER_BIT);

    modelViewMatrix.PushMatrix();
        modelViewMatrix.Translate(0.0f, 0.0f, viewZ);

        shaderManager.UseStockShader(GLT_SHADER_TEXTURE_REPLACE, transformPipeline.GetModelViewProjectionMatrix(), 0);

        glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_FLOOR]);
        floorBatch.Draw();

        glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_CEILING]);
        ceilingBatch.Draw();

        glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_BRICK]);
        leftWallBatch.Draw();
        rightWallBatch.Draw();

    modelViewMatrix.PopMatrix();
    // Flush drawing commands
    swapBuffers();
}

void GLWidget::initializeGL ()
{
    GLbyte *pBytes;
    GLint iWidth, iHeight, iComponents;
    GLenum eFormat;
    GLint iLoop;

    // Black background
    glClearColor(0.0f, 0.0f, 0.0f,1.0f);

    shaderManager.InitializeStockShaders();

    // Load textures
    glGenTextures(TEXTURE_COUNT, textures);
    for(iLoop = 0; iLoop < TEXTURE_COUNT; iLoop++)
    {
        // Bind to next texture object
        glBindTexture(GL_TEXTURE_2D, textures[iLoop]);

        // Load texture, set filter and wrap modes
        pBytes = gltReadTGABits(szTextureFiles[iLoop],&iWidth, &iHeight,
                              &iComponents, &eFormat);

        // Load texture, set filter and wrap modes^M
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, iComponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBytes);
        glGenerateMipmap(GL_TEXTURE_2D);
        // Don't need original texture data any more^M
        free(pBytes);
    }

    // Build Geometry
    GLfloat z;
    floorBatch.Begin(GL_TRIANGLE_STRIP, 28, 1);
    for(z = 60.0f; z >= 0.0f; z -=10.0f)
    {
        floorBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
        floorBatch.Vertex3f(-10.0f, -10.0f, z);

        floorBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
        floorBatch.Vertex3f(10.0f, -10.0f, z);

        floorBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
        floorBatch.Vertex3f(-10.0f, -10.0f, z - 10.0f);

        floorBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
        floorBatch.Vertex3f(10.0f, -10.0f, z - 10.0f);
    }
    floorBatch.End();

    ceilingBatch.Begin(GL_TRIANGLE_STRIP, 28, 1);
    for(z = 60.0f; z >= 0.0f; z -=10.0f)
    {
        ceilingBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
        ceilingBatch.Vertex3f(-10.0f, 10.0f, z - 10.0f);

        ceilingBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
        ceilingBatch.Vertex3f(10.0f, 10.0f, z - 10.0f);

        ceilingBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
        ceilingBatch.Vertex3f(-10.0f, 10.0f, z);

        ceilingBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
        ceilingBatch.Vertex3f(10.0f, 10.0f, z);
    }
    ceilingBatch.End();

    leftWallBatch.Begin(GL_TRIANGLE_STRIP, 28, 1);
    for(z = 60.0f; z >= 0.0f; z -=10.0f)
    {
        leftWallBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
        leftWallBatch.Vertex3f(-10.0f, -10.0f, z);

        leftWallBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
        leftWallBatch.Vertex3f(-10.0f, 10.0f, z);

        leftWallBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
        leftWallBatch.Vertex3f(-10.0f, -10.0f, z - 10.0f);

        leftWallBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
        leftWallBatch.Vertex3f(-10.0f, 10.0f, z - 10.0f);
    }
    leftWallBatch.End();

    rightWallBatch.Begin(GL_TRIANGLE_STRIP, 28, 1);
    for(z = 60.0f; z >= 0.0f; z -=10.0f)
    {
        rightWallBatch.MultiTexCoord2f(0, 0.0f, 0.0f);
        rightWallBatch.Vertex3f(10.0f, -10.0f, z);

        rightWallBatch.MultiTexCoord2f(0, 0.0f, 1.0f);
        rightWallBatch.Vertex3f(10.0f, 10.0f, z);

        rightWallBatch.MultiTexCoord2f(0, 1.0f, 0.0f);
        rightWallBatch.Vertex3f(10.0f, -10.0f, z - 10.0f);

        rightWallBatch.MultiTexCoord2f(0, 1.0f, 1.0f);
        rightWallBatch.Vertex3f(10.0f, 10.0f, z - 10.0f);
    }
    rightWallBatch.End();
}

void GLWidget::keyPressEvent(QKeyEvent *e)
{

    switch( e->key() )
    {
        case Qt::Key_Down:
            viewZ -= 0.5f;
            break;

        case Qt::Key_Up:
            viewZ += 0.5f;
            break;

        }
    updateGL();
}

void GLWidget::createMenus()
{

    nearest = new QAction("GL_NEAREST", this);
    nearest->setCheckable(true);

    linear = new QAction("GL_LINEAR", this);
    linear->setCheckable(true);

    nearest_mipmap_nearest = new QAction("GL_NEAREST_MIPMAP_NEAREST", this);
    nearest_mipmap_nearest->setCheckable(true);

    nearest_mipmap_linear = new QAction("GL_NEAREST_MIPMAP_LINEAR", this);
    nearest_mipmap_linear->setCheckable(true);

    linear_mipmap_nearest = new QAction("GL_LINEAR_MIPMAP_NEAREST", this);
    linear_mipmap_nearest->setCheckable(true);

    linear_mipmap_linear = new QAction("GL_LINEAR_MIPMAP_LINEAR", this);
    linear_mipmap_linear->setCheckable(true);

    menuOptions = new QActionGroup(this);
    menuOptions->addAction(nearest);
    menuOptions->addAction(linear);
    menuOptions->addAction(nearest_mipmap_nearest);
    menuOptions->addAction(nearest_mipmap_linear);
    menuOptions->addAction(linear_mipmap_nearest);
    menuOptions->addAction(linear_mipmap_linear);

    nearest->setChecked(true);

    menu.addAction(nearest);
    menu.addAction(linear);
    menu.addAction(nearest_mipmap_nearest);
    menu.addAction(nearest_mipmap_linear);
    menu.addAction(linear_mipmap_nearest);
    menu.addAction(linear_mipmap_linear);

}

void GLWidget::contextMenuEvent(QContextMenuEvent *event)
{

    GLint iLoop;
    QAction* pItem = menu.exec(mapToGlobal(event->pos()));

    for(iLoop = 0; iLoop < TEXTURE_COUNT; iLoop++)
    {
        glBindTexture(GL_TEXTURE_2D, textures[iLoop]);
        if(pItem == nearest)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        }
        else if(pItem == linear)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        }
        else if(pItem == nearest_mipmap_nearest)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        }
        else if(pItem == nearest_mipmap_linear)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        }
        else if(pItem == linear_mipmap_nearest)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
        }
        else if(pItem == linear_mipmap_linear)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        }
    }
    updateGL();
}
