#ifndef OPENGLCANVAS_H
#define OPENGLCANVAS_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLShader>
#include <QOpenGLBuffer>

#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>

#include <memory>
#include <iostream>

#include <tucanow/scene.hpp>

class OpenGLMediator;

class OpenGLCanvas: public QOpenGLWidget, QOpenGLFunctions
{
public:

    OpenGLCanvas();

    void setOpenGLMediator (OpenGLMediator* glmed);

    void createTube (std::vector<float> &vertex_coordinates, std::vector<unsigned int> &triangle_list, std::vector<float> &normal_list, bool finalRender );

    void loadGeneratedMesh(const QString meshName);
    void renderCylinder(QVector<QVector3D> points3D);
protected:

    void initializeGL() override;
    void resizeGL( int w, int h ) override;
    void paintGL () override;

    void initializeShaders();

    OpenGLMediator* glmediator = nullptr;

    void toggleHeadlight();
    void toggleWireframe();

    void keyPressEvent (QKeyEvent* event) override;

    void mousePressEvent (QMouseEvent* event) override;
    void mouseMoveEvent (QMouseEvent* event) override;
    void mouseReleaseEvent (QMouseEvent* event) override;
    void wheelEvent (QWheelEvent* event) override;

    void timerEvent(QTimerEvent *event);

private:

    std::shared_ptr<tucanow::Scene> tscene;


    std::shared_ptr<QOpenGLShaderProgram> program;
    std::shared_ptr<QOpenGLShader> shader;
    std::shared_ptr<QOpenGLBuffer> bf_vertices;

    int viewportWidth ;

    int viewportHeight ;

    int y_RotateTimer, x_RotateTimer = 0 ;

    bool rotateAnimation = true;
    bool sceneHasModel =false;

    int timerId;

    bool mouseIsClicked = false;

};

#endif // OPENGLCANVAS_H
