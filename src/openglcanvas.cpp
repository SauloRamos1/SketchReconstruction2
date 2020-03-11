#include <tucanow/misc.hpp>


#include "include/openglcanvas.h"

#include "qmath.h"

#include "openglmediator.h"


OpenGLCanvas::OpenGLCanvas()
{

}

void OpenGLCanvas::setOpenGLMediator (OpenGLMediator* glmed){

    glmediator = glmed;

}

void OpenGLCanvas::initializeGL(){

    initializeOpenGLFunctions();

    tucanow::misc::initGlew();

    initializeShaders();

    //glClearColor( 0.8f, 0.8f, 1.0,1.0);

    //    program = std::make_shared< QOpenGLShaderProgram >();
    //    program->create();

    //    shader = std::make_shared< QOpenGLShader >( QOpenGLShader::Vertex );
    //    shader->compileSourceFile( " " );

    //    bf_vertices = std::make_shared< QOpenGLBuffer >();
    //    bf_vertices->create();

}

void OpenGLCanvas::initializeShaders(){

    tscene = std::make_shared<tucanow::Scene>();

    float h = width();
    //tscene->initialize(width(), height());
    tscene->initialize(width(), h);
    tscene->setClearColor(0xF7, 0xF7, 0xF7);

    toggleHeadlight();


}

void OpenGLCanvas::createTube(std::vector<float> &vertex_coordinates, std::vector<unsigned int> &triangle_list, std::vector<float> &normal_list){


    tscene->setMesh(vertex_coordinates, triangle_list, normal_list);
    //tscene->setMeshColor(0xF1, 0xA3, 0x40);

    tscene->setMeshColor(0x00, 0x99, 0x33);



    update();
}

void OpenGLCanvas::loadGeneratedMesh (const QString meshName){

    tscene->loadMeshFromPLY(meshName.toStdString());

    tscene->setMeshColor(0xF1, 0xA3, 0x40);

}

void OpenGLCanvas::renderCylinder (QVector<QVector3D> points3D){



}



void OpenGLCanvas::resizeGL(int w, int h){

    tscene->setViewport(w,h);

}

void OpenGLCanvas::paintGL(){

    tscene->render();
    //glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLCanvas::toggleHeadlight()
{
    static bool headlight_mode = false;

    headlight_mode = !headlight_mode;
    tscene->setHeadlight(headlight_mode);
}

void OpenGLCanvas::toggleWireframe()
{
    static bool render_wireframe = false;

    render_wireframe = !render_wireframe;
    tscene->renderWireframe(render_wireframe);
}

void OpenGLCanvas::keyPressEvent(QKeyEvent *event){

    if (event->key() == Qt::Key_H){
        toggleHeadlight();
    }

    if (event->key() == Qt::Key_W){
        toggleWireframe();
    }
    update();
    QOpenGLWidget::keyPressEvent(event);
}

void OpenGLCanvas::mousePressEvent(QMouseEvent *event){

    setFocus();

    QPointF p = event->pos();
    if (event->buttons() == Qt::LeftButton){
        tscene->rotateCamera( p.x(), p.y() );
    }

    update();

    QOpenGLWidget::mousePressEvent( event );
}

void OpenGLCanvas::mouseMoveEvent( QMouseEvent* event )
{
    QPointF p = event->pos();

    if( event->buttons() == Qt::LeftButton ){

        tscene->rotateCamera( p.x(), p.y() );
    }

    update();

    QOpenGLWidget::mouseMoveEvent( event );

}

void OpenGLCanvas::mouseReleaseEvent(QMouseEvent *event){

    tscene->stopRotateCamera();

    update();

    QOpenGLWidget::mouseReleaseEvent( event );


}

void OpenGLCanvas::wheelEvent(QWheelEvent *event){

    if (event->delta() > 0){

        tscene->increaseCameraZoom();

    } else if ( event->delta() < 0 ){

        tscene->decreaseCameraZoom();
    }

    update();

    QOpenGLWidget::wheelEvent(event);

}








