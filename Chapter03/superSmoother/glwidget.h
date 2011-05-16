#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <GLTools/include/GLTools.h>   // OpenGL toolkit
#include <GLTools/include/GLFrustum.h>

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
    void contextMenuEvent(QContextMenuEvent *event);
private:

    void createMenus();
    GLShaderManager shaderManager;
    GLFrustum viewFrustum;
    GLBatch smallStarBatch;
    GLBatch mediumStarBatch;
    GLBatch largeStarBatch;
    GLBatch mountainRangeBatch;
    GLBatch moonBatch;

    QActionGroup *menuOptions;
    QAction* alias;
    QAction* antialias;

    QMenu menu;

};
#endif
