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
    glDeleteProgram(myTexturedIdentityShader);
    glDeleteTextures(1, &textureID);
}

void GLWidget::resizeGL(int w, int h)
{

    glViewport(0, 0, w, h);
}

void GLWidget::paintGL()
{
    // Clear the window with current clearing color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glUseProgram(myTexturedIdentityShader);
    glBindTexture(GL_TEXTURE_2D, textureID);
    GLint iTextureUniform = glGetUniformLocation(myTexturedIdentityShader, "colorMap");
    glUniform1i(iTextureUniform, 0);

    triangleBatch.Draw();
    // Do the buffer Swap
    swapBuffers();
}

void GLWidget::initializeGL ()
{
    // Blue background
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f );

    shaderManager.InitializeStockShaders();

    // Load up a triangle
    GLfloat vVerts[] = { -0.5f, 0.0f, 0.0f,
                         0.5f, 0.0f, 0.0f,
                         0.0f, 0.5f, 0.0f };

    GLfloat vTexCoords [] = { 0.0f, 0.0f,
                              1.0f, 0.0f,
                              0.5f, 1.0f };

    triangleBatch.Begin(GL_TRIANGLES, 3, 1);
    triangleBatch.CopyVertexData3f(vVerts);
    triangleBatch.CopyTexCoordData2f(vTexCoords, 0);
    triangleBatch.End();

    myTexturedIdentityShader = gltLoadShaderPairWithAttributes("TexturedIdentity.vp", "TexturedIdentity.fp", 2,
                                                               GLT_ATTRIBUTE_VERTEX, "vVertex", GLT_ATTRIBUTE_TEXTURE0, "vTexCoords");

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    LoadTGATexture("stone.tga", GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE);

}


// Load a TGA as a 2D Texture. Completely initialize the state
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
   glTexImage2D(GL_TEXTURE_2D, 0, nComponents, nWidth, nHeight, 0,
                eFormat, GL_UNSIGNED_BYTE, pBits);

   free(pBits);

   if(minFilter == GL_LINEAR_MIPMAP_LINEAR ||
      minFilter == GL_LINEAR_MIPMAP_NEAREST ||
      minFilter == GL_NEAREST_MIPMAP_LINEAR ||
      minFilter == GL_NEAREST_MIPMAP_NEAREST)
       glGenerateMipmap(GL_TEXTURE_2D);

   return true;
}

