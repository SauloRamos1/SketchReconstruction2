#include <iostream>

#include <QPainter>
#include <QDebug>
#include <QMatrix4x4>


#include "include/input_sketch.h"


InputSketch::InputSketch()
{
    //setPath (curve);
    //setPath (openContour);
    //setPath (closedContour);
    
    updateColorMap();
    depthLevelList.push_back(0);
}



//************************************************************************************************
/// ........................................ INTERACTIONS ..........................................
//************************************************************************************************


void InputSketch::chooseDefaultInteraction(){
    statusSketch = Interaction::DEFAULT;
}

void InputSketch::chooseCrossSelection(){
    statusSketch = Interaction::CROSS_SELECTION;

}

void InputSketch::chooseCropSelection(){
    statusSketch = Interaction::CROP_SELECTION;

}

void InputSketch::chooseEraseSelection(){
    statusSketch = Interaction::ERASE_SELECTION;

}

void InputSketch::chooseMoveZoom_Interaction(){
    statusSketch = Interaction::MOVE_ZOOM;

}

void InputSketch::chooseOpenContour_Interaction()
{
    statusSketch = Interaction::OPENCONTOUR;

}

void InputSketch::chooseClosedContour_Interaction()
{
    statusSketch = Interaction::CLOSEDCONTOUR;

}

void InputSketch::chooseStripes_Interaction()
{
    statusSketch = Interaction::STRIPES;
}


//************************************************************************************************
/// ................................... OPEN CONTOURS ..........................................
//************************************************************************************************

void InputSketch::createOpenContour (const QPointF& pos){
    
    prepareGeometryChange();
    openContour.moveTo(pos);
    update();

    
}

void InputSketch::addOpenContour (const QPointF& pos){

    prepareGeometryChange();
    openContour.lineTo(pos);
    update();

}

bool InputSketch::saveOpenContour (){

    if (openContour.length() < 2){
        openContour = QPainterPath();
        return false; //Testa se ha linha desenhada

    }
    smoothPath(openContour);
    smoothPath(openContour);
    smoothPath(openContour);

    QPainterPath3D curve3D;
    curve3D.contour = openContour;
    curve3D.level = lineLevel;

    QString name = "Open ";
    name.append(QString::number(nOpenContours));
    nOpenContours++;
    curve3D.name = name;
    names.append(name);

    sameOpenContourList.append( curve3D );
    openContourList.append( sameOpenContourList );
    sameOpenContourList.clear();
    openContour = QPainterPath();


    update();

    return true;

}

void InputSketch::increaseOpenContourLevelWhileDrawing(){

    if (openContour.length() < 2) return; //Testa se ha linha desenhada

    smoothPath(openContour);
    smoothPath(openContour);
    smoothPath(openContour);


    QPainterPath3D curve3D;
    curve3D.contour = openContour;
    curve3D.level = lineLevel;
    QString name = "Open ";
    name.append(QString::number(nOpenContours));
    nOpenContours++;
    curve3D.name = name;
    names.append(name);

    sameOpenContourList.append(curve3D);
    openContour = QPainterPath();

    openContour.moveTo(curve3D.contour.pointAtPercent(1));

    lineLevel +=1;
    update();
}

void InputSketch::decreaseOpenContourLevelWhileDrawing(){


    if (openContour.length() < 2) return; //Testa se ha linha desenhada

    smoothPath(openContour);
    smoothPath(openContour);
    smoothPath(openContour);

    QPainterPath3D curve3D;
    curve3D.contour = openContour;
    curve3D.level = lineLevel;
    QString name = "Open ";
    name.append(QString::number(nOpenContours));
    nOpenContours++;
    curve3D.name = name;
    names.append(name);

    sameOpenContourList.append(curve3D);
    openContour = QPainterPath();
    openContour.moveTo(curve3D.contour.pointAtPercent(1));

    lineLevel -=1;
    update();

}


//************************************************************************************************
/// ................................... CLOSED CONTOURS ..........................................
//************************************************************************************************

void InputSketch::createClosedContour (const QPointF& pos){
    
    prepareGeometryChange();
    closedContour.moveTo(pos);
    update();
    
}

void InputSketch::addClosedContour (const QPointF& pos){


    prepareGeometryChange();
    closedContour.lineTo(pos);

    for (int i = 0 ; i < closedContourList.size() ; i++){

        if (closedContourList[i].contour.intersects(closedContour) && closedContourList[i].level == lineLevel){

            // Se intersecta o abaixo esta acima e deve calcular depth automatico, indica que o proximo esta intersectando

            //-----
            linesToPolygon.clear();

            curvePolygon = closedContour.toFillPolygon();

            pathPolygon = pathPolygon.united(closedContourList[i].contour.toFillPolygon().intersected(curvePolygon));

            if (closedContourList[i].contour.contains(closedContour.pointAtPercent(1))){
                if (closedContourIntersects == -1){
                    closedContourIntersects = i;
                }

                QPainterPath poly;

                currentPolySugestion = closedContourList[i].contour.toFillPolygon();

                for (int j = 0; j < currentPolySugestion.size()-1; ++j) {

                    poly = QPainterPath();
                    poly.moveTo(closedContour.pointAtPercent(1));
                    poly.lineTo(currentPolySugestion[j]);
                    poly.lineTo(currentPolySugestion[j+1]);
                    linesToPolygon.append(poly);
                }
            }
        }
    }

    curvePolygon = QPolygonF();
    pathPolygon = QPolygonF();
    currentPolySugestion = QPolygonF();

    update();
}

bool InputSketch::saveClosedContour (){

    if (closedContour.length() < 2){
        closedContour = QPainterPath();
        return false; //Testa se ha linha desenhada
    }
    smoothPath(closedContour);
    smoothPath(closedContour);
    smoothPath(closedContour);

    QPainterPath3D curve3D;
    curve3D.contour = closePath(closedContour);
    smoothPath(curve3D.contour);
    curve3D.level = lineLevel;

    QString name = "Closed ";
    name.append(QString::number(nClosedContours));
    nClosedContours++;
    curve3D.name = name;
    names.append(name);

    curve3D.attachClosedContour = closedContourIntersects;
    closedContourIntersects = -1;

    receiveSelectedPath(curve3D.contour, curve3D.name, curve3D.level);

    closedContourList.append(curve3D);
    closedContour = QPainterPath();
    return true;
}

//USE IT FOR SVG and SKETCHING
QPainterPath InputSketch::closePath (QPainterPath pathToBeClosed){

    if ( almostEqual( pathToBeClosed.pointAtPercent(0),pathToBeClosed.pointAtPercent(1) ) ){

        return pathToBeClosed;

    } else {

        ///Close path with one segment
        ///
        ///
        ///

        int npoints = pathToBeClosed.elementCount();

        //NPoints means P0,P1,P2,P3 = 4 points

        QPainterPath finalClosedContour;
        QPainterPath pathThatClosesContour;

        finalClosedContour.addPath(pathToBeClosed);

        pathThatClosesContour.moveTo(pathToBeClosed.elementAt(npoints-1));

        QPointF ControlPointP1, ControlPointP2;

        // S is the new Bezier Curve
        // S0 = P3
        // S1 = P3 + (P3-P2)
        // S2 = Q0 + (Q0 - Q1)
        // S3 = Q0

        ControlPointP1 = pathToBeClosed.elementAt(npoints - 1) + (( pathToBeClosed.elementAt(npoints - 1) - pathToBeClosed.elementAt(npoints - 2) ));
        ControlPointP2 = pathToBeClosed.elementAt(0) + (( pathToBeClosed.elementAt(0) - pathToBeClosed.elementAt(1) ));

        pathThatClosesContour.cubicTo(ControlPointP1,ControlPointP2,pathToBeClosed.elementAt(0));

        finalClosedContour.addPath(pathThatClosesContour);

        return finalClosedContour;

    }

}


QPainterPath InputSketch::closeSVGSegments(const QVector<QPainterPath>& pathsToClose){

    // Fechar os Paths combinando pontos 0-0 / 0-3 / 3-0 / 3-3
    //

    QVector<QPainterPath> pathsSegmentsList = pathsToClose;

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

    return closedPath;

    //    canvas2->setCurvesSelection( closedPath );
    //    layers->receivePaths( closedPath );

}



/// ................................... InputSketch 2 ..........................................

void InputSketch::receiveSelectedPath (const QPainterPath &path, const QString &name, const int &lineLevel) {

    QVector<QPointF> sampledPointsOnCurve;
    QVector<qreal> angleAtSampledPoint;

    allSampledPoints t = allSampledPoints();
    float contourSamplingStep = 5.0f;
    //float contourSamplingStep = 0.5f;


    for (qreal j = 0; j < path.length(); j = j + contourSamplingStep) {

        sampledPointsOnCurve.push_back(path.pointAtPercent(path.percentAtLength(j)));
        angleAtSampledPoint.push_back(path.angleAtPercent(path.percentAtLength(j)));

    }


    for (int j = 0; j < sampledPointsOnCurve.size()/2 ; j++) {

        QVector3D temp_ql(sampledPointsOnCurve[j].x(),sampledPointsOnCurve[j].y(), (lineLevel));
        qreal temp_angleql(angleAtSampledPoint[j]);

        QVector3D temp_qr(sampledPointsOnCurve[sampledPointsOnCurve.size()-1-j].x(),sampledPointsOnCurve[sampledPointsOnCurve.size()-1-j].y(), (lineLevel));
        qreal temp_angleqr(angleAtSampledPoint[angleAtSampledPoint.size()-1-j]);

        t.ql.push_back(temp_ql);
        t.anglesql.push_back(temp_angleql);
        t.qr.push_back(temp_qr);
        t.anglesqr.push_back(temp_angleqr);

    }

    for (int j = 0; j < t.ql.size(); ++j) {

        if (j == 0){
            lastmidPointsPath.moveTo(((t.ql[j] + t.qr[j]) / 2).x(), ((t.ql[j] + t.qr[j]) / 2).y());
        } else {
            lastmidPointsPath.lineTo(((t.ql[j] + t.qr[j]) / 2).x(), ((t.ql[j] + t.qr[j]) / 2).y());
        }
    }

    //findSymmetricalMedialAxis(pathList[i], t.ql, t.qr);

    QPainterPath midPointsPath;
    for (int j = 0; j < t.ql.size(); ++j) {

        t.midPoints.push_back((t.ql[j] + t.qr[j]) / 2);

        if (j == 0){
            midPointsPath.moveTo(t.midPoints[0].x(), t.midPoints[0].y());
        } else {
            midPointsPath.lineTo(t.midPoints[j].x(), t.midPoints[j].y());
        }
    }

    t.midPointsPath = midPointsPath;
    t.name = name;
    t.contour = QPainterPath();
    t.contour = path;

    allShapesSampledPoints.push_back(t);


    //RotationalBlendingSurface(i, paths[i], t.ql, t.qr);
    //update();

}

