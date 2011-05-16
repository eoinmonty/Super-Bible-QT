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
    void keyPressEvent(QKeyEvent *e);
private:

    GLFrame             viewFrame;
    GLFrustum           viewFrustum;
    GLTriangleBatch     sphereBatch;
    GLBatch             cubeBatch;
    GLMatrixStack       modelViewMatrix;
    GLMatrixStack       projectionMatrix;
    GLGeometryTransform transformPipeline;
    GLuint              cubeTexture;
    GLuint              tarnishTexture;
    GLint               reflectionShader;
    GLint               skyBoxShader;

    GLint               locMVPReflect, locMVReflect, locNormalReflect, locInvertedCamera;
    GLint               locCubeMap, locTarnishMap;
    GLint               locMVPSkyBox;

};
#endif
