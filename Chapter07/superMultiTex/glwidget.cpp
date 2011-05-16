#include "glwidget.h"


// Six sides of a cube map
const char *szCubeFaces[6] = { "pos_x.tga", "neg_x.tga", "pos_y.tga", "neg_y.tga", "pos_z.tga", "neg_z.tga" };

GLenum  cube[6] = {  GL_TEXTURE_CUBE_MAP_POSITIVE_X,
                     GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
                     GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
                     GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
                     GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
                     GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };


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
    glDeleteTextures(1, &cubeTexture);
}

void GLWidget::resizeGL(int w, int h)
{
    // Prevent a divide by zero
    if(h == 0)
        h = 1;

    // Set Viewport to window dimensions
    glViewport(0, 0, w, h);

    viewFrustum.SetPerspective(60.0f, float(w)/float(h), 1.0f, 10000.0f);

    projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
    transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);

}

void GLWidget::paintGL()
{
    // Clear the window
     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

     M3DMatrix44f mCamera;
     M3DMatrix44f mCameraRotOnly;
     M3DMatrix44f mInverseCamera;

     viewFrame.GetCameraMatrix(mCamera, false);
     viewFrame.GetCameraMatrix(mCameraRotOnly, true);
     m3dInvertMatrix44(mInverseCamera, mCameraRotOnly);

     modelViewMatrix.PushMatrix();
         // Draw the sphere
         modelViewMatrix.MultMatrix(mCamera);
         glUseProgram(reflectionShader);
         glUniformMatrix4fv(locMVPReflect, 1, GL_FALSE, transformPipeline.GetModelViewProjectionMatrix());
         glUniformMatrix4fv(locMVReflect, 1, GL_FALSE, transformPipeline.GetModelViewMatrix());
         glUniformMatrix3fv(locNormalReflect, 1, GL_FALSE, transformPipeline.GetNormalMatrix());
         glUniformMatrix4fv(locInvertedCamera, 1, GL_FALSE, mInverseCamera);
         glUniform1i(locCubeMap, 0);
         glUniform1i(locTarnishMap, 1);

         glEnable(GL_CULL_FACE);
         sphereBatch.Draw();
         glDisable(GL_CULL_FACE);
     modelViewMatrix.PopMatrix();

     modelViewMatrix.PushMatrix();
         modelViewMatrix.MultMatrix(mCameraRotOnly);
         glUseProgram(skyBoxShader);
         glUniformMatrix4fv(locMVPSkyBox, 1, GL_FALSE, transformPipeline.GetModelViewProjectionMatrix());
         cubeBatch.Draw();
    modelViewMatrix.PopMatrix();

    // Do the buffer Swap
    swapBuffers();
}

void GLWidget::initializeGL ()
{
    GLbyte *pBytes;
    GLint iWidth, iHeight, iComponents;
    GLenum eFormat;
    int i;

    // Cull backs of polygons
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_DEPTH_TEST);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Load the tarnish texture
    glGenTextures(1, &tarnishTexture);
    glBindTexture(GL_TEXTURE_2D, tarnishTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    pBytes = gltReadTGABits("tarnish.tga", &iWidth, &iHeight, &iComponents, &eFormat);
    glTexImage2D(GL_TEXTURE_2D, 0, iComponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBytes);
    free(pBytes);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Load the cube map····
    glGenTextures(1, &cubeTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTexture);

    // Set up texture maps········
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // Load Cube Map images
    for(i = 0; i < 6; i++)
    {
        // Load this texture map
        pBytes = gltReadTGABits(szCubeFaces[i], &iWidth, &iHeight, &iComponents, &eFormat);
        glTexImage2D(cube[i], 0, iComponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBytes);
        free(pBytes);
    }
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    viewFrame.MoveForward(-4.0f);
    gltMakeSphere(sphereBatch, 1.0f, 52, 26);
    gltMakeCube(cubeBatch, 20.0f);

    reflectionShader = gltLoadShaderPairWithAttributes("Reflection.vp", "Reflection.fp", 3,
                                                GLT_ATTRIBUTE_VERTEX, "vVertex",
                                                GLT_ATTRIBUTE_NORMAL, "vNormal",
                                                GLT_ATTRIBUTE_TEXTURE0, "vTexCoords");

    locMVPReflect = glGetUniformLocation(reflectionShader, "mvpMatrix");
    locMVReflect = glGetUniformLocation(reflectionShader, "mvMatrix");
    locNormalReflect = glGetUniformLocation(reflectionShader, "normalMatrix");
    locInvertedCamera = glGetUniformLocation(reflectionShader, "mInverseCamera");
    locCubeMap = glGetUniformLocation(reflectionShader, "cubeMap");
    locTarnishMap = glGetUniformLocation(reflectionShader, "tarnishMap");

    skyBoxShader = gltLoadShaderPairWithAttributes("SkyBox.vp", "SkyBox.fp", 2,
                                                GLT_ATTRIBUTE_VERTEX, "vVertex",
                                                GLT_ATTRIBUTE_NORMAL, "vNormal");

    locMVPSkyBox = glGetUniformLocation(skyBoxShader, "mvpMatrix");

    // Set textures to their texture units
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tarnishTexture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTexture);

}

void GLWidget::keyPressEvent(QKeyEvent *e)
{
    switch( e->key() )
    {
        case Qt::Key_Down:
            viewFrame.MoveForward(-0.1f);
            break;

        case Qt::Key_Up:
            viewFrame.MoveForward(0.1f);
            break;

        case Qt::Key_Left:
            viewFrame.RotateLocalY(0.1);
            break;

        case Qt::Key_Right:
            viewFrame.RotateLocalY(-0.1);
            break;
        }
        updateGL();
}
