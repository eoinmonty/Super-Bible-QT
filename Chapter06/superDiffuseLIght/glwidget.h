#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <GLTools/include/GLTools.h>   // OpenGL toolkit
#include <GLTools/include/GLFrame.h>
#include <GLTools/include/GLShaderManager.h>
#include <GLTools/include/GLFrustum.h>
#include <GLTools/include/GLMatrixStack.h>
#include <GLTools/include/GLGeometryTransform.h>
#include <GLTools/include/StopWatch.h>

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
private:

    GLFrame             viewFrame;
    GLFrustum           viewFrustum;
    GLTriangleBatch     sphereBatch;
    GLMatrixStack       modelViewMatrix;
    GLMatrixStack       projectionMatrix;
    GLGeometryTransform transformPipeline;
    GLShaderManager     shaderManager;

    GLuint  diffuseLightShader; // The diffuse light shader
    GLint   locColor;           // The location of the diffuse color
    GLint   locLight;           // The location of the Light in eye coordinates
    GLint   locMVP;             // The location of the ModelViewProjection matrix uniform
    GLint   locMV;              // The location of the ModelView matrix uniform
    GLint   locNM;              // The location of the Normal matrix uniform
};
#endif
