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
    void contextMenuEvent(QContextMenuEvent *event);
private:

    void createMenus();
    GLShaderManager     shaderManager;       // Shader Manager
    GLMatrixStack       modelViewMatrix;     // Modelview Matrix
    GLMatrixStack       projectionMatrix;    // Projection Matrix
    GLFrustum           viewFrustum;         // View Frustum
    GLGeometryTransform transformPipeline;   // Geometry Transform Pipeline

    GLBatch             floorBatch;
    GLBatch             ceilingBatch;
    GLBatch             leftWallBatch;
    GLBatch             rightWallBatch;

    GLfloat             viewZ;

    QActionGroup *menuOptions;
    QActionGroup *menuAnisotropic;
    QAction* nearest;
    QAction* linear;
    QAction* nearest_mipmap_nearest;
    QAction* nearest_mipmap_linear;
    QAction* linear_mipmap_nearest;
    QAction* linear_mipmap_linear;
    QAction* anisotropic_on;
    QAction* anisotropic_off;

    QMenu menu;
};
#endif