void InputSketch::estimateShapes(){

    //    int lineLeveldiff = layerDifference;

    //    lineLeveldiff = QInputDialog::getInt(nullptr, "Depth Difference", "Layering difference in depth");
    //    lineLeveldiff*=-1;

    pointsFor3Ddisks.clear();
    normalsFor3Ddisks.clear();

    for (int i = 0; i < allShapesSampledPoints.size(); ++i) {

        int reconstType = 0;
        if (closedContourList[i].attachClosedContour == 0){

        }

        while (reconstType < 1 || reconstType > 2){
            reconstType = QInputDialog::getInt(nullptr, "Select Reconstruction Type for " + allShapesSampledPoints[i].name, "Select Reconstruction Type for: " + allShapesSampledPoints[i].name + " \n 1 - Rotational Blending Surface \n 2 - Hermitian Radial Basis Function");
        }
        if (reconstType == 1) {
            RotationalBlendingSurface(i, allShapesSampledPoints[i].contour, allShapesSampledPoints[i].ql,allShapesSampledPoints[i].qr);

        } else {
            DataForHRBF (i, allShapesSampledPoints[i].contour, allShapesSampledPoints[i].ql,allShapesSampledPoints[i].qr);
        }
    }

    // update();

}



void InputSketch::RotationalBlendingSurface(const int shapeNumber, QPainterPath &contour, QVector<QVector3D>& ql, QVector<QVector3D>& qr){

    if (closedContourList[shapeNumber].attachClosedContour != -1){
        int attachedContour = closedContourList[shapeNumber].attachClosedContour;
        qDebug () << "Contour: " <<  shapeNumber <<  "Attached Contour: " << attachedContour;
        for (int i = 0; i < ql.size(); ++i) {
            ql[i].setZ(depthLevelList[closedContourList[attachedContour].level-1] + closedContourList[attachedContour].maior_z*0.8);
            qr[i].setZ(depthLevelList[closedContourList[attachedContour].level-1] + closedContourList[attachedContour].maior_z*0.8);
//            ql[i].setZ(closedContourList[attachedContour].maior_z*0.8);
//            qr[i].setZ(closedContourList[attachedContour].maior_z*0.8);
        }
    } else {
        for (int i = 0; i < ql.size(); ++i) {
            ql[i].setZ(depthLevelList[closedContourList[shapeNumber].level-1]);
            qr[i].setZ(depthLevelList[closedContourList[shapeNumber].level-1]);
        }
    }



    QVector3D n;
    QVector3D pl, pr;
    int u = 0, u_step = 1;

    allShapesSampledPoints[shapeNumber].shapePoints.clear();

    QVector3D bottomLeft (contour.boundingRect().bottomLeft().x(), contour.boundingRect().bottomLeft().y() , ql[0].z());

    QVector3D topLeft (contour.boundingRect().topLeft().x(), contour.boundingRect().topLeft().y() , ql[0].z());
    QVector3D bottomRight (contour.boundingRect().bottomRight().x(), contour.boundingRect().bottomRight().y() , ql[0].z());

    n = QVector3D::crossProduct(topLeft - bottomLeft , bottomRight - bottomLeft);

    int nvertex = 0;

    QVector3D center = (ql[ql.size()/2] + qr[qr.size()/2]) / 2;

    while (u < ql.size()) {

        int v = 0, v_step = 5;

        QVector<Vertex> vPoints;

        while (v < 360) {

            QVector3D s0 = (ql[u] + qr[u]) / 2;

            QVector3D s1 = (qr[u] - ql[u]);
            float norm = s1.length();

            s1 = norm * n;

            QMatrix4x4 m1;
            m1.setToIdentity();
            m1.translate(s0);
            m1.rotate(v, QVector3D::crossProduct( s1 - s0, ql[u] - s0));
            m1.translate(-s0);

            pl = m1 * ql[u];

            QMatrix4x4 m2;

            m2.setToIdentity();
            m2.translate(s0);
            m2.rotate(-v, QVector3D::crossProduct( s1 - s0, qr[u] - s0));
            m2.translate(-s0);

            pr = m2 * qr[u];

            float t = v/360;

            QVector3D p = ( 1 - t ) * pl + t * pr;

            if (p.z() < closedContourList[shapeNumber].maior_z){
                closedContourList[shapeNumber].maior_z = p.z();
//                qDebug () << p.z();
            }


            pointsFor3Ddisks.push_back(p);
            normalsFor3Ddisks.push_back((p - center).normalized());

            Vertex vertex;
            vertex.point3D = p;
            vertex.vertexNumber = nvertex;
            vPoints.push_back(vertex);


            v += v_step;
            nvertex+=1;

        }

        allShapesSampledPoints[shapeNumber].shapePoints.push_back(vPoints);


        u += u_step;

    }
//    if (closedContourList[shapeNumber].attachClosedContour != -1){
//        int attachedContour = closedContourList[shapeNumber].attachClosedContour;
////        layerDifference = closedContourList[attachedContour].maior_z*0.9;
////        qDebug () << layerDifference;
//        for (int i = 0; i < ql.size(); ++i) {
//            ql[i].setZ(ql[i].z() - closedContourList[attachedContour].maior_z);
//            qr[i].setZ(qr[i].z() - closedContourList[attachedContour].maior_z);
//            ql[i].setZ(ql[i].z()/layerDifference);
//            qr[i].setZ(qr[i].z()/layerDifference);
//        }
//    } else {
//        for (int i = 0; i < ql.size(); ++i) {
//            ql[i].setZ(ql[i].z()/layerDifference);
//            qr[i].setZ(qr[i].z()/layerDifference);
//        }
//    }




}


void InputSketch::DataForHRBF(const int shapeNumber, QPainterPath &contour, QVector<QVector3D>& ql, QVector<QVector3D>& qr){

    QVector <QVector3D> knownPoints;
    QVector <QVector3D> knownNormals;
    QVector <QVector3D> totalPoints;
    QVector <QVector3D> totalNormals;

    QList<QPolygonF> pathPolygons = contour.toSubpathPolygons();

    QPolygonF pathPolygon;

    foreach (QPolygonF poly, pathPolygons){

        pathPolygon.append(poly);
    }

    QLineF angleLine;
    QPointF point2D;
    QVector3D point3D;

    QVector3D normalVector;
    normalVector.setZ(0);


    ///TODO FIND POINTS ON PATH TO RETRIVE ANGLE


    // Create normals for contour / stroke

    double contoursamplingvalue = 5.0; // ADJUSTABLE in Pixels
    double percent;

    double pathLength = contour.length();

    float angulopranormal;
    if (getPathArea(contour, 10) < 0 ){
        angulopranormal = 90;
    } else {
        angulopranormal = -90;
    }

    for (double var = pathLength; var > 0 ; var = var - contoursamplingvalue){

        percent = var / pathLength;
        /* Set the origin: */

        point2D = static_cast<QPointF>( contour.pointAtPercent(percent));
        angleLine.setP1(point2D);
        point3D.setX(static_cast<float>(point2D.x()));
        point3D.setY(static_cast<float>(point2D.y()));
        point3D.setZ(0);
        //pointsView.push_back(point3D);

        knownPoints.push_back(point3D);
        totalPoints.push_back(point3D);

        /* Set the angle and length: */

        angleLine.setAngle (contour.angleAtPercent(percent)+angulopranormal);
        angleLine.setLength(1);
        normalVector.setX(static_cast<float>(angleLine.x2())-static_cast<float>(angleLine.x1()));
        normalVector.setY(static_cast<float>(angleLine.y2())-static_cast<float>(angleLine.y1()));

        angleLine.setLength(10);
        normalsContour.append(angleLine);

        knownNormals.push_back(normalVector); //PROBLEM ?? normalize
        totalNormals.push_back(normalVector.normalized());

    }

    //Estimate Normals

    QRectF pathBox = contour.boundingRect();
    double xmin = pathBox.left();
    double xmax = pathBox.right();
    double ymin = pathBox.top();
    double ymax = pathBox.bottom();

    double spacing = 5.0; // ADJUSTABLE in Pixels

    //Calculate Grid
    for(double x = xmin; x < xmax; x = x + spacing){
        for(double y = ymin; y < ymax; y = y + spacing){

            QPointF gridPoint (x,y);

            if (pathPolygon.containsPoint(gridPoint, Qt::WindingFill)){

                QVector3D p(static_cast<float>(x),static_cast<float>(y),0);
                //Calculate Weight for point based on distance

                double weight = 0;

                for (QVector3D pi:knownPoints){

                    weight += 1.0/static_cast<double>(((p-pi).lengthSquared()));
                }

                //Calculate Normal
                QVector3D n(0,0,0);

                for(int i = 0; i <knownNormals.size(); i++){

                    QVector3D pi = knownPoints.at(i);
                    QVector3D ui = knownNormals.at(i);
                    n += (ui / (p-pi).lengthSquared());
                }

                n = n / static_cast<float>(weight);

                n.setZ(sqrt(1 - n.x()*n.x() - n.y()*n.y()));



                if (n.z() < 0.5f){
                    p.setZ(n.z()*(static_cast<float>(pathBox.width())/5)); //Adjustable
                    //p.setZ(p.z() + lineLeveldiff * ql[0].z());

                    totalPoints.push_back(p);
                    totalNormals.push_back(n);

                    //pointsView.push_back(p);
                    p.setZ(p.z() * -1);//Adjustable
                    //p.setZ(p.z() + lineLeveldiff * ql[0].z());

                    totalPoints.push_back(p);

                    //pointsView.push_back(p);
                    n.setZ(n.z()*-1);
                    totalNormals.push_back(n);
                }
            }

        }
    }

    std::ofstream fOut;

    std::string outFile = "exportData";
    outFile.append(std::to_string(shapeNumber));
    outFile.append(".data");
    fOut.open(outFile.c_str());
    fOut << "3" <<std::endl;
    fOut << knownPoints.size() <<std::endl;
    for(QVector3D p:knownPoints)
    {
        p.setZ(p.z() + (layerDifference * ql[0].z()));
        //       p.setZ(p.z() + (ql[0].z()));
        fOut << p.x() <<" " << p.y()<< " " << p.z()<< std::endl;
        pointsFor3Ddisks.append(p);
    }

    for(QVector3D n:knownNormals){
        normalsFor3Ddisks.append(n);
    }

    fOut << totalNormals.size() << std::endl;

    for(int k = 0; k < totalNormals.size(); k++)
    {

        QVector3D p = totalPoints.at(k);
        QVector3D n = totalNormals.at(k);

        fOut << n.x() <<" " << n.y()<< " " << n.z()<< std::endl;
        p.setZ(p.z() + (layerDifference * ql[0].z()));
        //p.setZ(p.z() + (ql[0].z()));
        fOut << p.x() <<" " << p.y()<< " " << p.z()<< std::endl;
        pointsFor3Ddisks.append(p);
        normalsFor3Ddisks.append(n);

        totalPoints[k] = p;
    }
    fOut << "0" << std::endl;
    fOut.close();


    std::ofstream fOut2;

    std::string outFile2 = "exportDataVIPSS";
    outFile2.append(std::to_string(shapeNumber));
    outFile2.append(".xyz");
    fOut2.open(outFile2.c_str());

    foreach (QVector3D p, totalPoints) {

        fOut2 << p.x() <<" " << p.y()<< " " << p.z()<< std::endl;
    }

    fOut2.close();

    totalPoints.clear();
    totalNormals.clear();

}


//************************************************************************************************
/// ....................................... STRIPES ..........................................
//************************************************************************************************

void InputSketch::createStripeContour (const QPointF& pos){
    
    prepareGeometryChange();
    stripeContour.moveTo(pos);
    update();
}

void InputSketch::addStripeContour (const QPointF& pos){

    prepareGeometryChange();
    stripeContour.lineTo(pos);
    update();
}


