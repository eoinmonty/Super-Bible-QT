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
    GLFrustum           viewFrustum;
    GLBatch             starsBatch;

    GLuint  starFieldShader;    // The point sprite shader
    GLint   locMVP;             // The location of the ModelViewProjection matrix uniform
    GLint   locTimeStamp;       // The location of the time stamp
    GLint   locTexture;         // The location of the  texture uniform

    GLuint  starTexture;        // The star texture texture object
};
#endif
