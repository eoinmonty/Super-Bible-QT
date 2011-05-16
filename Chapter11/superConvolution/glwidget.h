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
private:

    GLsizei     screenWidth;            // Desired window or desktop width
    GLsizei     screenHeight;            // Desired window or desktop height

    GLboolean bFullScreen;            // Request to run full screen

    GLMatrixStack        modelViewMatrix;        // Modelview Matrix
    GLMatrixStack        projectionMatrix;        // Projection Matrix
    GLFrustum            viewFrustum;            // View Frustum
    GLGeometryTransform  transformPipeline;        // Geometry Transform Pipeline
    GLFrame              cameraFrame;            // Camera frame
    GLBatch              screenQuad;
    M3DMatrix44f         orthoMatrix;

    GLuint               instancingProg;

    GLuint               absValueProg;

    GLuint               square_vao;
    GLuint               square_vbo;

    GLuint               sourceTexture;
    GLuint               gaussian_kernelTexture;
    GLuint               gaussian_kernelBuffer;

    GLuint               sobel_kernelTexture1;
    GLuint               sobel_kernelTexture2;
    GLuint               sobel_kernelBuffer1;
    GLuint               sobel_kernelBuffer2;

    GLuint               intermediateFBO;
    GLuint               intermediateTexture;

    GLuint               targetFBO;
    GLuint               targetTexture;

    GLfloat              kernelScale[2];

    void UpdateMode(void);
    void GenerateOrtho2DMat(GLuint imageWidth, GLuint imageHeight);
    bool LoadBMPTexture(const char *szFileName, GLenum minFilter, GLenum magFilter, GLenum wrapMode);
    bool LoadOpenEXRImage(char *fileName, GLint textureName, GLuint &texWidth, GLuint &texHeight);
    void GenTexCoordOffsets(GLuint width, GLuint height);
    void SetupTexReplaceProg(GLfloat *vLightPos, GLfloat *vColor);
    void SetupFlatColorProg(GLfloat *vLightPos, GLfloat *vColor);
    void SetupHDRProg();
    void SetupBlurProg();
};
#endif