void InputSketch::finishBand (){//Finish


    if (sameStripeContourList.isEmpty()){
        return;
    }

    //stripeContourList.append(sameStripeContourList);


    QPainterPath joinedLeftLine, joinedRightLine;

    //QVector<int> newLevelList;

    joinedLeftLine = sameStripeContourList.begin()->leftLine;
    joinedRightLine = sameStripeContourList.begin()->rightLine;

    //newLevelList.append(levelList[0]);

    int subStripes = sameStripeContourList.size();

    if (sameStripeContourList.size() > 1){

        for (int i = 0; i < sameStripeContourList.size()-1; ++i) {

            if (sameStripeContourList[i].level == sameStripeContourList[i+1].level) {

                sameStripeContourList[i].contour.connectPath(sameStripeContourList[i+1].contour);

                sameStripeContourList[i].leftLine.connectPath(sameStripeContourList[i+1].leftLine);
                sameStripeContourList[i].rightLine.connectPath(sameStripeContourList[i+1].rightLine);

                sameStripeContourList.removeAt(i+1);
                i--;

            } else {

                sameStripeContourList[i].name = "";

            }
        }
    }
    //    secondbandLineListLeft.clear();


    //    secondbandLineListRight.clear();

    //    if (finalSecondBandLineListLeft.isEmpty() || finalSecondBandLineListRight.isEmpty())
    //        return;

    prepareGeometryChange();



    for (int i = 0; i < sameStripeContourList.size(); i++){
        for (int j = 0; j < 10; j++){
            smoothPath(sameStripeContourList[i].leftLine);
            smoothPath(sameStripeContourList[i].rightLine);
        }
    }

    stripeContourList.append(sameStripeContourList);
    sameStripeContourList.clear();

    nStripeContours -= subStripes;

    QString name = "Stripe ";
    name.append(QString::number(nStripeContours));
    nStripeContours++;
    stripeContourList.last().last().name = name;
    names.append(name);

   // qDebug () << stripeContourList.size();
}


bool InputSketch::saveStripeContour (){

    if (stripeContour.length() < 2){//Testa se ha linha desenhada
        stripeContour = QPainterPath();
        return false;
    }

    smoothPath(stripeContour);
    smoothPath(stripeContour);
    smoothPath(stripeContour);

    Stripe3D stripe;
    stripe.contour = QPainterPath ();
    stripe.contour = stripeContour;

    stripe.leftLine = QPainterPath();
    stripe.rightLine = QPainterPath();
    stripe.level = lineLevel;

    QString name = "Stripe ";
    name.append(QString::number(nStripeContours));
    nStripeContours++;
    stripe.name = name;
    names.append(name);

    QPainterPath leftBandLine, rightBandLine;

    QLineF stripeLeftLine, stripeRightLine;

    stripeRightLine.setP1(stripeContour.pointAtPercent((stripeContour.percentAtLength(0))));
    stripeLeftLine.setP1(stripeContour.pointAtPercent((stripeContour.percentAtLength(0))));
    stripeRightLine.setLength(twistingThickness);
    stripeLeftLine.setLength(twistingThickness);

    stripeRightLine.setAngle(stripeContour.angleAtPercent(stripeContour.percentAtLength(0))-90);
    stripeLeftLine.setAngle(stripeContour.angleAtPercent(stripeContour.percentAtLength(0))+90);

    leftBandLine.moveTo(stripeLeftLine.p2());
    rightBandLine.moveTo(stripeRightLine.p2());

    for (double i = 0; i < stripeContour.length(); i = i+0.5) {

        stripeRightLine.setP1(stripeContour.pointAtPercent((stripeContour.percentAtLength(i))));
        stripeRightLine.setLength(twistingThickness);

        stripeLeftLine.setP1(stripeContour.pointAtPercent((stripeContour.percentAtLength(i))));
        stripeLeftLine.setLength(twistingThickness);

        stripeRightLine.setAngle(stripeContour.angleAtPercent(stripeContour.percentAtLength(i))+90);
        stripeLeftLine.setAngle(stripeContour.angleAtPercent(stripeContour.percentAtLength(i))-90);

        leftBandLine.lineTo(stripeRightLine.p2());
        rightBandLine.lineTo(stripeLeftLine.p2());

    }

    if (front) {

        stripe.leftLine = leftBandLine;
        stripe.rightLine = rightBandLine;
        sameStripeContourList.append(stripe);

    } else {

        stripe.leftLine = rightBandLine;
        stripe.rightLine = leftBandLine;
        sameStripeContourList.append(stripe);
    }

    stripeContour = QPainterPath ();

    //Invert Band
    front = !front ;
    return true;

}


void InputSketch::increaseStripeContourLevelWhileDrawing(){

    if (stripeContour.length() < 2) return; //Testa se ha linha desenhada

    smoothPath(stripeContour);
    smoothPath(stripeContour);
    smoothPath(stripeContour);

    Stripe3D stripe;
    //stripe.contour = QPainterPath ();
    stripe.contour = stripeContour;

    //    stripe.leftLine = QPainterPath();
    //    stripe.rightLine = QPainterPath();

    QString name = "Stripe ";
    name.append(QString::number(nStripeContours));
    nStripeContours++;
    stripe.name = name;
    names.append(name);

    QPainterPath leftBandLine, rightBandLine;

    QLineF stripeLeftLine, stripeRightLine;

    stripeRightLine.setP1(stripeContour.pointAtPercent((stripeContour.percentAtLength(0))));
    stripeLeftLine.setP1(stripeContour.pointAtPercent((stripeContour.percentAtLength(0))));
    stripeRightLine.setLength(twistingThickness);
    stripeLeftLine.setLength(twistingThickness);

    stripeRightLine.setAngle(stripeContour.angleAtPercent(stripeContour.percentAtLength(0))-90);
    stripeLeftLine.setAngle(stripeContour.angleAtPercent(stripeContour.percentAtLength(0))+90);

    leftBandLine.moveTo(stripeLeftLine.p2());
    rightBandLine.moveTo(stripeRightLine.p2());

    for (double i = 0; i < stripeContour.length(); i = i+0.5) {

        stripeRightLine.setP1(stripeContour.pointAtPercent((stripeContour.percentAtLength(i))));
        stripeRightLine.setLength(twistingThickness);

        stripeLeftLine.setP1(stripeContour.pointAtPercent((stripeContour.percentAtLength(i))));
        stripeLeftLine.setLength(twistingThickness);

        stripeRightLine.setAngle(stripeContour.angleAtPercent(stripeContour.percentAtLength(i))+90);
        stripeLeftLine.setAngle(stripeContour.angleAtPercent(stripeContour.percentAtLength(i))-90);

        leftBandLine.lineTo(stripeRightLine.p2());
        rightBandLine.lineTo(stripeLeftLine.p2());

    }

    stripe.level = lineLevel;


    if (front) {

        stripe.leftLine = leftBandLine;
        stripe.rightLine = rightBandLine;
        sameStripeContourList.append(stripe);

    }
    else {

        stripe.leftLine = rightBandLine;
        stripe.rightLine = leftBandLine;
        sameStripeContourList.append(stripe);
    }

    stripeContour = QPainterPath ();
    stripeContour.moveTo(stripe.contour.pointAtPercent(1));

    lineLevel +=1;
    update();
}


void InputSketch::decreaseStripeContourLevelWhileDrawing(){

    if (stripeContour.length() < 2) return; //Testa se ha linha desenhada

    smoothPath(stripeContour);
    smoothPath(stripeContour);
    smoothPath(stripeContour);

    Stripe3D stripe;
    //stripe.contour = QPainterPath ();
    stripe.contour = stripeContour;

    //    stripe.leftLine = QPainterPath();
    //    stripe.rightLine = QPainterPath();

    QPainterPath leftBandLine, rightBandLine;

    QLineF stripeLeftLine, stripeRightLine;

    QString name = "Stripe ";
    name.append(QString::number(nStripeContours));
    nStripeContours++;
    stripe.name = name;
    names.append(name);

    stripeRightLine.setP1(stripeContour.pointAtPercent((stripeContour.percentAtLength(0))));
    stripeLeftLine.setP1(stripeContour.pointAtPercent((stripeContour.percentAtLength(0))));
    stripeRightLine.setLength(twistingThickness);
    stripeLeftLine.setLength(twistingThickness);

    stripeRightLine.setAngle(stripeContour.angleAtPercent(stripeContour.percentAtLength(0))-90);
    stripeLeftLine.setAngle(stripeContour.angleAtPercent(stripeContour.percentAtLength(0))+90);

    leftBandLine.moveTo(stripeLeftLine.p2());
    rightBandLine.moveTo(stripeRightLine.p2());

    for (double i = 0; i < stripeContour.length(); i = i + 0.5) {

        stripeRightLine.setP1(stripeContour.pointAtPercent((stripeContour.percentAtLength(i))));
        stripeRightLine.setLength(twistingThickness);

        stripeLeftLine.setP1(stripeContour.pointAtPercent((stripeContour.percentAtLength(i))));
        stripeLeftLine.setLength(twistingThickness);

        stripeRightLine.setAngle(stripeContour.angleAtPercent(stripeContour.percentAtLength(i))+90);
        stripeLeftLine.setAngle(stripeContour.angleAtPercent(stripeContour.percentAtLength(i))-90);

        leftBandLine.lineTo(stripeRightLine.p2());
        rightBandLine.lineTo(stripeLeftLine.p2());

    }

    stripe.level = lineLevel;


    if (front) {

        stripe.leftLine = leftBandLine;
        stripe.rightLine = rightBandLine;
        sameStripeContourList.append(stripe);

    }
    else {

        stripe.leftLine = rightBandLine;
        stripe.rightLine = leftBandLine;
        sameStripeContourList.append(stripe);
    }

    stripeContour = QPainterPath ();
    stripeContour.moveTo(stripe.contour.pointAtPercent(1));

    lineLevel -=1;
    update();
}



//************************************************************************************************
/// ....................................... SELECTION ..........................................
//************************************************************************************************

void InputSketch::createSelectionCurve(const QPointF &pos){
    prepareGeometryChange();
    curve = QPainterPath();
    curve.moveTo(pos);
    update();
}

void InputSketch::addSelectionCurve(const QPointF &pos){

    prepareGeometryChange();
    curve.lineTo(pos);
    update();
}


//************************************************************************************************
/// ....................................... LAYERING ..........................................
//************************************************************************************************


void InputSketch::increaseLevel(){
    lineLevel +=1;
}

void InputSketch::decreaseLevel(){
    lineLevel-=1;
}



