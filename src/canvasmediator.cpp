#include "canvasmediator.h"
//#include "input_sketch2.h"

//#include "include/canvas2.h"
#include "include/layers.h"

CanvasMediator::CanvasMediator(){

}

void CanvasMediator::setCanvas(const std::shared_ptr<Canvas> &cv)
{
    canvas = cv;
}


void CanvasMediator::setLayers(const std::shared_ptr<Layers> &ly)
{
    layers = ly;
}


void CanvasMediator::sendNames(QString name){

    if( canvas == nullptr || layers == nullptr ) return;
    layers->receiveNamePaths( name );

    qDebug () << name;

}


void CanvasMediator::selectPath ( const int pathIndex ){

    //canvas2->selectPath(pathIndex);
}


void CanvasMediator::sendPathsAndNames(QPainterPath path, int lineLevel){

    pathList.clear();
    //pathList = canvas2->getPaths();
    lineLevels.append(lineLevel);

    QString namePath = QString();
    namePath.append("Path ");
    namePath.append(QString::number(pathList.size()));

    namePathList.append(namePath);
    pathList.append(path);

    //canvas2->setCurvesSelection( pathList, namePathList, lineLevels );
    layers->receiveNamePaths( namePath );

}


void CanvasMediator::renamePath(int pathIndex, QString name){

    namePathList[pathIndex] = QString();
    namePathList[pathIndex] = name;

    //canvas2->setCurvesSelection( pathList, namePathList, lineLevels );

}


bool CanvasMediator::almostEqual(QPointF a, QPointF b)
{
    //return fabs(a - b) <= FLT_EPSILON;
    return fabs(a.x() - b.x()) <= 2 && fabs(a.y() - b.y()) <=2 ;
}

QVector<QString> CanvasMediator::getPathNames(){

    return namePathList;

}

