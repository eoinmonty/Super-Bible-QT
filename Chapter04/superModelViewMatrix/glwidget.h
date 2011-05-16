#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <GLTools/include/GLTools.h>   // OpenGL toolkit
#include <GLTools/include/GLMatrixStack.h>
#include <GLTools/include/GLFrame.h>
#include <GLTools/include/GLFrustum.h>
#include <GLTools/include/GLBatch.h>
#include <GLTools/include/GLGeometryTransform.h>
#include <GLTools/include/StopWatch.h>

#include <QGLWidget>
#include <QVector>

class GLWidget : public QGLWidget {

    Q_OBJECT
public:
    GLWidget(QWidget *parent = 0);
    ~GLWidget();
protected:
    void paintGL ();
    void resizeGL(int w, int h);
    void initializeGL ();
private:

    // Keep track of effects step
    int nStep;

    GLShaderManager     shaderManager;
    GLMatrixStack       modelViewMatrix;
    GLMatrixStack       projectionMatrix;
    GLFrame             cameraFrame;
    GLFrame             objectFrame;
    GLFrustum           viewFrustum;

    GLTriangleBatch     sphereBatch;
    GLTriangleBatch     torusBatch;
    GLTriangleBatch     cylinderBatch;
    GLTriangleBatch     coneBatch;
    GLTriangleBatch     diskBatch;


    GLGeometryTransform transformPipeline;
    M3DMatrix44f        shadowMatrix;

};
#endif
