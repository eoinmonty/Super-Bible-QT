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

    GLFrame              viewFrame;

    GLuint               juliaShader;        // The julia set renderer
    GLint                locC;               // The location of the Julia constant C uniform
    GLint                locZoom;            // Location of the zoom uniform
    GLint                locOffset;          // Location of the offset uniform

    GLuint               vao;                // The VAO
    GLuint               vertexBuffer;       // Geometry VBO

    GLuint               gradient_texture;   // The gradient texture

    bool                 paused;
    float                time_offset;
    float                zoom;
    float                x_offset;
    float                y_offset;
};
#endif
