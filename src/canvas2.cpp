#include "include/canvas2.h"

#include "canvasmediator.h"
#include "openglmediator.h"


Canvas2::Canvas2()
{

    scene2 = std::make_shared< Scene2 >();

    setScene( scene2.get() );

    setMouseTracking( true );
    setInteractive( true );
    setAcceptDrops( true );

}

void Canvas2::setMediator( CanvasMediator* med )
{
    mediator = med;
}

void Canvas2::setOpenGLMediator (OpenGLMediator* glmed){

    glmediator = glmed;

}


void Canvas2::setOversketchingMode( ){


    if( scene2->getCurve().isEmpty() == true ) {
        QMessageBox msgBox;
        msgBox.setText("No contour was selected.");
        msgBox.setInformativeText("Please select a curve before oversketching it");
        msgBox.exec();


        return;
    }

    scene2->setOversketchingMode();

}

void Canvas2::setDefRotAxisMode( ){


    if( scene2->getCurve().isEmpty() == true ) {
        QMessageBox msgBox;
        msgBox.setText("No contour was selected.");
        msgBox.setInformativeText("Please select a curve before define his rotation axis");
        msgBox.exec();


        return;
    }

    scene2->setDefRotAxisMode();

}

void Canvas2::setCrossSecBlendSurfaceMode( ){


    if( scene2->getCurve().isEmpty() == true ) {
        QMessageBox msgBox;
        msgBox.setText("No contour was selected.");
        msgBox.setInformativeText("Please select a curve before define his Cross Sectional Blending Surface");
        msgBox.exec();


        return;
    }

    scene2->setCrossSecBlendSurfaceMode();

}
void Canvas2::smoothSketch(){

    if (scene2->getCurve().isEmpty()){
        QMessageBox msgBox;
        msgBox.setText("No contour was selected.");
        msgBox.setInformativeText("Please select a curve before smoothing it");
        msgBox.exec();

        return;
    }

    scene2->smoothSketch();

}

void Canvas2::estimateShape(){


    if ( scene2->getPaths().isEmpty() ){

        QMessageBox msgBox;
        msgBox.setText("No contour was selected.");
        msgBox.setInformativeText("Please select a curve before estimate it's normals");
        msgBox.exec();

        return;
    }

    scene2->estimateShape();

    update();
}

void Canvas2::showNormals(){

    scene2->showNormals();

    update();

}

void Canvas2::exportMesh(){

    //scene2->exportMesh();
    glmediator->exportMesh();
    update();
}

void Canvas2::view3DPoints(){

    qDebug () << "view3DPoints";
    if (glmediator == nullptr) return;

    //glmediator->viewSketch3D(scene2->getPaths(), mediator->getPathNames() );

    glmediator->viewClosedContours3D(scene2->getPoints3D(), scene2->getNormals3D() );

}


void Canvas2::setCurvesSelection( const QVector<QPainterPath>& pathList, const QVector<QString>& namePathList , const QVector<int>& lineLevels){
    scene2->setCurvesSelection( pathList, namePathList, lineLevels );

}

void Canvas2::selectPath( const int pathIndex ){

    scene2->selectPath(pathIndex);

}

QVector<QPainterPath> Canvas2::getPaths(){

    return scene2->getPaths();
}

#include "moc_canvas2.cpp"
