#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <GLTools/include/GLTools.h>   // OpenGL toolkit
#include <GLTools/include/GLMatrixStack.h>
#include <GLTools/include/GLFrame.h>
#include <GLTools/include/GLFrustum.h>
#include <GLTools/include/GLBatch.h>
#include <GLTools/include/GLGeometryTransform.h>
#include <GLTools/include/StopWatch.h>
#include <GLTools/include/GLShaderManager.h>

#include <math.h>
#include <stdio.h>

#include <QGLWidget>

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

    // Keep track of effects step

    void DrawSongAndDance(GLfloat yRot);   // Called to draw dancing objects
    bool LoadTGATexture(const char *szFileName, GLenum minFilter, GLenum magFilter, GLenum wrapMode);
    GLShaderManager     shaderManager;       // Shader Manager
    GLMatrixStack       modelViewMatrix;     // Modelview Matrix
    GLMatrixStack       projectionMatrix;    // Projection Matrix
    GLFrustum           viewFrustum;          // View Frustum
    GLGeometryTransform transformPipeline; // Geometry Transform Pipeline

    GLTriangleBatch     torusBatch;
    GLBatch             floorBatch;
    GLTriangleBatch     sphereBatch;
    GLFrame             cameraFrame;

    GLuint              uiTextures[3];



};
#endif
