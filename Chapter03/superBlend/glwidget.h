#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <GLTools/include/GLTools.h>   // OpenGL toolkit
#include <GLTools/include/GLShaderManager.h>    // Shader Manager Class

#include <QGLWidget>
#include <QTime>

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

    GLBatch squareBatch;
    GLBatch greenBatch;
    GLBatch redBatch;
    GLBatch blueBatch;
    GLBatch blackBatch;

    GLShaderManager shaderManager;

};
#endif
