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

    GLFrame              viewFrame;                  // View, frame and transformation matrices
    GLFrustum            viewFrustum;
    GLMatrixStack        modelViewMatrix;
    GLMatrixStack        projectionMatrix;
    GLGeometryTransform  transformPipeline;

    GLuint               flockingUpdateProgram;      // The update program
    GLuint               flockingRenderProgram;      // The render program
    bool                 done;

    // Parameters
    // Size of flock. 1500 works fine on my low-end integrated GPU. If you have something
    // beefy, you can go to 10000 or higher. If you like TDRs, try adding a couple more
    // zeros.
    GLuint               flock_size;

    // Data management
    GLuint               update_vao[2];
    GLuint               render_vao[2];
    GLuint               flock_position[2];
    GLuint               flock_velocity[2];
    GLuint               flock_geometry;
    GLuint               position_tbo[2];
    GLuint               velocity_tbo[2];
    GLuint               update_vs;
    GLuint               render_vs;
    GLuint               render_gs;
    GLuint               render_fs;

    // Locations of some uniforms that are updated
    // Update program uniform locations
    GLint                timestep_loc;
    GLint                goal_location;
    // Render program uniform locations
    GLint                mvp_location;

};
#endif