void InputSketch::updateColorMap(){

    //  Update number of Layers

    if (lineLevel > numberOfLayers){
        numberOfLayers = lineLevel;
        depthLevelList.push_back(depthLevelList.last() + layerDifference);
    }

    lineColorMap.clear();

    int     startRedVal = 0,
            endRedValue = 255,
            startGreenVal = 0,
            endGreenValue = 255,
            startBlueVal = 0,
            endBlueValue = 255;

    float ratio;

    for (int i = 0; i < numberOfLayers ; i++){

        if (numberOfLayers == 1){
            ratio = 0;
        } else {
            ratio = (  static_cast<float>(i) /static_cast<float>((numberOfLayers - 1)));
        }

        ratio = 1 - ratio;

        // qDebug () << "ratio: " << ratio;

        //ratio should go from 0 to 1?

        float red = (ratio*startRedVal + (1-ratio)*endRedValue);
        float green = (ratio*startGreenVal + (1-ratio)*endGreenValue);
        float blue = (ratio*startBlueVal + (1-ratio)*endBlueValue);
        QVector3D color(red,green,blue);
        lineColorMap.append(color);
    }

    shapeColorMap.clear();

    startRedVal = 0;
    endRedValue = 255;
    startGreenVal = 0;
    endGreenValue = 255;
    startBlueVal = 255;
    endBlueValue = 0;

    for (int i = 0; i < numberOfLayers ; i++){

        if (numberOfLayers == 1){
            ratio = 0;
        } else {
            ratio = (  static_cast<float>(i) /static_cast<float>((numberOfLayers - 1)));
        }

        ratio = 1 - ratio;

        // qDebug () << "ratio: " << ratio;

        //ratio should go from 0 to 1?

        float red = (ratio*startRedVal + (1-ratio)*endRedValue);
        float green = (ratio*startGreenVal + (1-ratio)*endGreenValue);
        float blue = (ratio*startBlueVal + (1-ratio)*endBlueValue);
        QVector3D color(red,green,blue);
        shapeColorMap.append(color);
    }

    hatchingMap.clear();

    QVector<QBrush> hatching;
    hatching.append(Qt::VerPattern);
    hatching.append(Qt::HorPattern);
    hatching.append(Qt::BDiagPattern);
    hatching.append(Qt::FDiagPattern);

    for (int i = 0; i < numberOfLayers ; i++){
        hatchingMap.append(hatching[0]);
        hatching.move(0,hatching.size()-1);
    }



}


//************************************************************************************************
/// ......................................... SVG ..........................................
//************************************************************************************************

void InputSketch::drawPaths(const QVector<QPainterPath>& readsvgPaths){

    svgPaths = readsvgPaths;

}


void InputSketch::crossSelection(){

    //selectedPathOnCrossSelection = QPainterPath();

    ///Interaction -- Cross the Path
    ///
    ///
    if (svgPaths.size() > 0){

        for (int i = 0 ; i < svgPaths.size(); i++ ) {

            if (svgPaths.at(i).intersects(curve)) {

                QPainterPath3D closedContourFromSVG;

                closedContourFromSVG.contour = svgPaths[i];
                svgPaths[i]  = QPainterPath();

                closedContourFromSVG.level = lineLevel;

                QString name = "Closed ";
                name.append(QString::number(nClosedContours));
                nClosedContours++;
                closedContourFromSVG.name = name;
                names.append(name);

                //                if(levelList.size() > 0){
                //                    selectedLineLevel = levelList[i];
                //                    //levelList.removeAt(i);
                //                }

                receiveSelectedPath(closedContourFromSVG.contour, closedContourFromSVG.name, closedContourFromSVG.level);

                closedContourList.append(closedContourFromSVG);
                break;
            }
        }
    }

    curve = QPainterPath();
}

void InputSketch::cropSelection(){

    selectedPathsOnCropSelection.clear();
    /// Interaction -- Brush Selection
    /// For each curve point create a circle/radius and if intersects to a segment, this segment compose the new path
    ///
    /// Organize direction of the curve while selecting (on interaction)
    ///

    if (svgPaths.size() < 1) return;


    //double sampleRate = curve.length() / 5;

    for (double i = 0 ; i < curve.length(); i = i + 5) {

        QPointF pos = curve.pointAtPercent(curve.percentAtLength(i));

        QRectF brushRectSelect(pos.x()-3,pos.y()-3,6,6);

        rectList.append(brushRectSelect);

        for (int j = 0 ; j < svgPaths.size(); j++ ) {
            if ( brushRectSelect.contains(svgPaths[j].pointAtPercent(0))) {
                selectedPathsOnCropSelection.append(svgPaths[j]);
                svgPaths[j] = QPainterPath();
            } else if (brushRectSelect.contains(svgPaths[j].pointAtPercent(1))) {
                selectedPathsOnCropSelection.append(svgPaths[j].toReversed());
                svgPaths[j] = QPainterPath();
            }
        }
    }

    QPainterPath3D closedContourFromSVG;

    closedContourFromSVG.contour = closeSVGSegments(selectedPathsOnCropSelection);
    closedContourFromSVG.level = lineLevel;

    QString name = "Closed ";
    name.append(QString::number(nClosedContours));
    nClosedContours++;
    closedContourFromSVG.name = name;
    names.append(name);


    smoothPath(closedContourFromSVG.contour);

    receiveSelectedPath(closedContourFromSVG.contour, closedContourFromSVG.name, closedContourFromSVG.level);
    closedContourList.append(closedContourFromSVG);


    curve = QPainterPath();
}

int InputSketch::eraseSelection(){

    /// Interaction -- Erase Paths
    ///
    ///

    for (int i = 0 ; i < svgPaths.size(); i++ ) {

        if (svgPaths[i].intersects(curve)) {
            svgPaths[i] = QPainterPath();
            curve = QPainterPath();
            return -1 ;

        }
    }

    int count = 0;

    for (int i = 0 ; i < openContourList.size(); i++ ) {
        for (int j = 0 ; j < openContourList[i].size(); j++ ) {

            if (openContourList[i][j].contour.intersects(curve)) {
                openContourList[i][j].contour = QPainterPath();
                openContourList[i].removeAt(j);
                selectedOpenContour = count;
                lastSelected = 0;
                curve = QPainterPath();
                return 0;
            }
            count++;
        }
    }

    count = 0;

    for (int i = 0 ; i < closedContourList.size(); i++ ) {

        if (closedContourList[i].contour.intersects(curve)) {
            closedContourList[i].contour = QPainterPath();
            closedContourList.removeAt(i);
            selectedClosedContour = count;
            lastSelected = 1;
            curve = QPainterPath();
            return 1;
        }
        count++;
    }
    count = 0;

    for (int i = 0 ; i < stripeContourList.size(); i++ ) {
        for (int j = 0 ; j < stripeContourList[i].size(); j++ ) {

            if (stripeContourList[i][j].contour.intersects(curve)) {
                stripeContourList[i][j].contour = QPainterPath();
                stripeContourList[i].removeAt(j);
                selectedStripeContour = count;
                lastSelected = 2;
                curve = QPainterPath();
                return 2;
            }
            count++;
        }
    }


    curve = QPainterPath();
    return -1 ;

    //TODO atualizar Layers Dock

}


//************************************************************************************************
/// ..................................... CURVE EDITION ..........................................
//************************************************************************************************


QPolygonF InputSketch::getCurve() const
{
    if( curve.isEmpty() ) return QPolygonF();

    QPolygonF current_sketch  = curve.toSubpathPolygons().at( 0 );
    return current_sketch;
}

void InputSketch::joinPaths()
{
    //TODO SUBSTITUIR POR LINHA SELECIONADA E OVERSKETCHING CURVE

    switch (lastSelected) {

    case 0 : {
        int count = 0;

        for (int i = 0; i < openContourList.size(); ++i) {
            for (int j = 0; j < openContourList[i].size(); ++j) {

                if (selectedOpenContour == count){

                    //  ------- OVERSKETCH -------
                    prepareGeometryChange();
                    QPolygonF current_sketch;
                    for (int k = 0; k < openContourList[i][j].contour.toSubpathPolygons().size(); ++k) {
                        current_sketch << openContourList[i][j].contour.toSubpathPolygons().at(k);
                    }



                    QPolygonF polygon_list;

                    for (int k = 0; k < oversketchingCurve.toSubpathPolygons().size(); ++k) {
                        polygon_list << oversketchingCurve.toSubpathPolygons().at(k);
                    }
                    oversketchingCurve = QPainterPath();

                    auto polygon_curve = SketchLibrary::smooth( polygon_list );
                    oversketchingCurve.addPolygon(polygon_curve);
                    QPolygonF new_sketch;
                    for (int i = 0; i < oversketchingCurve.toSubpathPolygons().size(); ++i) {
                        new_sketch << oversketchingCurve.toSubpathPolygons().at(i);
                    }

                    smoothPath(oversketchingCurve);
                    smoothPath(oversketchingCurve);
                    smoothPath(oversketchingCurve);

                    bool changed = SketchLibrary::overSketch( current_sketch, new_sketch );
                    //Means that an oversketch happened
                    if ( changed )
                    {

                        openContourList[i][j].contour = QPainterPath();
                        openContourList[i][j].contour.addPolygon(current_sketch);
                        smoothPath(openContourList[i][j].contour);
                        oversketchingCurve = QPainterPath();

                    } else {

                        qDebug () << "Sketch didn't change!";
                    }

                }

                count++;
            }
        }
    } break;


    case 1 :{

        for (int i = 0 ; i < closedContourList.size(); i++) {

            if (selectedClosedContour == i){

                //  ------- OVERSKETCH -------
                prepareGeometryChange();
                QPolygonF current_sketch;
                for (int j = 0; j < closedContourList[i].contour.toSubpathPolygons().size(); j++) {
                    current_sketch << closedContourList[i].contour.toSubpathPolygons().at(j);
                }

                QPolygonF polygon_list;

                for (int j = 0; j < oversketchingCurve.toSubpathPolygons().size(); j++) {
                    polygon_list << oversketchingCurve.toSubpathPolygons().at(i);
                }
                oversketchingCurve = QPainterPath();

                auto polygon_curve = SketchLibrary::smooth( polygon_list );
                oversketchingCurve.addPolygon(polygon_curve);
                QPolygonF new_sketch;
                for (int j = 0; j < oversketchingCurve.toSubpathPolygons().size(); j++) {
                    new_sketch << oversketchingCurve.toSubpathPolygons().at(i);
                }

                smoothPath(oversketchingCurve);
                smoothPath(oversketchingCurve);
                smoothPath(oversketchingCurve);

                bool changed = SketchLibrary::overSketch( current_sketch, new_sketch );
                //Means that an oversketch happened
                if ( changed )
                {

                    closedContourList[i].contour = QPainterPath();
                    closedContourList[i].contour.addPolygon(current_sketch);

                    closedContourList[i].contour.lineTo(closedContourList[i].contour.pointAtPercent(0));

                    closePath(closedContourList[i].contour);

                    smoothPath(closedContourList[i].contour);
                    allShapesSampledPoints.remove(i);

                    receiveSelectedPath(closedContourList[i].contour,closedContourList[i].name, closedContourList[i].level );
                    oversketchingCurve = QPainterPath();





                } else {

                    qDebug () << "Sketch didn't change!";
                }

            }
        }

    } break;

    case 2 :{/// TODO -> Create Routine for Oversketching for Stripes
        int count = 0;

        for (int i = 0 ; i < sameStripeContourList.size() ; i++){

            //  ------- OVERSKETCH -------
            prepareGeometryChange();
            QPolygonF current_sketch;
            for (int i = 0; i < sameStripeContourList[i].contour.toSubpathPolygons().size(); ++i) {
                current_sketch << sameStripeContourList[i].contour.toSubpathPolygons().at(i);
            }



            QPolygonF polygon_list;

            for (int i = 0; i < oversketchingCurve.toSubpathPolygons().size(); ++i) {
                polygon_list << oversketchingCurve.toSubpathPolygons().at(i);
            }
            oversketchingCurve = QPainterPath();

            auto polygon_curve = SketchLibrary::smooth( polygon_list );
            oversketchingCurve.addPolygon(polygon_curve);
            QPolygonF new_sketch;
            for (int i = 0; i < oversketchingCurve.toSubpathPolygons().size(); ++i) {
                new_sketch << oversketchingCurve.toSubpathPolygons().at(i);
            }

            smoothPath(oversketchingCurve);
            smoothPath(oversketchingCurve);
            smoothPath(oversketchingCurve);

            bool changed = SketchLibrary::overSketch( current_sketch, new_sketch );
            //Means that an oversketch happened
            if ( changed )
            {

                sameStripeContourList[i].contour = QPainterPath();
                sameStripeContourList[i].contour.addPolygon(current_sketch);
                smoothPath(closedContourList[i].contour);
                sameStripeContourList[i].contour = QPainterPath();

            } else {

                qDebug () << "Sketch didn't change!";
            }
            count ++ ;

        }


    } break;


    }

    oversketchingCurve = QPainterPath();

}

