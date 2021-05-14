#-------------------------------------------------
#
# Project created by QtCreator 2012-04-16T14:12:16
#
#-------------------------------------------------



#
# Versão para matrix cheia utilizando a lib Eigen.
#
#


QT       += core

QT       -= gui


INCLUDEPATH += Eigen

#INCLUDEPATH += /usr/include/suitesparse /usr/include/atlas

#IBS += -lblas


#LIBS+= -lcholmod

TARGET = hbrbf_3d
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    HBRBF.cpp \
    MeshData.cpp \
    HBRBFEval.cpp \
    XCube.cpp \
    VTKExport.cpp \
    vecmath.cpp \
    polyg/SurfacePolygonizer.cpp \
    InverseMultiquadrics.cpp \
    Gaussian.cpp \
    Multiquadrics.cpp \
    GeneralInverseMultiquadrics.cpp \
    Solver.cpp

HEADERS += \
    HBRBF.h \
    MeshData.h \
    HBRBFEval.h \
    RBF.h \
    XCube.h \
    VTKExport.h \
    vecmath.h \
    SparseMatrix.h \
    polyg/point3d.h \
    polyg/MC.h \
    polyg/SurfacePolygonizer.h \
    InverseMultiquadrics.h \
    Gaussian.h \
    Multiquadrics.h \
    GeneralInverseMultiquadrics.h \
    Solver.h
