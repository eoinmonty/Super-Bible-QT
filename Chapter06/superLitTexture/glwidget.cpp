#include "glwidget.h"

#define NUM_SPHERES 50
GLFrame spheres[NUM_SPHERES];

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
        modelViewMatrix.Rotate(rotTimer.GetElapsedSeconds() * 10.0f, 0.0f, 1.0f, 0.0f);

        GLfloat vEyeLight[] = { -100.0f, 100.0f, 100.0f };
        GLfloat vAmbientColor[] = { 0.2f, 0.2f, 0.2f, 1.0f };
        GLfloat vDiffuseColor[] = { 1.0f, 1.0f, 1.0f, 1.0f};
        GLfloat vSpecularColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };

        glBindTexture(GL_TEXTURE_2D, texture);
        glUseProgram(ADSTextureShader);
        glUniform4fv(locAmbient, 1, vAmbientColor);
        glUniform4fv(locDiffuse, 1, vDiffuseColor);
        glUniform4fv(locSpecular, 1, vSpecularColor);
        glUniform3fv(locLight, 1, vEyeLight);
        glUniformMatrix4fv(locMVP, 1, GL_FALSE, transformPipeline.GetModelViewProjectionMatrix());
        glUniformMatrix4fv(locMV, 1, GL_FALSE, transformPipeline.GetModelViewMatrix());
        glUniformMatrix3fv(locNM, 1, GL_FALSE, transformPipeline.GetNormalMatrix());
        glUniform1i(locTexture, 0);
    sphereBatch.Draw();

    modelViewMatrix.PopMatrix();

    // Do the buffer Swap
    swapBuffers();
}

void GLWidget::initializeGL ()
{
    // Background
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f );

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    shaderManager.InitializeStockShaders();
    viewFrame.MoveForward(4.0f);

    // Make the sphere
    gltMakeSphere(sphereBatch, 1.0f, 100, 50);

    ADSTextureShader = gltLoadShaderPairWithAttributes("ADSTexture.vp", "ADSTexture.fp", 3, GLT_ATTRIBUTE_VERTEX, "vVertex",
                                                        GLT_ATTRIBUTE_NORMAL, "vNormal", GLT_ATTRIBUTE_TEXTURE0, "vTexture0");

    locAmbient = glGetUniformLocation(ADSTextureShader, "ambientColor");
    locDiffuse = glGetUniformLocation(ADSTextureShader, "diffuseColor");
    locSpecular = glGetUniformLocation(ADSTextureShader, "specularColor");
    locLight = glGetUniformLocation(ADSTextureShader, "vLightPosition");
    locMVP = glGetUniformLocation(ADSTextureShader, "mvpMatrix");
    locMV  = glGetUniformLocation(ADSTextureShader, "mvMatrix");
    locNM  = glGetUniformLocation(ADSTextureShader, "normalMatrix");
    locTexture = glGetUniformLocation(ADSTextureShader, "colorMap");

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    LoadTGATexture("CoolTexture.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE);

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

