#include <QDebug>

#include "scene2.h"

Scene2::Scene2()
{
    setSceneRect(QRect (0,0,500,500));

    setBackgroundBrush(QColor(230,255,230));

    //addEllipse( 0, 0, 10, 10 );

    addRect(QRect(0,0,500,500), QPen(QColor(179,179,179),1,Qt::SolidLine), Qt::white);
    addItem ( &sketch2 );

}

void Scene2::setCurvesSelection( const QVector<QPainterPath>& pathList, const QVector<QString>& namePathList, const QVector<int>& lineLevels )
{

    sketch2.receiveSelectedPath ( pathList, namePathList, lineLevels );
    update();

}


void Scene2::selectPath(const int pathIndex){

    sketch2.selectPath(pathIndex);
    update ();
}


void Scene2::setOversketchingMode(){


    if (status == Interaction::OVERSKETCHING_MODE){
        status = Interaction::SKETCH;
        sketch2.interactionMode = "Selection Mode";
        qDebug () << "Selection Mode";
    } else {
        status = Interaction::OVERSKETCHING_MODE;
        sketch2.interactionMode = "Oversketching Mode";
    }

    qDebug () << "Oversketching Mode";

    update();

}

void Scene2::setDefRotAxisMode(){

    if (status == Interaction::DEFINE_ROT_AXIS_MODE){
        status = Interaction::SKETCH;
        sketch2.interactionMode = "Selection Mode";
        qDebug () << "Selection  Mode";
    } else {
        status = Interaction::DEFINE_ROT_AXIS_MODE;
        sketch2.interactionMode = "Define Rotation Axis Mode";
    }

    qDebug () << "Define Rot Axis Mode";
    update();

}

void Scene2::setCrossSecBlendSurfaceMode(){

    if (status == Interaction::DEFINE_CROSS_SEC_MODE){
        status = Interaction::SKETCH;
        sketch2.interactionMode = "Selection Mode";
        qDebug () << "Selection  Mode";
    } else {
        status = Interaction::DEFINE_CROSS_SEC_MODE;
        sketch2.interactionMode = "Define Cross Sectional Blend Surface Mode";
    }

    qDebug () << "Define Rot Axis Mode";
    update();

}

void Scene2::smoothSketch()
{
    sketch2.smooth();
    update();

}

void Scene2::estimateShape(){



    sketch2.estimateShape();
    update();

}

void Scene2::showNormals(){

    sketch2.showNormals();
    update();

}

void Scene2::exportMesh(){

    sketch2.exportMesh();

    update();

}

void Scene2::mousePressEvent(QGraphicsSceneMouseEvent *event){


    if( event->button() == Qt::LeftButton && status == Interaction::OVERSKETCHING_MODE)
    {
        setFocus();

        QPointF pos = event->scenePos();
        sketch2.create( pos );
        update();
        QGraphicsScene::mousePressEvent(event);
    }

}


void Scene2::mouseMoveEvent(QGraphicsSceneMouseEvent *event){


    if( event->buttons() == Qt::LeftButton && status == Interaction::OVERSKETCHING_MODE)
    {
        QPointF pos = event->scenePos();
        sketch2.add( pos );


        update();
        QGraphicsScene::mouseMoveEvent(event);
    }


    update();

}

void Scene2::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){

    QPointF pos = event->scenePos();

    if (status == Interaction::OVERSKETCHING_MODE){
        static bool first_run = true;
        bool sketch_has_changed = sketch2.joinPaths();
        update ();
        QGraphicsScene::mouseReleaseEvent(event);
    }

    if (status == Interaction::DEFINE_ROT_AXIS_MODE){

        //sketch2.defRotAxis();

        update ();
        QGraphicsScene::mouseReleaseEvent(event);

    }


    update();


}

void Scene2::wheelEvent(QGraphicsSceneWheelEvent* event){

    if (status == Interaction::DEFINE_ROT_AXIS_MODE){
        if ( event->delta() > 0 ) {

            sketch2.defRotAxis(1);
            update();

        } else if (event->delta() < 0) {

            sketch2.defRotAxis(-1);
            update();
        }
    }

    if (status == Interaction::DEFINE_CROSS_SEC_MODE){
        if ( event->delta() > 0 ) {

            sketch2.defCrossSecToEdit(1);
            update();

        } else if (event->delta() < 0) {

            sketch2.defCrossSecToEdit(-1);
            update();
        }
    }




}

QVector<QPainterPath> Scene2::getPaths(){

    return sketch2.getPaths();

}

QPainterPath Scene2::getCurve(){

    return sketch2.getCurve();

}

QVector<QVector3D> Scene2::getPoints3D(){

    return sketch2.getPoints3D();
}

QVector<QVector3D> Scene2::getNormals3D(){

    return sketch2.getNormals3D();
}


