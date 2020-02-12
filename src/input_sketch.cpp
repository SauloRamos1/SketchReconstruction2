#include <iostream>

#include <QPainter>
#include <QDebug>
#include <QMatrix4x4>


#include "include/input_sketch.h"


InputSketch::InputSketch()
{
    setPath (curve);
    setPath (openContour);
    setPath (closedContour);
    
    updateColorMap();
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

void InputSketch::saveOpenContour (){

    if (openContour.length() < 2) return; //Testa se ha linha desenhada

    smoothPath(openContour);
    smoothPath(openContour);
    smoothPath(openContour);

    QPainterPath3D curve3D;
    curve3D.contour = openContour;
    curve3D.level = lineLevel;
    QString name = "Open ";
    name.append(openContourList.size());
    curve3D.name = name;

    sameOpenContourList.append( curve3D );
    openContourList.append( sameOpenContourList );
    sameOpenContourList.clear();
    openContour = QPainterPath();
    update();
}

void InputSketch::increaseOpenContourLevelWhileDrawing(){

    if (openContour.length() < 2) return; //Testa se ha linha desenhada

    smoothPath(openContour);
    smoothPath(openContour);
    smoothPath(openContour);


    QPainterPath3D curve3D;
    curve3D.contour = openContour;
    curve3D.level = lineLevel;

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

            linesToPolygon.clear();

            curvePolygon = closedContour.toFillPolygon();

            pathPolygon = pathPolygon.united(closedContourList[i].contour.toFillPolygon().intersected(curvePolygon));

            if (closedContourList[i].contour.contains(closedContour.pointAtPercent(1))){

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

void InputSketch::saveClosedContour (){

    if (closedContour.length() < 2) return; //Testa se ha linha desenhada

    smoothPath(closedContour);
    smoothPath(closedContour);
    smoothPath(closedContour);

    QPainterPath3D curve3D;
    curve3D.contour = closePath(closedContour);
    smoothPath(curve3D.contour);
    curve3D.level = lineLevel;

    QString name = "Closed ";
    name.append(closedContourList.size());
    curve3D.name = name;

    receiveSelectedPath(curve3D.contour, curve3D.name, curve3D.level);

    closedContourList.append(curve3D);
    closedContour = QPainterPath();

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

        QVector3D temp_ql(sampledPointsOnCurve[j].x(),sampledPointsOnCurve[j].y(), (lineLevel*layerDifference));
        qreal temp_angleql(angleAtSampledPoint[j]);

        QVector3D temp_qr(sampledPointsOnCurve[sampledPointsOnCurve.size()-1-j].x(),sampledPointsOnCurve[sampledPointsOnCurve.size()-1-j].y(), (lineLevel*layerDifference));
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

   // int lineLeveldiff = layerDifference;

    //lineLeveldiff = QInputDialog::getInt(nullptr, "Depth Difference", "Layering difference in depth");
    //lineLeveldiff*=-1;

    for (int i = 0; i < allShapesSampledPoints.size(); ++i) {

        int reconstType = 0;

        while (reconstType < 1 || reconstType > 2){
            reconstType = QInputDialog::getInt(nullptr, "Select Reconstruction Type for " + allShapesSampledPoints[i].name, "Select Reconstruction Type for: " + allShapesSampledPoints[i].name + " \n 1 - Rotational Blending Surface \n 2 - Hermitian Radial Basis Function");
        }
        if (reconstType == 1) {
            RotationalBlendingSurface(i, allShapesSampledPoints[i].contour, allShapesSampledPoints[i].ql,allShapesSampledPoints[i].qr);

        } else {
            DataForHRBF (i, allShapesSampledPoints[i].contour, allShapesSampledPoints[i].ql,allShapesSampledPoints[i].qr);
        }
    }

    update();

}


void InputSketch::RotationalBlendingSurface(const int shapeNumber, QPainterPath &contour, QVector<QVector3D>& ql, QVector<QVector3D>& qr){

    //Normal Plane

    for (int i = 0; i < ql.size(); ++i) {
        ql[i].setZ(ql[i].z()*layerDifference);
        qr[i].setZ(qr[i].z()*layerDifference);
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

}


void InputSketch::DataForHRBF(const int shapeNumber, QPainterPath &contour, QVector<QVector3D>& ql, QVector<QVector3D>& qr){

    QVector <QVector3D> knownPoints;
    QVector <QVector3D> knownNormals;
    //QVector <QVector3D> totalPoints;
    //QVector <QVector3D> totalNormals;

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
                    p.setZ(p.z() * -1); //Adjustable
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
        fOut << p.x() <<" " << p.y()<< " " << p.z()<< std::endl;
        pointsFor3Ddisks.push_back(p);
    }

    for(QVector3D n:knownNormals){
        normalsFor3Ddisks.push_back(n);
    }

    fOut << totalNormals.size() << std::endl;

    for(int k = 0; k < totalNormals.size(); k++)
    {

        QVector3D p = totalPoints.at(k);
        QVector3D n = totalNormals.at(k);

        fOut << n.x() <<" " << n.y()<< " " << n.z()<< std::endl;
        p.setZ(p.z() + (layerDifference * ql[0].z()));
        fOut << p.x() <<" " << p.y()<< " " << p.z()<< std::endl;
        pointsFor3Ddisks.push_back(p);
        normalsFor3Ddisks.push_back(n);

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

    if (sameStripeContourList.size() > 1){

        for (int i = 0; i < sameStripeContourList.size()-1; ++i) {

            if (sameStripeContourList[i].level == sameStripeContourList[i+1].level) {

                sameStripeContourList[i].contour.connectPath(sameStripeContourList[i+1].contour);

                sameStripeContourList[i].leftLine.connectPath(sameStripeContourList[i+1].leftLine);
                sameStripeContourList[i].rightLine.connectPath(sameStripeContourList[i+1].rightLine);

                sameStripeContourList.removeAt(i+1);
                i--;

            } else {

                //            finalSecondBandLineListLeft.append(secondbandLineListLeft[i]);
                //            finalSecondBandLineListRight.append(secondbandLineListRight[i]);
                //            newLevelList.append(levelList[i]);
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

    qDebug ()<< stripeContourList.size();
}


void InputSketch::saveStripeContour (){

    if (stripeContour.length() < 2) return; //Testa se ha linha desenhada

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
    name.append(stripeContourList.size());
    stripe.name = name;

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

    }
    else {

        stripe.leftLine = rightBandLine;
        stripe.rightLine = leftBandLine;
        sameStripeContourList.append(stripe);
    }

    stripeContour = QPainterPath ();

    //Invert Band
    front = !front ;

}


void InputSketch::increaseStripeContourLevelWhileDrawing(){

    if (stripeContour.length() < 2) return; //Testa se ha linha desenhada

    lineLevel +=1;

    // smoothPath(stripeContour);
    // smoothPath(stripeContour);
    // smoothPath(stripeContour);

    Stripe3D stripe;
    stripe.contour = QPainterPath ();
    stripe.contour = stripeContour;

    stripe.leftLine = QPainterPath();
    stripe.rightLine = QPainterPath();

    QPainterPath leftBandLine, rightBandLine;

    QLineF stripeLeftLine, stripeRightLine;

    stripeRightLine.setP1(stripeContour.pointAtPercent((stripeContour.percentAtLength(0))));
    stripeLeftLine.setP1(stripeContour.pointAtPercent((stripeContour.percentAtLength(0))));
    stripeRightLine.setLength(twistingThickness);
    stripeLeftLine.setLength(twistingThickness);

    stripeRightLine.setAngle(stripeContour.angleAtPercent(stripeContour.percentAtLength(0))+90);
    stripeLeftLine.setAngle(stripeContour.angleAtPercent(stripeContour.percentAtLength(0))-90);

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
}


void InputSketch::decreaseStripeContourLevelWhileDrawing(){

    if (stripeContour.length() < 2) return; //Testa se ha linha desenhada

    lineLevel -=1;

    //  smoothPath(stripeContour);
    //  smoothPath(stripeContour);
    //  smoothPath(stripeContour);

    Stripe3D stripe;
    stripe.contour = QPainterPath ();
    stripe.contour = stripeContour;

    stripe.leftLine = QPainterPath();
    stripe.rightLine = QPainterPath();

    QPainterPath leftBandLine, rightBandLine;

    QLineF stripeLeftLine, stripeRightLine;

    stripeRightLine.setP1(stripeContour.pointAtPercent((stripeContour.percentAtLength(0))));
    stripeLeftLine.setP1(stripeContour.pointAtPercent((stripeContour.percentAtLength(0))));
    stripeRightLine.setLength(twistingThickness);
    stripeLeftLine.setLength(twistingThickness);

    stripeRightLine.setAngle(stripeContour.angleAtPercent(stripeContour.percentAtLength(0))+90);
    stripeLeftLine.setAngle(stripeContour.angleAtPercent(stripeContour.percentAtLength(0))-90);

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

    selectedPathOnCrossSelection = QPainterPath();

    ///Interaction -- Cross the Path
    ///
    ///
    if (svgPaths.size() > 0){

        for (int i = 0 ; i < svgPaths.size(); i++ ) {

            if (svgPaths.at(i).intersects(curve)) {
                selectedPathOnCrossSelection = svgPaths[i];
                svgPaths[i]  = QPainterPath();
                if(levelList.size() > 0){
                    selectedLineLevel = levelList[i];
                    //levelList.removeAt(i);
                }

                selectedPathsList.append(selectedPathOnCrossSelection);
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

    selectedPathsList.append(selectedPathsOnCropSelection);

    curve = QPainterPath();
}

void InputSketch::eraseSelection(){

    /// Interaction -- Erase Paths
    ///
    ///
    if (svgPaths.size() > 0){

        for (int i = 0 ; i < svgPaths.size(); i++ ) {

            if (svgPaths[i].intersects(curve)) {
                svgPaths[i] = QPainterPath();
            }
        }
    }

    curve = QPainterPath();

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

bool InputSketch::joinPaths()
{

    if( pathsList.size() < 2 ){
        return false;
    }

    prepareGeometryChange();
    QPolygonF current_sketch  = pathsList[pathsList.size()-2].toSubpathPolygons().at(0);
    QPolygonF new_sketch = pathsList.last().toSubpathPolygons().at(0);

    bool changed = SketchLibrary::overSketch( current_sketch, new_sketch );
    //Means that an oversketch happened
    if ( changed )
    {

        pathsList[pathsList.size()-2] = QPainterPath();
        pathsList[pathsList.size()-2].addPolygon(current_sketch);
        pathsList.removeLast();

    }
    return changed;
}

void InputSketch::smooth()
{

    if( curve.isEmpty() == true )
        return;

    prepareGeometryChange();

    // polygon_list is a QList<QPolygonF> object
    auto polygon_list = curve.toSubpathPolygons();
    curve = QPainterPath();

    for( int i = 0; i < polygon_list.size(); ++i )
    {
        // polygon_curve is a QPolygonF
        auto polygon_curve = SketchLibrary::smooth( polygon_list.at(i) );
        curve.addPolygon(polygon_curve);

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
        int depthdiff = layerDifference;

        for (int i = 0; i < openContourList.size(); i++) {

            QVector<QVector3D> openContour3DPoints;

            for (int j = 0; j < openContourList[i].size(); j++) {

                for (double k = 0; k < openContourList[i][j].contour.length(); k = k + 0.5) {

                    QVector3D p(openContourList[i][j].contour.pointAtPercent(openContourList[i][j].contour.percentAtLength(k)).x(),openContourList[i][j].contour.pointAtPercent(openContourList[i][j].contour.percentAtLength(k)).y(), depthdiff*openContourList[i][j].level);

                    openContour3DPoints.append(p);

                }
            }

            for (int l = 0; l < depthdiff * 10; l++){ // i < 200
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

        int depthdiff = QInputDialog::getInt(nullptr, "Depth Difference STRIPE", "Layering difference in depth STRIPE");

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


void InputSketch::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget ) {
    
    
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(Qt::black,1,Qt::SolidLine));
    
    QFont font = painter->font() ;
    
    font.setPointSize ( 11 );
    
    painter->setFont(font);
    painter->drawText(2, 15, interactionString);
    
    
    
    
    if (svgPaths.size() > 0 && statusSketch == Interaction::DEFAULT){
        foreach (QPainterPath path, svgPaths){
            painter->drawPath(path);
        }
    }
    
    
    if (selectedPathsList.size() > 0){
        painter->setPen(QPen(QColor(Qt::gray),1,Qt::SolidLine));
        painter->setOpacity(0.5);
        foreach (QPainterPath path, selectedPathsList){
            painter->drawPath(path);
        }
    }
    painter->setOpacity(1);
    
    
    Qt::PenStyle penStyle = Qt::SolidLine;
    
    QConicalGradient conicalGradient;
    
    /// Paint Headlight
    ///
    ///
    ///
    
    if (statusSketch == Interaction::OPENCONTOUR){
        
        for (int i = 0; i < sameOpenContourList.size(); ++i) {
            
            
            
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
            }
        }
        
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
                }
            }
        }
    }
    
    if (statusSketch == Interaction::CLOSEDCONTOUR){
        
        
        painter->setBrush( Qt::NoBrush );

        if (color){
            
            for (int i = 0 ; i < closedContourList.size(); i++) {
                

                // painter->setOpacity( 1 /static_cast<qreal>(closedContourList.size()));
                
                painter->fillPath(closedContourList[i].contour, QColor(shapeColorMap[closedContourList[i].level-1].x(), shapeColorMap[closedContourList[i].level-1].y(), shapeColorMap[closedContourList[i].level-1].z()));
                
            }
            
        }
        
        if (hatching){

            for (int i = 0 ; i < closedContourList.size(); i++) {
                
                painter->setOpacity( 1 );
                painter->fillPath(closedContourList[i].contour, hatchingMap[closedContourList[i].level-1]);
                
            }
        }
        
        for (int i = 0; i < closedContourList.size(); i++) {
            
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
            }
        }

        painter->setPen(QPen(Qt::darkRed, 2,Qt::SolidLine));

        for (int i = 0; i < allShapesSampledPoints.size(); ++i) {
            painter->drawPath(allShapesSampledPoints[i].midPointsPath);
        }

    }
    
    
    if (statusSketch == Interaction::STRIPES) {
        
        painter->setPen(QPen(QColor(Qt::blue),2,penStyle, Qt::RoundCap, Qt::RoundJoin));
        
        painter->drawEllipse(mousePointer, 2,2);
        
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
            
        }
        
        
        for (int i = 0 ; i < stripeContourList.size() ; i++){
            
            for (int j = 0; j < stripeContourList[i].size() ; j++) {
                
                painter->setPen(QPen(QColor(Qt::blue),stripeContourList[i][j].level+1,penStyle, Qt::RoundCap, Qt::RoundJoin));
                painter->drawPath(stripeContourList[i][j].leftLine);
                painter->setPen(QPen(QColor(Qt::red),stripeContourList[i][j].level+1,penStyle, Qt::RoundCap, Qt::RoundJoin));
                painter->drawPath(stripeContourList[i][j].rightLine);
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
    
    
    if (!openContour.isEmpty()){
        painter->setPen(QPen(QColor(Qt::black),lineLevel+1.5,penStyle, Qt::FlatCap, Qt::RoundJoin));
        painter->drawPath(openContour);
        painter->setPen(QPen(QColor(Qt::white),lineLevel+1,penStyle, Qt::FlatCap, Qt::RoundJoin));
        painter->drawPath(openContour);
        
    }
    
    if (!closedContour.isEmpty()){
        
        painter->setPen(QPen(QColor(Qt::black),lineLevel+1.5,penStyle, Qt::FlatCap, Qt::RoundJoin));
        painter->drawPath(closedContour);
        painter->setPen(QPen(QColor(Qt::white),lineLevel+1,penStyle, Qt::FlatCap, Qt::RoundJoin));
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
    
    if(statusSketch == Interaction::OPENCONTOUR || statusSketch == Interaction::CLOSEDCONTOUR || statusSketch == Interaction::STRIPES){
        painter->drawText(mousePointer.x(), mousePointer.y() - 5, QString::number(lineLevel));
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
    levelList.clear();
    
    svgPaths.clear();
    
    secondbandLineListLeft.clear();
    secondbandLineListRight.clear();
    
    pathsfor3D.clear();
    levelListfor3D.clear();
    
    selectedPathsList.clear();
    
    linesToPolygon.clear();
    
    closingLines1.clear();
    closingLines2.clear();
    closingLines3.clear();
    
    
    curvePolygon = QPolygonF();
    pathPolygon = QPolygonF();
    currentPolySugestion = QPolygonF();
    
    setPath( curve );
    
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
//************************************************************************************************
/// ..................................... OTHERS ..........................................
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


