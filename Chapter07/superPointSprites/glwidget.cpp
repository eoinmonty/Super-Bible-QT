#include "glwidget.h"


#define NUM_STARS 1000


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
    glDeleteTextures(1, &starTexture);
}

void GLWidget::resizeGL(int w, int h)
{
    // Prevent a divide by zero
    if(h == 0)
        h = 1;

    // Set Viewport to window dimensions
    glViewport(0, 0, w, h);

    viewFrustum.SetPerspective(35.0f, float(w)/float(h), 1.0f, 1000.0f);

}

void GLWidget::paintGL()
{
    static CStopWatch timer;

    // Clear the window and the depth buffer
    glClear(GL_COLOR_BUFFER_BIT);

    // Turn on additive blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    // Let the vertex program determine the point size
    glEnable(GL_PROGRAM_POINT_SIZE);

    // Bind to our shader, set uniforms
    glUseProgram(starFieldShader);
    glUniformMatrix4fv(locMVP, 1, GL_FALSE, viewFrustum.GetProjectionMatrix());
    glUniform1i(locTexture, 0);

    // fTime goes from 0.0 to 999.0 and recycles
    float fTime = timer.GetElapsedSeconds() * 50.0f;
    fTime = fmod(fTime, 999.0f);
    glUniform1f(locTimeStamp, fTime);

    // Draw the stars
    starsBatch.Draw();
    // Do the buffer Swap
    swapBuffers();
}

void GLWidget::initializeGL ()
{
    // Background
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f );

    // A number of shipping drivers are not conformant to the current OpenGL
    // spec and require this. NVidia... in particular. The OpenGL specification
    // states that this is always "on", in fact you can't enable or disable it
    // anymore. Adding this lines "fixes" this on non-conformant drivers, but
    // be aware, if you have a pure core (and working correctly) GL context,·
    //you should not do this
    glEnable(GL_POINT_SPRITE);

    GLfloat fColors[4][4] = {{ 1.0f, 1.0f, 1.0f, 1.0f}, // White
                             { 0.67f, 0.68f, 0.82f, 1.0f}, // Blue Stars
                             { 1.0f, 0.5f, 0.5f, 1.0f}, // Reddish
                             { 1.0f, 0.82f, 0.65f, 1.0f}}; // Orange


    // Randomly place the stars in their initial positions, and pick a random color
    starsBatch.Begin(GL_POINTS, NUM_STARS);
    for(int i = 0; i < NUM_STARS; i++)
    {
        int iColor = 0;   // All stars start as white

        // One in five will be blue
        if(rand() % 5 == 1)
            iColor = 1;

        // One in 50 red
        if(rand() % 50 == 1)
           iColor = 2;

        // One in 100 is amber
        if(rand() % 100 == 1)
           iColor = 3;


        starsBatch.Color4fv(fColors[iColor]);

        M3DVector3f vPosition;
        vPosition[0] = float(3000 - (rand() % 6000)) * 0.1f;
        vPosition[1] = float(3000 - (rand() % 6000)) * 0.1f;
        vPosition[2] = -float(rand() % 1000)-1.0f;  // -1 to -1000.0f

        starsBatch.Vertex3fv(vPosition);
    }
    starsBatch.End();


    starFieldShader = gltLoadShaderPairWithAttributes("SpaceFlight.vp", "SpaceFlight.fp", 2, GLT_ATTRIBUTE_VERTEX, "vVertex",
                                                       GLT_ATTRIBUTE_COLOR, "vColor");

    locMVP = glGetUniformLocation(starFieldShader, "mvpMatrix");
    locTexture = glGetUniformLocation(starFieldShader, "starImage");
    locTimeStamp = glGetUniformLocation(starFieldShader, "timeStamp");

    glGenTextures(1, &starTexture);
    glBindTexture(GL_TEXTURE_2D, starTexture);
    LoadTGATexture("star.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE);

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
