#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <GLTools/include/GLTools.h>   // OpenGL toolkit
#include <GLTools/include/GLMatrixStack.h>
#include <GLTools/include/GLFrame.h>
#include <GLTools/include/GLFrustum.h>
#include <GLTools/include/GLBatch.h>
#include <GLTools/include/GLGeometryTransform.h>

#include <QGLWidget>
#include <GLTools/include/GLShaderManager.h>    // Shader Manager Class

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
    void mouseReleaseEvent (QMouseEvent * e);
private:

    void Popup(const QPoint& pt);
    GLFrame             viewFrame;
    GLFrustum           viewFrustum;
    GLTriangleBatch     torusBatch;
    GLMatrixStack       modelViewMatix;
    GLMatrixStack       projectionMatrix;
    GLGeometryTransform transformPipeline;

    // Flags for effects
    int iCull;
    int iDepth;
    GLShaderManager     shaderManager;

//private slots:
    void togDepth();
    void togCull();
    void setFill();
    void setLine();
    void setPoint();

};
#endif
