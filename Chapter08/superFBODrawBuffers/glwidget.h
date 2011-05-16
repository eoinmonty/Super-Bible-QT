#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <GLTools/include/GLTools.h>   // OpenGL toolkit
#include <GLTools/include/GLFrame.h>
#include <GLTools/include/GLShaderManager.h>
#include <GLTools/include/GLFrustum.h>
#include <GLTools/include/GLMatrixStack.h>
#include <GLTools/include/GLGeometryTransform.h>
#include <GLTools/include/StopWatch.h>

#include <Models/Ninja/sbm.h>
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

    GLsizei  screenWidth;        // Desired window or desktop width
    GLsizei  screenHeight;       // Desired window or desktop height

    GLboolean bFullScreen;       // Request to run full screen
    GLboolean bAnimated;         // Request for continual updates


    GLShaderManager     shaderManager;       // Shader Manager
    GLMatrixStack       modelViewMatrix;     // Modelview Matrix
    GLMatrixStack       projectionMatrix;    // Projection Matrix
    GLFrustum           viewFrustum;         // View Frustum
    GLGeometryTransform transformPipeline;   // Geometry Transform Pipeline
    GLFrame             cameraFrame;         // Camera frame

    GLTriangleBatch     torusBatch;
    GLTriangleBatch     sphereBatch;
    GLBatch             floorBatch;
    GLBatch             screenQuad;

    GLuint              textures[3];
    GLuint              processProg;
    GLuint              texBO[3];
    GLuint              texBOTexture;
    bool                bUseFBO;
    GLuint              fboName;
    GLuint              depthBufferName;
    GLuint              renderBufferNames[3];

    SBObject            ninja;
    GLuint              ninjaTex[1];

    bool bF2IsDown;

    void MoveCamera(void);
    void DrawWorld(GLfloat yRot);
    void UseProcessProgram(M3DVector4f vLightPos, M3DVector4f vColor, int textureUnit);
    bool LoadBMPTexture(const char *szFileName, GLenum minFilter, GLenum magFilter, GLenum wrapMode);
    static float* LoadFloatData(const char *szFile, int *count);

};
#endif
