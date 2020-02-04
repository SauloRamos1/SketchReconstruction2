#include "canvasmediator.h"
#include "input_sketch2.h"

#include "include/canvas2.h"
#include "include/layers.h"

CanvasMediator::CanvasMediator(){

}

void CanvasMediator::sendClosedPaths( const QPainterPath pathToCanvas2, const int& lineLevel){


    if( canvas2 == nullptr || layers == nullptr ) return;
    if(pathToCanvas2.isEmpty()) return;

    // Check if it is closed

    if ( almostEqual( pathToCanvas2.pointAtPercent(0),pathToCanvas2.pointAtPercent(1) ) ){

        sendPathsAndNames(pathToCanvas2, lineLevel);
        //        canvas2->setCurvesSelection( pathToCanvas2 );
        //        layers->receivePaths(pathToCanvas2);

    } else {

        ///Close path with one segment
        ///
        ///
        ///

        int npoints = pathToCanvas2.elementCount();

        //NPoints means P0,P1,P2,P3 = 4 points

        QPainterPath closedContour;
        QPainterPath pathThatClosesContour;

        closedContour.addPath(pathToCanvas2);

        pathThatClosesContour.moveTo(pathToCanvas2.elementAt(npoints-1));

        QPointF ControlPointP1, ControlPointP2;

        // S is the new Bezier Curve
        // S0 = P3
        // S1 = P3 + (P3-P2)
        // S2 = Q0 + (Q0 - Q1)
        // S3 = Q0

        ControlPointP1 = pathToCanvas2.elementAt(npoints - 1) + (( pathToCanvas2.elementAt(npoints - 1) - pathToCanvas2.elementAt(npoints - 2) ));
        ControlPointP2 = pathToCanvas2.elementAt(0) + (( pathToCanvas2.elementAt(0) - pathToCanvas2.elementAt(1) ));

        pathThatClosesContour.cubicTo(ControlPointP1,ControlPointP2,pathToCanvas2.elementAt(0));

        closedContour.addPath(pathThatClosesContour);
        sendPathsAndNames(closedContour, lineLevel);

        //        canvas2->setCurvesSelection( closedContour );
        //        layers->receivePaths( closedContour );


    }

}

void CanvasMediator::closeAndSendPaths( const QVector<QPainterPath>& pathsToCanvas2, const int& lineLevel){

    // Fechar os Paths combinando pontos 0-0 / 0-3 / 3-0 / 3-3
    //

    if( canvas2 == nullptr || layers == nullptr ) return;
    if (pathsToCanvas2.size() == 0) return;

    QVector<QPainterPath> pathsSegmentsList = pathsToCanvas2;

    QPainterPath closedPath;



    //NPoints means P0,P1,P2,P3 = 4 points

    for (int i = 0; i < pathsSegmentsList.size()-1 ; i++ ) {

        closedPath.addPath(pathsSegmentsList[i]);

        int npoints = pathsSegmentsList[i].elementCount();

        closedPath.moveTo(pathsSegmentsList[i].elementAt( npoints-1 ));

        QPointF ControlPointP1, ControlPointP2;

        // S is the new Bezier Curve
        // S0 = P3
        // S1 = P3 + (P3-P2)
        // S2 = Q0 + (Q0 - Q1)
        // S3 = Q0

        ControlPointP1 = pathsSegmentsList[i].elementAt( npoints - 1 ) + (( pathsSegmentsList[i].elementAt( npoints -1 ) - pathsSegmentsList[i].elementAt( npoints - 2 ) ));
        ControlPointP2 = pathsSegmentsList[i+1].elementAt( 0 ) + (( pathsSegmentsList[i+1].elementAt( 0 ) - pathsSegmentsList[ i+1 ].elementAt( 1 ) ));

        closedPath.cubicTo(ControlPointP1,ControlPointP2,pathsSegmentsList[i+1].elementAt(0));

    }

    closedPath.addPath(pathsSegmentsList[pathsSegmentsList.size()-1]);

    int npoints = pathsSegmentsList[pathsSegmentsList.size()-1].elementCount();

    closedPath.moveTo(pathsSegmentsList[pathsSegmentsList.size()-1].elementAt( npoints-1 ));

    QPointF ControlPointP1, ControlPointP2;

    // S is the new Bezier Curve
    // S0 = P3
    // S1 = P3 + (P3-P2)
    // S2 = Q0 + (Q0 - Q1)
    // S3 = Q0

    ControlPointP1 = pathsSegmentsList[pathsSegmentsList.size()-1].elementAt( npoints - 1 ) + (( pathsSegmentsList[pathsSegmentsList.size()-1].elementAt( npoints -1 ) - pathsSegmentsList[pathsSegmentsList.size()-1].elementAt( npoints - 2 ) ));
    ControlPointP2 = pathsSegmentsList[0].elementAt( 0 ) + (( pathsSegmentsList[0].elementAt( 0 ) - pathsSegmentsList[ 0 ].elementAt( 1 ) ));

    closedPath.cubicTo(ControlPointP1,ControlPointP2,pathsSegmentsList[0].elementAt(0));

    sendPathsAndNames(closedPath, lineLevel);

    //    canvas2->setCurvesSelection( closedPath );
    //    layers->receivePaths( closedPath );

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

