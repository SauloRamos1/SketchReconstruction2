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

    //QString mesh = "output/Open0.ply";
    //loadGeneratedMesh(mesh);

    //mesh = "output/Open1.ply";
    //loadGeneratedMesh(mesh);

    //glEnable(GL_CULL_FACE);

    //glCullFace(GL_FRONT);

    //glClearColor( 0.8f, 0.8f, 1.0,1.0);

    //    program = std::make_shared< QOpenGLShaderProgram >();
    //    program->create();

    //    shader = std::make_shared< QOpenGLShader >( QOpenGLShader::Vertex );
    //    shader->compileSourceFile( " " );

    //    bf_vertices = std::make_shared< QOpenGLBuffer >();
    //    bf_vertices->create();
    timerId = startTimer(25);
}

void OpenGLCanvas::initializeShaders(){

    tscene = std::make_shared<tucanow::Scene>();

    float h = width();
    //tscene->initialize(width(), height());
    tscene->initialize(width(), h);
    tscene->setClearColor(0xF7, 0xF7, 0xF7);

    toggleHeadlight();

}

void OpenGLCanvas::createTube(std::vector<float> &vertex_coordinates, std::vector<unsigned int> &triangle_list, std::vector<float> &normal_list, bool finalRender){


    tscene->setMesh(vertex_coordinates, triangle_list, normal_list);
    //tscene->setMeshColor(0xF1, 0xA3, 0x40);
    if (finalRender){

        tscene->setMeshColor(0x3c, 0x99, 0xe6);
    } else {

        tscene->setMeshColor(0x00, 0x99, 0x33);
    }

    tscene->resetCamera();
    tscene->rotateCamera(viewportWidth/2 , 720 );
//    for (int i = 720; i > 0; --i) {
//        tscene->rotateCamera(viewportWidth/2 , i );
//    }
    tscene->rotateCamera(viewportWidth/2 , 0 );
    tscene->stopRotateCamera();

    if (vertex_coordinates.size() > 0){
        sceneHasModel = true;
    } else{
        sceneHasModel = false;
    }



    update();
}

void OpenGLCanvas::loadGeneratedMesh (const QString meshName){



    tscene->loadMeshFromPLY(meshName.toStdString());

    tscene->setMeshColor(0xF1, 0xA3, 0x40);

    toggleHeadlight();
}

void OpenGLCanvas::renderCylinder (QVector<QVector3D> points3D){



}



void OpenGLCanvas::resizeGL(int w, int h){

    viewportWidth = w;
    viewportHeight = h;

    y_RotateTimer = viewportHeight/2;

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

    if (event->key() == Qt::Key_R){
        tscene->resetCamera();
    }
    if (event->key() == Qt::Key_H){
        toggleHeadlight();
    }

    if (event->key() == Qt::Key_W){
        toggleWireframe();
    }

    if (event->key() == Qt::Key_R){
        rotateAnimation = !rotateAnimation;
    }

    update();
    QOpenGLWidget::keyPressEvent(event);
}

void OpenGLCanvas::mousePressEvent(QMouseEvent *event){

    setFocus();
    mouseIsClicked = true;

    tscene->stopRotateCamera();

    QPointF p = event->pos();
    if (event->buttons() == Qt::LeftButton){
        tscene->rotateCamera( p.x(), p.y() );

    }
    if (event->buttons() == Qt::MiddleButton){
        tscene->translateCamera( p.x(), p.y() );
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


    if (event->buttons() == Qt::MiddleButton){
        tscene->translateCamera( p.x(), p.y() );
    }

    update();

    QOpenGLWidget::mouseMoveEvent( event );

}

void OpenGLCanvas::mouseReleaseEvent(QMouseEvent *event){

    tscene->stopRotateCamera();
    tscene->stopTranslateCamera();

    mouseIsClicked = false;
    update();

    QOpenGLWidget::mouseReleaseEvent( event );


}

void OpenGLCanvas::wheelEvent(QWheelEvent *event){

//    if (event->delta() > 0){ event->

//        tscene->increaseCameraZoom();

//    } else if ( event->delta() < 0 ){

//        tscene->decreaseCameraZoom();
//    }

        if (event->angleDelta().y() > 0){

            tscene->increaseCameraZoom();

        } else if ( event->angleDelta().y() < 0 ){

            tscene->decreaseCameraZoom();
        }

    update();

    QOpenGLWidget::wheelEvent(event);

}


void OpenGLCanvas::timerEvent(QTimerEvent *event)
{

    if (!sceneHasModel || mouseIsClicked || rotateAnimation == false) return;

    tscene->rotateCamera(x_RotateTimer , y_RotateTimer);
    x_RotateTimer++;
    x_RotateTimer++;
    x_RotateTimer++;
    x_RotateTimer++;

    if (x_RotateTimer > viewportWidth-50){
        tscene->stopRotateCamera();
        x_RotateTimer = 50;
    }
    update();



}





