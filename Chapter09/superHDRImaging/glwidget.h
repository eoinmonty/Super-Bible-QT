#ifndef GLWIDGET_H
#define GLWIDGET_H


//#include <GL/glew.h>
#include <GLTools/include/GLTools.h>   // OpenGL toolkit
#include <GLTools/include/GLFrame.h>
#include <GLTools/include/GLShaderManager.h>
#include <GLTools/include/GLFrustum.h>
#include <GLTools/include/GLMatrixStack.h>
#include <GLTools/include/GLGeometryTransform.h>
#include <GLTools/include/StopWatch.h>

#include <ImfRgbaFile.h>            // OpenEXR headers
#include <ImfArray.h>
#include <QGLWidget>
#include <QTime>

class GLWidget : public QGLWidget {

    Q_OBJECT
public:
    GLWidget(QWidget *parent = 0);
    ~GLWidget();
protected:
    void paintGL ();
    void resizeGL(int w, int h);
    void initializeGL ();
    void keyPressEvent(QKeyEvent *e);
private:

    GLMatrixStack       modelViewMatrix;        // Modelview Matrix
    GLMatrixStack       projectionMatrix;       // Projection Matrix
    GLGeometryTransform transformPipeline;      // Geometry Transform Pipeline
    GLBatch             screenQuad;
    GLBatch             fboQuad;
    M3DMatrix44f        orthoMatrix;
    M3DMatrix44f        fboOrthoMatrix;

    GLsizei  screenWidth;        // Desired window or desktop width
    GLsizei  screenHeight;       // Desired window or desktop height

    GLboolean bFullScreen;       // Request to run full screen
    GLboolean bAnimated;         // Request for continual updates


    GLuint              hdrTextures[1];
    GLuint              lutTxtures[1];
    GLuint              fboTextures[1];
    GLuint              hdrTexturesWidth[1];
    GLuint              hdrTexturesHeight[1];
    GLuint              curHDRTex;
    GLuint              fboName;
    GLuint              mapTexProg;
    GLuint              varExposureProg;
    GLuint              adaptiveProg;
    GLuint              curProg;
    GLfloat             exposure;

    void GenerateOrtho2DMat(GLuint windowWidth, GLuint windowHeight, GLuint imageWidth, GLuint imageHeight);
    void GenerateFBOOrtho2DMat(GLuint imageWidth, GLuint imageHeight);
    void SetupHDRProg();
    void SetupStraightTexProg();
    bool LoadOpenEXRImage(char *fileName, GLint textureName, GLuint &texWidth, GLuint &texHeight);

};
#endif