void InputSketch::defRotAxis(int direction){

    //Usar Bola do mouse para fazer rotacao ! Por enquanto...

    if (direction == 1){
        allShapesSampledPoints[selectedClosedContour].qr.push_front(allShapesSampledPoints[selectedClosedContour].ql.first());
        allShapesSampledPoints[selectedClosedContour].ql.pop_front();

        allShapesSampledPoints[selectedClosedContour].ql.push_back(allShapesSampledPoints[selectedClosedContour].qr.last());
        allShapesSampledPoints[selectedClosedContour].qr.pop_back();


    } else {
        allShapesSampledPoints[selectedClosedContour].ql.push_front(allShapesSampledPoints[selectedClosedContour].qr.first());
        allShapesSampledPoints[selectedClosedContour].qr.pop_front();

        allShapesSampledPoints[selectedClosedContour].qr.push_back(allShapesSampledPoints[selectedClosedContour].ql.last());
        allShapesSampledPoints[selectedClosedContour].ql.pop_back();
    }


    allShapesSampledPoints[selectedClosedContour].midPoints.clear();
    allShapesSampledPoints[selectedClosedContour].midPointsPath = QPainterPath();

    QPainterPath midPointsPath;

    for (int i = 0; i < allShapesSampledPoints[selectedClosedContour].ql.size() ; ++i) {

        QVector3D temp_ql(allShapesSampledPoints[selectedClosedContour].ql[i].x(),allShapesSampledPoints[selectedClosedContour].ql[i].y(), allShapesSampledPoints[selectedClosedContour].ql[i].z() );

        QVector3D temp_qr(allShapesSampledPoints[selectedClosedContour].qr[i].x(),allShapesSampledPoints[selectedClosedContour].qr[i].y(), allShapesSampledPoints[selectedClosedContour].qr[i].z() );


        allShapesSampledPoints[selectedClosedContour].midPoints.push_back((temp_ql + temp_qr) / 2);

        if (i == 0){
            midPointsPath.moveTo(allShapesSampledPoints[selectedClosedContour].midPoints[0].x(), allShapesSampledPoints[selectedClosedContour].midPoints[0].y());
        } else {
            midPointsPath.lineTo(allShapesSampledPoints[selectedClosedContour].midPoints[i].x(), allShapesSampledPoints[selectedClosedContour].midPoints[i].y());
        }

    }


    allShapesSampledPoints[selectedClosedContour].midPointsPath = midPointsPath;

    //RotationalBlendingSurface(selectedPathIndex, paths[selectedPathIndex] ,allShapesSampledPoints[selectedPathIndex].ql, allShapesSampledPoints[selectedPathIndex].qr);


    update();
}

void InputSketch::samplePointsForRotationalBlendingSurface(QPainterPath &selectedCurve){


    allShapesSampledPoints[selectedClosedContour] = allSampledPoints();

    QVector<QPointF> sampledPointsOnCurve;

    allSampledPoints t;

    for (qreal j = 0; j < selectedCurve.length(); j = j + 5) {

        sampledPointsOnCurve.push_back(selectedCurve.pointAtPercent(selectedCurve.percentAtLength(j)));

    }

    QPainterPath midPointsPath;

    for (int j = 0; j < sampledPointsOnCurve.size()/2 ; j++) {

        QVector3D temp_ql(sampledPointsOnCurve[j].x(),sampledPointsOnCurve[j].y(), 0 );

        QVector3D temp_qr(sampledPointsOnCurve[sampledPointsOnCurve.size()-1-j].x(),sampledPointsOnCurve[sampledPointsOnCurve.size()-1-j].y(),0 );


        t.midPoints.push_back((temp_ql + temp_qr) / 2);
        t.ql.push_back(temp_ql);
        t.qr.push_back(temp_qr);
        if (j == 0){
            midPointsPath.moveTo(t.midPoints[0].x(), t.midPoints[0].y());
        } else {
            midPointsPath.lineTo(t.midPoints[j].x(), t.midPoints[j].y());
        }
    }

    t.midPointsPath = midPointsPath;

    allShapesSampledPoints[selectedClosedContour] = t;

    //RotationalBlendingSurface(selectedPathIndex, selectedCurve, t.ql, t.qr);


}


void InputSketch::smooth() {

    switch (lastSelected) {

    case 0 : {

        int count = 0;

        for (int i = 0; i < openContourList.size(); ++i) {
            for (int j = 0; j < openContourList[i].size(); ++j) {

                if (selectedOpenContour == count){
                    smoothPath(openContourList[i][j].contour);
                    smoothPath(openContourList[i][j].contour);
                    smoothPath(openContourList[i][j].contour);
                }

                count++;
            }
        }
    }

        break;

    case 1 : {

        QPainterPath path;

        for (int i = 0 ; i < closedContourList.size(); i++) {

            if (selectedClosedContour == i){

                path.moveTo(closedContourList[i].contour.pointAtPercent(0));

                for (double k = 0.5; k < closedContourList[i].contour.length(); k = k + 0.5){
                    path.lineTo(closedContourList[i].contour.pointAtPercent(closedContourList[i].contour.percentAtLength(k)));
                }

                smoothPath(path);
                smoothPath(path);
                smoothPath(path);

                closedContourList[i].contour = QPainterPath();
                closedContourList[i].contour = path;
                samplePointsForRotationalBlendingSurface(path);

            }
        }
    }

        break;

    case 2 : {

        int count = 0;

        for (int i = 0 ; i < sameStripeContourList.size() ; i++){


            if (selectedStripeContour == count){
                smoothPath(sameStripeContourList[i].contour);
                smoothPath(sameStripeContourList[i].contour);
                smoothPath(sameStripeContourList[i].contour);

                smoothPath(sameStripeContourList[i].leftLine);
                smoothPath(sameStripeContourList[i].leftLine);
                smoothPath(sameStripeContourList[i].leftLine);

                smoothPath(sameStripeContourList[i].rightLine);
                smoothPath(sameStripeContourList[i].rightLine);
                smoothPath(sameStripeContourList[i].rightLine);


            }
            count ++ ;

        }
    }

        break;

    }

    update();


}


QPainterPath InputSketch::smoothPath(QPainterPath &path){


    auto polygon_list = path.toSubpathPolygons();
    path = QPainterPath();

    for( int i = 0; i < polygon_list.size(); ++i )
    {
        // polygon_curve is a QPolygonF
        auto polygon_curve = SketchLibrary::smooth( polygon_list.at(i) );
        path.addPolygon(polygon_curve);
    }

    return path;
}

void InputSketch::createOversketchingCurve(const QPointF &pos){

    prepareGeometryChange();
    oversketchingCurve.moveTo( mapFromScene( pos ) );

}

void InputSketch::addOversketchingCurve(const QPointF &pos){

    prepareGeometryChange();
    oversketchingCurve.lineTo( mapFromScene( pos ) );

}






//************************************************************************************************
/// ..................................... 3D Rendering ..........................................
//************************************************************************************************


void InputSketch::changeLayerDifference(const int &difference)
{

    layerDifference = difference;

}
QList<QVector<QVector3D> > InputSketch::getOpenContoursPoints() {


    QList<QVector<QVector3D> > pointsFor3D;

    if (!openContourList.isEmpty()) { //Somente os contornos abertos, primeiro caso

        //int depthdiff = QInputDialog::getInt(nullptr, "Depth Difference OPEN CONTOURS", "Layering difference in depth OPEN CONTOURS");
        //int depthdiff = layerDifference;

        for (int i = 0; i < openContourList.size(); i++) {

            QVector<QVector3D> openContour3DPoints;

            for (int j = 0; j < openContourList[i].size(); j++) {

                for (double k = 0; k < openContourList[i][j].contour.length(); k = k + 0.5) {

                    QVector3D p(openContourList[i][j].contour.pointAtPercent(openContourList[i][j].contour.percentAtLength(k)).x(),openContourList[i][j].contour.pointAtPercent(openContourList[i][j].contour.percentAtLength(k)).y(), depthLevelList[openContourList[i][j].level-1]);

                    openContour3DPoints.append(p);

                }
            }

            for (int l = 0; l < layerDifference * 10; l++){ // i < 200
                chaikinOnZ (openContour3DPoints);
            }

            pointsFor3D.append(openContour3DPoints);

        }



    }

    //Smooth points 3D
    //Chaikin

    return pointsFor3D;

}


QVector<QVector3D> InputSketch::getClosedContoursPoints() {

    return pointsFor3Ddisks;
}



QVector<QVector3D> InputSketch::getClosedContoursNormals() {

    return normalsFor3Ddisks;
}

QVector<QVector3D> InputSketch::getStripesPoints () {


    QVector<QVector3D> pointsFor3D, stripeContour3DPoints;
    if (!stripeContourList.isEmpty()) {

        QPolygonF quad;
        QVector<QVector3D> pointsfor3Dleft, pointsfor3Dright;
        QVector<QVector3D> quadMesh;

        //int depthdiff = QInputDialog::getInt(nullptr, "Depth Difference STRIPE", "Layering difference in depth STRIPE");
        int depthdiff = layerDifference;

        for (int i = 0 ; i <  stripeContourList.size(); i++) {

            for (int j = 0 ; j <  stripeContourList[i].size(); j++) {

                float aspectratio = stripeContourList[i][j].rightLine.length() / stripeContourList[i][j].leftLine.length();

                for (int k = 0; k < stripeContourList[i][j].leftLine.length(); k++){
                    QVector3D p(stripeContourList[i][j].leftLine.pointAtPercent(stripeContourList[i][j].leftLine.percentAtLength(k)).x(), stripeContourList[i][j].leftLine.pointAtPercent(stripeContourList[i][j].leftLine.percentAtLength(k)).y(), stripeContourList[i][j].level*depthdiff);

                    pointsfor3Dleft.append(p);
                }

                for (float k = 0; k < stripeContourList[i][j].rightLine.length(); k = k + aspectratio){
                    QVector3D p(stripeContourList[i][j].rightLine.pointAtPercent(stripeContourList[i][j].rightLine.percentAtLength(k)).x(), stripeContourList[i][j].rightLine.pointAtPercent(stripeContourList[i][j].rightLine.percentAtLength(k)).y(), stripeContourList[i][j].level*depthdiff);

                    pointsfor3Dright.append(p);
                }
            }
        }

        for (int i = 0; i < depthdiff * 10; i++){ // i < 200
            chaikinOnZ (pointsfor3Dleft);
        }

        for (int i = 0; i < depthdiff * 10; i++){ // i < 200
            chaikinOnZ (pointsfor3Dright);
        }

        pointsFor3D.append(pointsfor3Dleft);
        pointsFor3D.append(pointsfor3Dright);


    }

    return pointsFor3D;
}



