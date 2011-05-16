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

    GLFrame             viewFrame;
    GLFrustum           viewFrustum;
    GLMatrixStack       modelViewMatrix;
    GLMatrixStack       projectionMatrix;
    GLGeometryTransform transformPipeline;

    GLuint  grassShader;      // The perspective demonstration shader
    GLint   locMVP;                 // The location of the ModelViewProjection matrix uniform

    GLuint  length_texture;         //
    GLuint  orientation_texture;
    GLuint  grasspalette_texture;
    GLuint  grasscolor_texture;
    GLuint  bend_texture;

    GLuint  vao;                    // The VAO
    GLuint  vertexBuffer;           // Geometry VBO
};
#endif
