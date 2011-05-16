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

    bool LoadTGATexture(const char *szFileName, GLenum minFilter, GLenum magFilter, GLenum wrapMode);

    GLFrame             viewFrame;
    GLFrustum           viewFrustum;
    GLTriangleBatch     torusBatch;
    GLMatrixStack       modelViewMatrix;
    GLMatrixStack       projectionMatrix;
    GLGeometryTransform transformPipeline;
    GLShaderManager     shaderManager;

    GLuint  sphereMapShader;    // The dissolving light shader
    GLint   locMVP;             // The location of the ModelViewProjection matrix uniform
    GLint   locMV;              // The location of the Modelview matrix
    GLint   locNormalMatrix;    // The location of the Normal Matrix
    GLint   locTexture;         // The location of the  texture uniform

    GLuint  sphereTexture;      // The cloud texture texture object

};
#endif