bool InputSketch::chaikinOnZ(QVector<QVector3D> &pointsFor3D){

    //Chaikin
    for (int i = 1; i < pointsFor3D.size()-2; ++i) {
        pointsFor3D[i].setZ(-0.25*pointsFor3D[i-1].z() + 0.75*pointsFor3D[i].z() + 0.75*pointsFor3D[i+1].z() - 0.25*pointsFor3D[i+2].z());
    }

    //meanFilter

    for (int i = 1; i < pointsFor3D.size()-1; ++i) {
        pointsFor3D[i].setZ((pointsFor3D[i-1].z() + (pointsFor3D[i].z() *3) + pointsFor3D[i+1].z()) /5.0);
    }

    for (int i = 1; i < pointsFor3D.size()-1; ++i) {
        pointsFor3D[i].setZ((pointsFor3D[i-1].z() + (pointsFor3D[i].z() *3) + pointsFor3D[i+1].z()) /5.0);
    }

    return true;
}


//************************************************************************************************
/// ........................................ PAINTER ..........................................
//************************************************************************************************

void InputSketch::mouseOverPoint(const QPointF& pos){

    mousePointer = pos;

}


QRectF InputSketch::boundingRect() const
{
    return curve.boundingRect();
}

void InputSketch::setSaveSVG(){
    saveSVGFlag = !saveSVGFlag;
}

