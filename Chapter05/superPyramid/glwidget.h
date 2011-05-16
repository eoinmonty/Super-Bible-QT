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


    void MakePyramid(GLBatch& pyramidBatch);
    bool LoadTGATexture(const char *szFileName, GLenum minFilter, GLenum magFilter, GLenum wrapMode);

    /////////////////////////////////////////////////////////////////////////////////
    // An assortment of needed classes
    GLShaderManager     shaderManager;
    GLMatrixStack       modelViewMatrix;
    GLMatrixStack       projectionMatrix;
    GLFrame             cameraFrame;
    GLFrame             objectFrame;
    GLFrustum           viewFrustum;

    GLBatch             pyramidBatch;

    GLuint              textureID;

    GLGeometryTransform transformPipeline;
    M3DMatrix44f        shadowMatrix;



};
#endif
