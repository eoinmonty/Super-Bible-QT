#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <GLTools/include/GLTools.h>            // OpenGL toolkit
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
private:
    void BounceFunction();
};
#endif