void InputSketch::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget ) {
    //void InputSketch::paint( QPainter *painter) {
    
    
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(Qt::black,1,Qt::SolidLine));
    
    QFont font = painter->font() ;
    



    font.setPointSize ( 8 );
    painter->setFont(font);

    Qt::PenStyle penStyle = Qt::SolidLine;

    if (saveSVGFlag){ //SALVAR SVG

        for (int i = 0; i < openContourList.size(); ++i) {
            for (int j = 0; j < openContourList[i].size(); ++j) {


                if (halo){
                    painter->setOpacity(0.5);
                    painter->setPen(QPen(QColor(Qt::white),openContourList[i][j].level+16,penStyle, Qt::FlatCap, Qt::RoundJoin));
                    painter->drawPath(openContourList[i][j].contour);
                    painter->setOpacity(1);
                    painter->setPen(QPen(QColor(Qt::white),openContourList[i][j].level+9,penStyle, Qt::FlatCap, Qt::RoundJoin));
                    painter->drawPath(openContourList[i][j].contour);
                }

                if (contour){
                    painter->setPen(QPen(QColor(Qt::black),openContourList[i][j].level+1.5,penStyle, Qt::FlatCap, Qt::RoundJoin));
                    painter->drawPath(openContourList[i][j].contour);
                    painter->setPen(QPen(QColor(lineColorMap[openContourList[i][j].level-1].x(), lineColorMap[openContourList[i][j].level-1].y(), lineColorMap[openContourList[i][j].level-1].z()),openContourList[i][j].level+1,penStyle, Qt::FlatCap, Qt::RoundJoin));
                    painter->drawPath(openContourList[i][j].contour);
                    painter->setPen(QPen(QColor(Qt::black),1,penStyle, Qt::FlatCap, Qt::RoundJoin));
                    if (showLabels) painter->drawText(openContourList[i][j].contour.elementAt(0).x + 10, openContourList[i][j].contour.elementAt(0).y, openContourList[i][j].name);
                }


            }
        }

        painter->setBrush( Qt::NoBrush );

        for (int i = 0 ; i < closedContourList.size(); i++) {


            if (color){

                painter->setOpacity( 1 /static_cast<qreal>(closedContourList.size()));
                painter->fillPath(closedContourList[i].contour, QColor(shapeColorMap[closedContourList[i].level-1].x(), shapeColorMap[closedContourList[i].level-1].y(), shapeColorMap[closedContourList[i].level-1].z()));
            }

            if (hatching){
                painter->setOpacity( 1 );
                painter->fillPath(closedContourList[i].contour, hatchingMap[closedContourList[i].level-1]);

            }
            if (halo){
                painter->setOpacity(0.5);
                painter->setPen(QPen(QColor(Qt::white),closedContourList[i].level+16,penStyle, Qt::FlatCap, Qt::RoundJoin));
                painter->drawPath(closedContourList[i].contour);
                painter->setOpacity(1);
                painter->setPen(QPen(QColor(Qt::white),closedContourList[i].level+9,penStyle, Qt::FlatCap, Qt::RoundJoin));
                painter->drawPath(closedContourList[i].contour);
            }

            if (contour){
                painter->setPen(QPen(QColor(Qt::black),closedContourList[i].level+1.5,penStyle, Qt::FlatCap, Qt::RoundJoin));
                painter->drawPath(closedContourList[i].contour);


                painter->setPen(QPen(QColor(lineColorMap[closedContourList[i].level-1].x(), lineColorMap[closedContourList[i].level-1].y(), lineColorMap[closedContourList[i].level-1].z()), closedContourList[i].level+1,penStyle, Qt::FlatCap, Qt::RoundJoin));

                //painter->setPen(QPen(QColor(Qt::red), closedContourList[i].level+1,penStyle, Qt::FlatCap, Qt::RoundJoin));
                painter->drawPath(closedContourList[i].contour);
                painter->setPen(QPen(QColor(Qt::black),1,penStyle, Qt::FlatCap, Qt::RoundJoin));
                if (showLabels) painter->drawText(closedContourList[i].contour.elementAt(0).x + 10, closedContourList[i].contour.elementAt(0).y, closedContourList[i].name);
            }

            painter->setPen(QPen(Qt::darkRed, 2,Qt::SolidLine));
            //painter->drawPath(allShapesSampledPoints[i].midPointsPath);

        }

        painter->setPen(QPen(QColor(Qt::blue),2,penStyle, Qt::RoundCap, Qt::RoundJoin));

        //painter->drawEllipse(mousePointer, 2,2);

        //painter->setOpacity(0.5);

        painter->setPen(QPen(QColor(Qt::black),1,penStyle, Qt::RoundCap, Qt::RoundJoin));

        //for (int i = 0; i < pathsList.size(); ++i) {

        //  painter->drawPath(pathsList[i]);

        //}


        for (int i = 0 ; i < sameStripeContourList.size() ; i++){

            painter->setPen(QPen(QColor(Qt::blue),sameStripeContourList[i].level+1,penStyle, Qt::RoundCap, Qt::RoundJoin));
            painter->drawPath(sameStripeContourList[i].leftLine);
            painter->setPen(QPen(QColor(Qt::red),sameStripeContourList[i].level+1,penStyle, Qt::RoundCap, Qt::RoundJoin));
            painter->drawPath(sameStripeContourList[i].rightLine);
            painter->setPen(QPen(QColor(Qt::black),1,penStyle, Qt::RoundCap, Qt::RoundJoin));
            if (showLabels) painter->drawText(sameStripeContourList[i].contour.elementAt(0).x + 10, sameStripeContourList[i].contour.elementAt(0).y, sameStripeContourList[i].name);
        }


        for (int i = 0 ; i < stripeContourList.size() ; i++){

            for (int j = 0; j < stripeContourList[i].size() ; j++) {


                painter->setPen(QPen(QColor(Qt::blue),stripeContourList[i][j].level+1,penStyle, Qt::RoundCap, Qt::RoundJoin));
                painter->drawPath(stripeContourList[i][j].leftLine);
                painter->setPen(QPen(QColor(Qt::red),stripeContourList[i][j].level+1,penStyle, Qt::RoundCap, Qt::RoundJoin));
                painter->drawPath(stripeContourList[i][j].rightLine);
                painter->setPen(QPen(QColor(Qt::black),1,penStyle, Qt::RoundCap, Qt::RoundJoin));
                if (showLabels) painter->drawText(stripeContourList[i][j].leftLine.elementAt(0).x + 10, stripeContourList[i][j].leftLine.elementAt(0).y, stripeContourList[i][j].name);

            }
        }

        saveSVGFlag = !saveSVGFlag;
        return;
    }

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(Qt::black,1,Qt::SolidLine));

    font.setPointSize ( 11 );

    painter->setFont(font);

    painter->drawText(2, 15, interactionString);


    font.setPointSize ( 8 );
    painter->setFont(font);


    
    if (statusSketch == Interaction::DEFAULT || statusSketch == Interaction::CROSS_SELECTION || statusSketch == Interaction::CROP_SELECTION || statusSketch == Interaction::ERASE_SELECTION){
        for (int i = 0; i < svgPaths.size(); i++){
            painter->drawPath(svgPaths[i]);
        }
    }

    
    //    if (selectedPathsList.size() > 0){

    //        painter->setPen(QPen(QColor(Qt::gray),1,Qt::SolidLine));
    //        painter->setOpacity(0.5);

    //        foreach (QPainterPath path, selectedPathsList){
    //            painter->drawPath(path);
    //        }

    //    }

    painter->setOpacity(1);

    
    QConicalGradient conicalGradient;
    
    /// Paint Headlight
    ///
    ///
    ///
    ///
    names.clear();

    if (statusSketch == Interaction::OPENCONTOUR){

        for (int h = numberOfLayers; h > 0; h--) {

            for (int i = 0; i < sameOpenContourList.size(); ++i) {

                if (sameOpenContourList[i].level == h){

                    if (halo){
                        painter->setOpacity(0.5);
                        painter->setPen(QPen(QColor(Qt::black),sameOpenContourList[i].level+16,penStyle, Qt::FlatCap, Qt::RoundJoin));
                        painter->drawPath(sameOpenContourList[i].contour);
                        painter->setOpacity(1);
                        painter->setPen(QPen(QColor(Qt::white),sameOpenContourList[i].level+9,penStyle, Qt::FlatCap, Qt::RoundJoin));
                        painter->drawPath(sameOpenContourList[i].contour);

                    }

                    if (contour){

                        painter->setPen(QPen(QColor(Qt::black),sameOpenContourList[i].level+1.5,penStyle, Qt::FlatCap, Qt::RoundJoin));
                        painter->drawPath(sameOpenContourList[i].contour);
                        painter->setPen(QPen( QColor(lineColorMap[sameOpenContourList[i].level-1].x(), lineColorMap[sameOpenContourList[i].level-1].y(), lineColorMap[sameOpenContourList[i].level-1].z()),sameOpenContourList[i].level+1,penStyle, Qt::FlatCap, Qt::RoundJoin));
                        //painter->setPen(QPen( QColor(Qt::white),sameOpenContourList[i].level+1,penStyle, Qt::FlatCap, Qt::RoundJoin));
                        painter->drawPath(sameOpenContourList[i].contour);
                        if (showLabels) painter->drawText(sameOpenContourList[i].contour.elementAt(0).x + 10, sameOpenContourList[i].contour.elementAt(0).y, sameOpenContourList[i].name);
                    }
                }
            }
            int count = 0;

            for (int i = 0; i < openContourList.size(); ++i) {
                for (int j = 0; j < openContourList[i].size(); ++j) {


                    if (selectedOpenContour == count){

                        painter->setPen(QPen(QColor(Qt::blue),2,penStyle, Qt::FlatCap, Qt::RoundJoin));
                        painter->drawPath(openContourList[i][j].contour);

                        if (showLabels) painter->drawText(openContourList[i][j].contour.elementAt(0).x + 10, openContourList[i][j].contour.elementAt(0).y, openContourList[i][j].name);

                    } else {

                        if (openContourList[i][j].level == h) {

                            if (halo){
                                painter->setOpacity(0.5);
                                painter->setPen(QPen(QColor(Qt::white),openContourList[i][j].level+16,penStyle, Qt::FlatCap, Qt::RoundJoin));
                                painter->drawPath(openContourList[i][j].contour);
                                painter->setOpacity(1);
                                painter->setPen(QPen(QColor(Qt::white),openContourList[i][j].level+9,penStyle, Qt::FlatCap, Qt::RoundJoin));
                                painter->drawPath(openContourList[i][j].contour);
                            }

                            if (contour){
                                painter->setPen(QPen(QColor(Qt::black),openContourList[i][j].level+1.5,penStyle, Qt::FlatCap, Qt::RoundJoin));
                                painter->drawPath(openContourList[i][j].contour);
                                painter->setPen(QPen(QColor(lineColorMap[openContourList[i][j].level-1].x(), lineColorMap[openContourList[i][j].level-1].y(), lineColorMap[openContourList[i][j].level-1].z()),openContourList[i][j].level+1,penStyle, Qt::FlatCap, Qt::RoundJoin));
                                painter->drawPath(openContourList[i][j].contour);
                                painter->setPen(QPen(QColor(Qt::black),1,penStyle, Qt::FlatCap, Qt::RoundJoin));
                                if (showLabels) painter->drawText(openContourList[i][j].contour.elementAt(0).x + 10, openContourList[i][j].contour.elementAt(0).y, openContourList[i][j].name);
                            }
                        }
                    }


                    count++;
                }
            }

        }
    } else {
        //Show Contours with water mark
        for (int i = 0; i < openContourList.size(); ++i) {
            for (int j = 0; j < openContourList[i].size(); ++j) {

                painter->setOpacity(0.5);
                painter->setPen(QPen(QColor(Qt::gray),2,penStyle, Qt::FlatCap, Qt::RoundJoin));
                painter->drawPath(openContourList[i][j].contour);
                if (showLabels) painter->drawText(openContourList[i][j].contour.elementAt(0).x + 10, openContourList[i][j].contour.elementAt(0).y, openContourList[i][j].name);
                painter->setOpacity(1);
            }
        }


    }

    if (statusSketch == Interaction::CLOSEDCONTOUR){

        painter->setBrush( Qt::NoBrush );

        for (int h = numberOfLayers; h > 0; --h) {

            for (int i = 0 ; i < closedContourList.size(); i++) {


                if (selectedClosedContour == i){

                    painter->setPen(QPen(QColor(Qt::blue),2,penStyle, Qt::FlatCap, Qt::RoundJoin));
                    painter->drawPath(closedContourList[i].contour);

                    painter->setPen(QPen(Qt::darkRed, 2,Qt::SolidLine));
                    painter->drawPath(allShapesSampledPoints[i].midPointsPath);

                    if (showLabels) painter->drawText(closedContourList[i].contour.elementAt(0).x + 10, closedContourList[i].contour.elementAt(0).y, closedContourList[i].name);

                } else {
                    if (closedContourList[i].level == h){

                        if (color){

                            painter->setOpacity( 1 /static_cast<qreal>(closedContourList.size()));
                            painter->fillPath(closedContourList[i].contour, QColor(shapeColorMap[closedContourList[i].level-1].x(), shapeColorMap[closedContourList[i].level-1].y(), shapeColorMap[closedContourList[i].level-1].z()));
                        }

                        if (hatching){
                            painter->setOpacity( 1 );
                            painter->fillPath(closedContourList[i].contour, hatchingMap[closedContourList[i].level-1]);

                        }
                        if (halo){
                            painter->setOpacity(0.5);
                            painter->setPen(QPen(QColor(Qt::white),closedContourList[i].level+16,penStyle, Qt::FlatCap, Qt::RoundJoin));
                            painter->drawPath(closedContourList[i].contour);
                            painter->setOpacity(1);
                            painter->setPen(QPen(QColor(Qt::white),closedContourList[i].level+9,penStyle, Qt::FlatCap, Qt::RoundJoin));
                            painter->drawPath(closedContourList[i].contour);
                        }

                        if (contour){
                            painter->setPen(QPen(QColor(Qt::black),closedContourList[i].level+1.5,penStyle, Qt::FlatCap, Qt::RoundJoin));
                            painter->drawPath(closedContourList[i].contour);


                            painter->setPen(QPen(QColor(lineColorMap[closedContourList[i].level-1].x(), lineColorMap[closedContourList[i].level-1].y(), lineColorMap[closedContourList[i].level-1].z()), closedContourList[i].level+1,penStyle, Qt::FlatCap, Qt::RoundJoin));

                            //painter->setPen(QPen(QColor(Qt::red), closedContourList[i].level+1,penStyle, Qt::FlatCap, Qt::RoundJoin));
                            painter->drawPath(closedContourList[i].contour);
                            painter->setPen(QPen(QColor(Qt::black),1,penStyle, Qt::FlatCap, Qt::RoundJoin));
                            if (showLabels) painter->drawText(closedContourList[i].contour.elementAt(0).x + 10, closedContourList[i].contour.elementAt(0).y, closedContourList[i].name);
                        }

                        painter->setPen(QPen(Qt::darkRed, 2,Qt::SolidLine));
                        painter->drawPath(allShapesSampledPoints[i].midPointsPath);
                    }
                }
            }



        }

    } else {

        painter->setOpacity(0.5);
        for (int i = 0 ; i < closedContourList.size(); i++) {

            painter->setPen(QPen(QColor(Qt::gray),2,penStyle, Qt::FlatCap, Qt::RoundJoin));
            painter->drawPath(closedContourList[i].contour);
            if (showLabels) painter->drawText(closedContourList[i].contour.elementAt(0).x + 10, closedContourList[i].contour.elementAt(0).y, closedContourList[i].name);

        }
        painter->setOpacity(1);

    }


    if (statusSketch == Interaction::STRIPES) {

        painter->setPen(QPen(QColor(Qt::blue),2,penStyle, Qt::RoundCap, Qt::RoundJoin));

        painter->drawEllipse(mousePointer, 2,2);

        //painter->setOpacity(0.5);

        painter->setPen(QPen(QColor(Qt::black),1,penStyle, Qt::RoundCap, Qt::RoundJoin));

        //for (int i = 0; i < pathsList.size(); ++i) {

        //  painter->drawPath(pathsList[i]);

        //}

        int count = 0;

        for (int h = numberOfLayers; h > 0; h--) {

            for (int i = 0 ; i < sameStripeContourList.size() ; i++){



                if (selectedStripeContour == count){

                    painter->setPen(QPen(QColor(Qt::black),2,penStyle, Qt::FlatCap, Qt::RoundJoin));
                    painter->drawPath(sameStripeContourList[i].contour);

                    if (showLabels) painter->drawText(sameStripeContourList[i].contour.elementAt(0).x + 10, sameStripeContourList[i].contour.elementAt(0).y, sameStripeContourList[i].name);

                } else {


                    if (sameStripeContourList[i].level == h) {
                        painter->setPen(QPen(QColor(Qt::blue),sameStripeContourList[i].level+1,penStyle, Qt::RoundCap, Qt::RoundJoin));
                        painter->drawPath(sameStripeContourList[i].leftLine);
                        painter->setPen(QPen(QColor(Qt::red),sameStripeContourList[i].level+1,penStyle, Qt::RoundCap, Qt::RoundJoin));
                        painter->drawPath(sameStripeContourList[i].rightLine);
                        painter->setPen(QPen(QColor(Qt::black),1,penStyle, Qt::RoundCap, Qt::RoundJoin));
                        if (showLabels) painter->drawText(sameStripeContourList[i].contour.elementAt(0).x + 10, sameStripeContourList[i].contour.elementAt(0).y, sameStripeContourList[i].name);
                    }
                }
                count ++ ;

            }


            for (int i = 0 ; i < stripeContourList.size() ; i++){

                for (int j = 0; j < stripeContourList[i].size() ; j++) {

                    if (stripeContourList[i][j].level == h) {

                        painter->setPen(QPen(QColor(Qt::blue),stripeContourList[i][j].level+1,penStyle, Qt::RoundCap, Qt::RoundJoin));
                        painter->drawPath(stripeContourList[i][j].leftLine);
                        painter->setPen(QPen(QColor(Qt::red),stripeContourList[i][j].level+1,penStyle, Qt::RoundCap, Qt::RoundJoin));
                        painter->drawPath(stripeContourList[i][j].rightLine);
                        painter->setPen(QPen(QColor(Qt::black),1,penStyle, Qt::RoundCap, Qt::RoundJoin));
                        if (showLabels) painter->drawText(stripeContourList[i][j].leftLine.elementAt(0).x + 10, stripeContourList[i][j].leftLine.elementAt(0).y, stripeContourList[i][j].name);
                    }
                }
            }

            painter->setBrush(Qt::NoBrush);
            painter->setOpacity(0.5);
            painter->setPen(QPen(QColor(Qt::black),2,penStyle, Qt::FlatCap, Qt::RoundJoin));

            QVector3D triangleNormal;

            for (int m = 0; m < triangleMesh.size(); m = m + 3) {

                triangleNormal = QVector3D::crossProduct(triangleMesh[m] - triangleMesh[m+1],triangleMesh[m+1] - triangleMesh[m+2]);

                triangleNormal.normalize();

                if (triangleNormal.z() > 0) {
                    painter->setOpacity(0.3);
                    painter->setPen(QPen(QColor(Qt::red),1,Qt::SolidLine));
                    //painter->setPen(QPen(QColor(Qt::red),2,penStyle, Qt::FlatCap, Qt::RoundJoin));
                } else {
                    painter->setOpacity(0.3);
                    painter->setPen(QPen(QColor(Qt::blue),1,Qt::SolidLine));
                    //painter->setPen(QPen(QColor(Qt::blue),2,penStyle, Qt::FlatCap, Qt::RoundJoin));
                }


                //                    if (triangleNormal.z() > 0) {
                //                        painter->setPen(QPen(QColor(Qt::black),2,penStyle, Qt::FlatCap, Qt::RoundJoin));
                //                        painter->setOpacity(1);
                //                        painter->setBrush(Qt::DiagCrossPattern);
                //                    } else {
                //                        painter->setOpacity(1);
                //                        painter->setPen(QPen(QColor(Qt::white),2,penStyle, Qt::FlatCap, Qt::RoundJoin));
                //                    }
                QPolygonF triangle;

                triangle << triangleMesh[m].toPointF();
                triangle << triangleMesh[m+1].toPointF();
                triangle << triangleMesh[m+2].toPointF();

                painter->drawPolygon(triangle);


                painter->setBrush(Qt::NoBrush);




                //            foreach (QPainterPath path, finalSecondBandLineListRight) {
                //                painter->setBrush(Qt::NoBrush);
                //                painter->setOpacity(0.5);
                //                painter->setPen(QPen(QColor(Qt::white),16,penStyle, Qt::FlatCap, Qt::RoundJoin));
                //                painter->drawPath(path);
                //                painter->setOpacity(1);
                //                painter->setPen(QPen(QColor(Qt::white),9,penStyle, Qt::FlatCap, Qt::RoundJoin));
                //                painter->drawPath(path);
                //                painter->setPen(QPen(QColor(Qt::black),3,penStyle, Qt::FlatCap, Qt::RoundJoin));
                //                painter->drawPath(path);
                //                painter->setPen(QPen(QColor(Qt::blue),2,penStyle, Qt::FlatCap, Qt::RoundJoin));
                //                painter->drawPath(path);
                //            }


                //            foreach (QPainterPath path, finalSecondBandLineListLeft) {
                //                painter->setOpacity(0.5);
                //                painter->setPen(QPen(QColor(Qt::white),16,penStyle, Qt::FlatCap, Qt::RoundJoin));
                //                painter->drawPath(path);
                //                painter->setOpacity(1);
                //                painter->setPen(QPen(QColor(Qt::white),9,penStyle, Qt::FlatCap, Qt::RoundJoin));
                //                painter->drawPath(path);
                //                painter->setPen(QPen(QColor(Qt::black),3,penStyle, Qt::FlatCap, Qt::RoundJoin));
                //                painter->drawPath(path);
                //                painter->setPen(QPen(QColor(Qt::red),2,penStyle, Qt::FlatCap, Qt::RoundJoin));
                //                painter->drawPath(path);
                //            }

            }

        }
    } else {

        for (int i = 0 ; i < stripeContourList.size() ; i++){

            for (int j = 0; j < stripeContourList[i].size() ; j++) {

                painter->setPen(QPen(QColor(Qt::gray),1,penStyle, Qt::RoundCap, Qt::RoundJoin));
                painter->drawPath(stripeContourList[i][j].leftLine);
                painter->setPen(QPen(QColor(Qt::gray),1,penStyle, Qt::RoundCap, Qt::RoundJoin));
                painter->drawPath(stripeContourList[i][j].rightLine);
                painter->setPen(QPen(QColor(Qt::gray),2,penStyle, Qt::FlatCap, Qt::RoundJoin));
                if (showLabels) painter->drawText(stripeContourList[i][j].leftLine.elementAt(0).x + 10, stripeContourList[i][j].leftLine.elementAt(0).y, stripeContourList[i][j].name);

            }
        }

    }



    painter->setPen(QPen(QColor(Qt::black),lineLevel+1.5,penStyle, Qt::FlatCap, Qt::RoundJoin));
    painter->drawPath(openContour);


    if (!closedContour.isEmpty()){

        painter->setPen(QPen(QColor(Qt::black),lineLevel+1.5,penStyle, Qt::FlatCap, Qt::RoundJoin));
        painter->drawPath(closedContour);

    }

    if (!closedContourList.isEmpty() && !closedContour.isEmpty()){

        painter->setPen(Qt::SolidLine);

        double last2pointsAngle; //angle

        last2pointsAngle = (closedContour.angleAtPercent(1) + closedContour.angleAtPercent(closedContour.percentAtLength(closedContour.length()-5))) /2;

        foreach (QPainterPath poly, linesToPolygon){

            painter->setOpacity((fabs (last2pointsAngle - poly.angleAtPercent(0)) / 150));

            if (painter->opacity() < 0.2 && painter->opacity() > 0.18){

                painter->setOpacity(1);

                QLinearGradient linearGrad(poly.pointAtPercent(0), poly.pointAtPercent(1));

                //The difference can be between 0 and 180

                QLineF arrow;

                arrow.setP1(closedContour.pointAtPercent(1));
                arrow.setLength(50);
                arrow.setAngle(last2pointsAngle);

                painter->setPen(Qt::DashLine);
                painter->drawLine(arrow);

                QLineF arrowLeft, arrowRight;
                arrowLeft.setP1(arrow.p2());
                arrowRight.setP1(arrow.p2());
                arrowLeft.setLength(5);
                arrowRight.setLength(5);
                arrowLeft.setAngle(arrow.angle()-157.5);
                arrowRight.setAngle(arrow.angle()-202.5);


                painter->setPen(Qt::SolidLine);
                painter->drawLine(arrowLeft);
                painter->drawLine(arrowRight);

                painter->fillPath (poly, QColor(Qt::black));

            }

        }

        linesToPolygon.clear();
    }

    if (!stripeContour.isEmpty()){

        painter->setPen(QPen(QColor(Qt::black),1,penStyle, Qt::RoundCap, Qt::RoundJoin));
        painter->drawPath(stripeContour);

        painter->drawEllipse(mousePointer, 2,2);

        QLineF stripeLeftLine, stripeRightLine;

        for (int i = 0; i < stripeContour.length(); ++i) {

            stripeRightLine.setP1(stripeContour.pointAtPercent((stripeContour.percentAtLength(i))));
            stripeLeftLine.setP1(stripeContour.pointAtPercent((stripeContour.percentAtLength(i))));

            stripeRightLine.setAngle(stripeContour.angleAtPercent(stripeContour.percentAtLength(0))+90);
            stripeLeftLine.setAngle(stripeContour.angleAtPercent(stripeContour.percentAtLength(0))-90);

            stripeRightLine.setLength(twistingThickness);
            stripeLeftLine.setLength(twistingThickness);

            if(front){

                painter->setPen(QPen(QColor(Qt::red),2,penStyle, Qt::RoundCap, Qt::RoundJoin));
                painter->drawPoint(stripeLeftLine.p2());
                painter->setPen(QPen(QColor(Qt::blue),2,penStyle, Qt::RoundCap, Qt::RoundJoin));
                painter->drawPoint(stripeRightLine.p2());
            } else {
                painter->setPen(QPen(QColor(Qt::blue),2,penStyle, Qt::RoundCap, Qt::RoundJoin));
                painter->drawPoint(stripeLeftLine.p2());
                painter->setPen(QPen(QColor(Qt::red),2,penStyle, Qt::RoundCap, Qt::RoundJoin));
                painter->drawPoint(stripeRightLine.p2());
            }
        }

    }

    if (!curve.isEmpty()){

        switch (statusSketch) {

        case Interaction::MOVE_ZOOM:

            break;

        case Interaction::CROSS_SELECTION:

            painter->setPen(QPen(QColor(Qt::green),2,Qt::SolidLine));
            painter->drawPath(curve);
            break;

        case Interaction::CROP_SELECTION:

            painter->setPen(QPen(QColor(0xFF, 0, 0, 0x80),6,Qt::SolidLine));
            painter->drawPath(curve);
            break;

        case Interaction::ERASE_SELECTION:

            painter->setPen(QPen(QColor(Qt::red),2,Qt::SolidLine));
            painter->drawPath(curve);
            break;

        }

    }
    painter->setPen(QPen(Qt::black,1,Qt::SolidLine));

    //if(statusSketch == Interaction::OPENCONTOUR || statusSketch == Interaction::CLOSEDCONTOUR || statusSketch == Interaction::STRIPES){
    painter->drawText(mousePointer.x(), mousePointer.y() - 5, QString::number(lineLevel));
    //}

    if (!oversketchingCurve.isEmpty()){
        painter->setPen(QPen(QColor(Qt::darkGreen),2,Qt::SolidLine));
        painter->drawPath(oversketchingCurve);
    }




}



