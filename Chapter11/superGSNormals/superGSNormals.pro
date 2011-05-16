#-------------------------------------------------
#
# Project created by QtCreator 2011-03-02T22:01:40
#
#-------------------------------------------------

QT       += core gui opengl

TARGET = superGSNormals
TEMPLATE = app
target.path=/usr/local/bin
INSTALLS=target

#LIBS += -L/usr/lib/libGLEW.so -lglut -lGLEW
SOURCES += main.cpp \
    glwidget.cpp \
    GLTools/src/GLBatch.cpp \
    GLTools/src/GLTriangleBatch.cpp \
    GLTools/src/GLShaderManager.cpp \
    GLTools/src/GLTools.cpp \
    GLTools/src/math3d.cpp

HEADERS  += \
    glwidget.h \
    GLTools/include/GLTools.h \
    GLTools/include/GLShaderManager.h \
    GLTools/include/GLBatch.h \
    GLTools/include/GLMatrixStack.h \
    GLTools/include/math3d.h \
    GLTools/include/GLBatchBase.h \
    GLTools/include/GLFrustum.h \
    GLTools/include/GLFrame.h \
    GLTools/include/GLGeometryTransform.h \
    GLTools/include/GLTriangleBatch.h \
    GLTools/include/StopWatch.h

FORMS    +=
