#include "canvasmediator.h"
#include "input_sketch2.h"

#include "include/canvas2.h"
#include "include/layers.h"

CanvasMediator::CanvasMediator(){

}

void CanvasMediator::sendClosedPaths( const QPainterPath pathToCanvas2, const int& lineLevel){


    if( canvas2 == nullptr || layers == nullptr ) return;

}


void CanvasMediator::closeAndSendPaths( const QVector<QPainterPath>& pathsToReconstruct, const int& lineLevel){

}



void CanvasMediator::setCanvas2(const std::shared_ptr<Canvas2> &cv2)
{
    canvas2 = cv2;
}

void CanvasMediator::setLayers(const std::shared_ptr<Layers> &ly)
{
    layers = ly;
}

void CanvasMediator::selectPath ( const int pathIndex ){

    canvas2->selectPath(pathIndex);
}

void CanvasMediator::sendPathsAndNames(QPainterPath path, int lineLevel){

    pathList.clear();
    pathList = canvas2->getPaths();
    lineLevels.append(lineLevel);

    QString namePath = QString();
    namePath.append("Path ");
    namePath.append(QString::number(pathList.size()));

    namePathList.append(namePath);
    pathList.append(path);

    canvas2->setCurvesSelection( pathList, namePathList, lineLevels );
    layers->receiveNamePaths( namePath );

}

void CanvasMediator::renamePath(int pathIndex, QString name){

    namePathList[pathIndex] = QString();
    namePathList[pathIndex] = name;

    canvas2->setCurvesSelection( pathList, namePathList, lineLevels );

}


bool CanvasMediator::almostEqual(QPointF a, QPointF b)
{
    //return fabs(a - b) <= FLT_EPSILON;
    return fabs(a.x() - b.x()) <= 2 && fabs(a.y() - b.y()) <=2 ;
}

QVector<QString> CanvasMediator::getPathNames(){

    return namePathList;

}

