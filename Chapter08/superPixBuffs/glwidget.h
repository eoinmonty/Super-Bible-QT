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

    GLsizei  screenWidth;           // Desired window or desktop width
    GLsizei  screenHeight;          // Desired window or desktop height

    GLboolean bFullScreen;          // Request to run full screen
    GLboolean bAnimated;            // Request for continual updates


    GLShaderManager     shaderManager;          // Shader Manager
    GLMatrixStack       modelViewMatrix;        // Modelview Matrix
    GLMatrixStack       projectionMatrix;       // Projection Matrix
    M3DMatrix44f        orthoMatrix;
    GLFrustum           viewFrustum;            // View Frustum
    GLGeometryTransform transformPipeline;      // Geometry Transform Pipeline
    GLFrame             cameraFrame;            // Camera frame

    GLTriangleBatch     torusBatch;
    GLBatch             floorBatch;
    GLBatch             screenQuad;

    GLuint              textures[1];
    GLuint              blurTextures[6];
    GLuint              pixBuffObjs[1];
    GLuint              curBlurTarget;
    bool                bUsePBOPath;
    GLfloat             speedFactor;
    GLuint              blurProg;
    void                *pixelData;
    GLuint              pixelDataSize;

    void MoveCamera(void);
    void DrawWorld(GLfloat yRot, GLfloat xPos);
    bool LoadBMPTexture(const char *szFileName, GLenum minFilter, GLenum magFilter, GLenum wrapMode);

    void SetupBlurProg(void);


    void UpdateFrameCount();

    void   AdvanceBlurTaget();
    GLuint GetBlurTarget0();
    GLuint GetBlurTarget1();
    GLuint GetBlurTarget2();
    GLuint GetBlurTarget3();
    GLuint GetBlurTarget4();
    GLuint GetBlurTarget5();
};
#endif