//************************************************************************************************
/// ..................................... FILE MANAGER ..........................................
//************************************************************************************************



void InputSketch::clear()
{
    prepareGeometryChange();

    curve = QPainterPath();
    pathsList.clear();

    svgPaths.clear();

    secondbandLineListLeft.clear();
    secondbandLineListRight.clear();

    pathsfor3D.clear();
    levelListfor3D.clear();

    selectedPathsList.clear();

    linesToPolygon.clear();


    curvePolygon = QPolygonF();
    pathPolygon = QPolygonF();
    currentPolySugestion = QPolygonF();

    setPath( curve );
    //New Clear
    cyclesList.clear();
    twistingThickness = 10;
    lineLevel = 1;
    layerDifference = 10;

    update();
}



//************************************************************************************************
/// ..................................... GETTERS ..........................................
//************************************************************************************************

QPainterPath InputSketch::getCrossSelectionPath() const
{
    return selectedPathOnCrossSelection;
}

int InputSketch::getLineLevel() const {
    return selectedLineLevel;
}

QVector<QPainterPath> InputSketch::getCropSelectionPaths() const
{
    return selectedPathsOnCropSelection;
}

QPainterPath InputSketch::getClosedContour()
{

    return closedContourList.last().contour;

}

int InputSketch::getClosedContourLevel()
{

    return closedContourList.last().level;

}

QString InputSketch::getPathNames()
{
    //qDebug () << names.size();
    return names.last();
}

bool InputSketch::isOpenContoursEmpty(){

    return openContourList.isEmpty();
}

bool InputSketch::isClosedContoursEmpty(){

    return closedContourList.isEmpty();
}

bool InputSketch::isStripeContoursEmpty(){
    return stripeContourList.isEmpty();
}

int InputSketch::getErasedTypeContour(){
    return lastSelected;
}

int InputSketch::getErasedOpenContourNumber(){
    return selectedOpenContour;
}

int InputSketch::getErasedClosedContourNumber(){
    return selectedClosedContour;
}

int InputSketch::getErasedStripeContourNumber(){
    return selectedStripeContour;
}


QList<QString> InputSketch::getLayerList(){

    QList<QString> a;
    for (int i = 0; i < depthLevelList.size() ; i++){
        a.push_back(QString::number(depthLevelList[i]));
    }

    return a;
}


//************************************************************************************************
/// ..................................... SETTERS ..........................................
//************************************************************************************************

void InputSketch::setShowLabels(bool _showLabels)
{
    showLabels = _showLabels;
    update();

}

void InputSketch::selectOpenContour(const int openContourIndex)
{
    selectedOpenContour = openContourIndex;
    lastSelected = 0;
}


void InputSketch::selectClosedContour(const int closedContourIndex)
{
    selectedClosedContour = closedContourIndex;
    lastSelected = 1;
}

void InputSketch::selectStripeContour(const int stripeContourIndex)
{
    selectedStripeContour = stripeContourIndex;
    lastSelected = 2;
}

void InputSketch::renameOpenContour(int itemNumber, QString name){

    int count = 0;
    for (int i = 0; i < openContourList.size(); ++i) {
        for (int j = 0; j < openContourList[i].size(); ++j) {
            if (itemNumber == count){
                openContourList[i][j].name.clear();
                openContourList[i][j].name = name;

            }
            count++;
        }
    }

    update();

}

void InputSketch::renameClosedContour(int itemNumber, QString name){

    for (int i = 0 ; i < closedContourList.size(); i++) {

        if (itemNumber == i){
            closedContourList[i].name.clear();
            closedContourList[i].name = name;

        }
    }

    update();
}

void InputSketch::renameStripeContour(int itemNumber, QString name){
    int count = 0;

    for (int i = 0 ; i < stripeContourList.size() ; i++){

        for (int j = 0; j < stripeContourList[i].size() ; j++) {
            if (itemNumber == count){

                stripeContourList[i][j].name.clear();
                stripeContourList[i][j].name = name;

            }

            count ++;
        }
    }
}

void InputSketch::setDepths(int row, QString depth){

    depthLevelList[row] = depth.toFloat();

    //qDebug () << depthLevelList[row];

}

//************************************************************************************************
/// ........................................ OTHERS .............................................
//************************************************************************************************


int InputSketch::number_subpaths()
{
    QList< QPolygonF > subpaths =  curve.toSubpathPolygons();
    std::cout << "Number of subpaths: " << subpaths.size() << std::endl << std::flush;
    return subpaths.size();
}


bool InputSketch::almostEqual(QPointF a, QPointF b)
{
    //return fabs(a - b) <= FLT_EPSILON;
    return fabs(a.x() - b.x()) <= 2 && fabs(a.y() - b.y()) <=2 ;
}


float InputSketch::getPathArea(QPainterPath p, float step)
{
    QPointF a,b;
    float len;
    float area=0;
    for(len=0; len<p.length(); len+=step)
    {
        a=p.pointAtPercent(p.percentAtLength(len));
        b=p.pointAtPercent(p.percentAtLength(len+step>p.length()?p.length():len+step));
        area+=(b.x() - a.x())*(a.y()+b.y());
    }
    return area/float(2);
}


