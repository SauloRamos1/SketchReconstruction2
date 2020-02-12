#include <QPainter>
#include <QDebug>
#include <QList>
#include <QVector3D>
#include <QVector4D>
#include <QMatrix4x4>
#include <QtMath>
#include <QHBoxLayout>



#include <fstream>
#include <math.h>

#include "include/sketch_library.h"
#include "include/input_sketch2.h"



InputSketch2::InputSketch2()
{

    setPath (curve);
    setPath (oversketchingCurve);

}

void InputSketch2::create (const QPointF& pos){

    prepareGeometryChange();
    oversketchingCurve.moveTo( mapFromScene( pos ) );

}

void InputSketch2::add (const QPointF& pos){

    prepareGeometryChange();
    oversketchingCurve.lineTo( mapFromScene( pos ) );

}

QRectF InputSketch2::boundingRect() const
{
    return curve.boundingRect();
}


void InputSketch2::smooth(){


    prepareGeometryChange();


    QPolygonF polygon_list;

    for (int i = 0; i < curve.toSubpathPolygons().size(); ++i) {
        polygon_list << curve.toSubpathPolygons().at(i);
    }

    curve = QPainterPath();

    auto polygon_curve = SketchLibrary::smooth( polygon_list );
    curve.addPolygon(polygon_curve);


    paths[selectedPathIndex] = QPainterPath();

    samplePointsForRotationalBlendingSurface(curve);

    paths[selectedPathIndex] = curve;

    update();


}

bool InputSketch2::joinPaths(){


    prepareGeometryChange();
    QPolygonF current_sketch;
    for (int i = 0; i < curve.toSubpathPolygons().size(); ++i) {
        current_sketch << curve.toSubpathPolygons().at(i);
    }

    //  ------- Smooth -------

    QPolygonF polygon_list;

    for (int i = 0; i < oversketchingCurve.toSubpathPolygons().size(); ++i) {
        polygon_list << oversketchingCurve.toSubpathPolygons().at(i);
    }

    oversketchingCurve = QPainterPath();

    auto polygon_curve = SketchLibrary::smooth( polygon_list );
    oversketchingCurve.addPolygon(polygon_curve);

    //  ------- Smooth -----

    QPolygonF new_sketch;
    for (int i = 0; i < oversketchingCurve.toSubpathPolygons().size(); ++i) {
        new_sketch << oversketchingCurve.toSubpathPolygons().at(i);
    }




    bool changed = SketchLibrary::overSketch( current_sketch, new_sketch );
    if ( !changed )
    {
        qDebug () << "Sketch didn't change!";
    }


    oversketchingCurve = QPainterPath();
    curve = QPainterPath();
    curve.addPolygon( current_sketch );

    paths[selectedPathIndex] = QPainterPath();

    samplePointsForRotationalBlendingSurface(curve);

    paths[selectedPathIndex] = curve;



    return changed;
}


//--------------- MOVED TO INPUTSKETCH1
void InputSketch2::receiveSelectedPath (const QVector<QPainterPath>& pathList, const QVector<QString>& namePathList, const QVector<int>& lineLevels) {

    paths.clear();
    paths = pathList;

    names.clear();
    names = namePathList;


    for (int i = allShapesSampledPoints.size() ; i < paths.size() ; i++) {


        QVector<QPointF> sampledPointsOnCurve;
        QVector<qreal> angleAtSampledPoint;

        allSampledPoints t = allSampledPoints();




        for (qreal j = 0; j < paths[i].length(); j = j + contourSamplingStep) {

            sampledPointsOnCurve.push_back(paths[i].pointAtPercent(paths[i].percentAtLength(j)));
            angleAtSampledPoint.push_back(paths[i].angleAtPercent(paths[i].percentAtLength(j)));

        }



        for (int j = 0; j < sampledPointsOnCurve.size()/2 ; j++) {

            QVector3D temp_ql(sampledPointsOnCurve[j].x(),sampledPointsOnCurve[j].y(), (lineLevels[i]));
            qreal temp_angleql(angleAtSampledPoint[j]);

            QVector3D temp_qr(sampledPointsOnCurve[sampledPointsOnCurve.size()-1-j].x(),sampledPointsOnCurve[sampledPointsOnCurve.size()-1-j].y(), (lineLevels[i]));
            qreal temp_angleqr(angleAtSampledPoint[angleAtSampledPoint.size()-1-j]);

            t.ql.push_back(temp_ql);
            t.anglesql.push_back(temp_angleql);
            t.qr.push_back(temp_qr);
            t.anglesqr.push_back(temp_angleqr);

        }

        // QVector3D midPointOnMiddle =  ( t.ql[t.ql.size()/2] + t.qr[t.qr.size()/2] ) /2;

        qDebug () << t.ql;



        for (int j = 0; j < t.ql.size(); ++j) {

            if (j == 0){
                lastmidPointsPath.moveTo(((t.ql[j] + t.qr[j]) / 2).x(), ((t.ql[j] + t.qr[j]) / 2).y());
            } else {
                lastmidPointsPath.lineTo(((t.ql[j] + t.qr[j]) / 2).x(), ((t.ql[j] + t.qr[j]) / 2).y());
            }
        }

        //findSymmetricalMedialAxis(pathList[i], t.ql, t.qr);

        qDebug () << t.ql;

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

        allShapesSampledPoints.push_back(t);

        //RotationalBlendingSurface(i, paths[i], t.ql, t.qr);

    }

    update();

}
//--------------- MOVED TO INPUTSKETCH1

void InputSketch2::findSymmetricalMedialAxis (QPainterPath contour, QVector<QVector3D> &temp_ql, QVector<QVector3D> &temp_qr){


    QVector<QVector3D> mostSimilarql, mostSimilarqr ;//aka Symmetric

    double area_ql = 0, area_qr = 0, actualdifference = 0, lastdifference = std::numeric_limits<double>::max() ;


    foreach (QVector3D a, temp_ql) {

        area_ql = 0;
        area_qr = 0;

        for (int i = 0;  i < temp_ql.size()-1 ; i++) {

            area_ql += (temp_ql[i+1].x()-temp_ql[i].x())*(temp_ql[i].y()+temp_ql[i+1].y());

            area_qr += (temp_qr[i+1].x()-temp_qr[i].x())*(temp_qr[i].y()+temp_qr[i+1].y());

        }

        actualdifference = qFabs(qFabs(area_ql) - qFabs(area_qr));

        if ( actualdifference < lastdifference ){

            lastdifference = actualdifference;

            mostSimilarql.clear();
            mostSimilarqr.clear();

            mostSimilarql.append(temp_ql);
            mostSimilarqr.append(temp_qr);

        }

        temp_ql.push_front(temp_qr.front());
        temp_qr.pop_front();
        temp_qr.push_back(temp_ql.back());
        temp_ql.pop_back();

    }

    temp_ql.clear();
    temp_qr.clear();
    temp_ql.append(mostSimilarql);
    temp_qr.append(mostSimilarqr);


}


void InputSketch2::samplePointsForRotationalBlendingSurface(QPainterPath &selectedCurve){
    //USED IN SMOOTHING AND OVERSKETCHING

    QVector<QPointF> sampledPointsOnCurve;

    allSampledPoints t;

    for (qreal j = 0; j < selectedCurve.length(); j = j + contourSamplingStep) {

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

    allShapesSampledPoints[selectedPathIndex] = allSampledPoints();

    allShapesSampledPoints[selectedPathIndex] = t;

    //RotationalBlendingSurface(selectedPathIndex, selectedCurve, t.ql, t.qr);


}

void InputSketch2::selectPath(const int pathIndex){

    curve = QPainterPath();
    curve = paths[pathIndex];

    selectedPathIndex = pathIndex;

    update () ;
}

void InputSketch2::defRotAxis(int direction){


    //Usar Bola do mouse para fazer rotacao ! Por enquanto...

    if (direction == 1){
        allShapesSampledPoints[selectedPathIndex].qr.push_front(allShapesSampledPoints[selectedPathIndex].ql.first());
        allShapesSampledPoints[selectedPathIndex].ql.pop_front();

        allShapesSampledPoints[selectedPathIndex].ql.push_back(allShapesSampledPoints[selectedPathIndex].qr.last());
        allShapesSampledPoints[selectedPathIndex].qr.pop_back();


    } else {
        allShapesSampledPoints[selectedPathIndex].ql.push_front(allShapesSampledPoints[selectedPathIndex].qr.first());
        allShapesSampledPoints[selectedPathIndex].qr.pop_front();

        allShapesSampledPoints[selectedPathIndex].qr.push_back(allShapesSampledPoints[selectedPathIndex].ql.last());
        allShapesSampledPoints[selectedPathIndex].ql.pop_back();
    }


    allShapesSampledPoints[selectedPathIndex].midPoints.clear();
    allShapesSampledPoints[selectedPathIndex].midPointsPath = QPainterPath();

    QPainterPath midPointsPath;

    for (int i = 0; i < allShapesSampledPoints[selectedPathIndex].ql.size() ; ++i) {

        QVector3D temp_ql(allShapesSampledPoints[selectedPathIndex].ql[i].x(),allShapesSampledPoints[selectedPathIndex].ql[i].y(), allShapesSampledPoints[selectedPathIndex].ql[i].z() );

        QVector3D temp_qr(allShapesSampledPoints[selectedPathIndex].qr[i].x(),allShapesSampledPoints[selectedPathIndex].qr[i].y(), allShapesSampledPoints[selectedPathIndex].qr[i].z() );


        allShapesSampledPoints[selectedPathIndex].midPoints.push_back((temp_ql + temp_qr) / 2);

        if (i == 0){
            midPointsPath.moveTo(allShapesSampledPoints[selectedPathIndex].midPoints[0].x(), allShapesSampledPoints[selectedPathIndex].midPoints[0].y());
        } else {
            midPointsPath.lineTo(allShapesSampledPoints[selectedPathIndex].midPoints[i].x(), allShapesSampledPoints[selectedPathIndex].midPoints[i].y());
        }

    }


    allShapesSampledPoints[selectedPathIndex].midPointsPath = midPointsPath;

    //RotationalBlendingSurface(selectedPathIndex, paths[selectedPathIndex] ,allShapesSampledPoints[selectedPathIndex].ql, allShapesSampledPoints[selectedPathIndex].qr);


    update();
}



void InputSketch2::defCrossSecToEdit(int direction){



}


//--------------- MOVED TO INPUTSKETCH1
void InputSketch2::estimateShape(){


    lineLeveldiff = 0;

    lineLeveldiff = QInputDialog::getInt(nullptr, "Depth Difference", "Layering difference in depth");
    lineLeveldiff*=-1;





    for (int i = 0; i < allShapesSampledPoints.size(); ++i) {

        int reconstType = 0;

        while (reconstType < 1 || reconstType > 2){
            reconstType = QInputDialog::getInt(nullptr, "Select Reconstruction Type for " + names[i], "Select Reconstruction Type for: " + names[i] + " \n 1 - Rotational Blending Surface \n 2 - Hermitian Radial Basis Function");
        }
        if (reconstType == 1) {
            RotationalBlendingSurface(i, paths[i], allShapesSampledPoints[i].ql,allShapesSampledPoints[i].qr);

        } else {
            DataForHRBF (i, paths[i], allShapesSampledPoints[i].ql,allShapesSampledPoints[i].qr);
        }
    }

    update();

}

//--------------- MOVED TO INPUTSKETCH1

// ----MOVED TO INPUTSKETCH1

void InputSketch2::DataForHRBF(const int shapeNumber, QPainterPath &contour, QVector<QVector3D>& ql, QVector<QVector3D>& qr){

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
        p.setZ(p.z() + (lineLeveldiff * ql[0].z()));
        fOut << p.x() <<" " << p.y()<< " " << p.z()<< std::endl;
        pointsView.append(p);
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
        p.setZ(p.z() + (lineLeveldiff * ql[0].z()));
        fOut << p.x() <<" " << p.y()<< " " << p.z()<< std::endl;
        pointsView.append(p);
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

// ----MOVED TO INPUTSKETCH1
void InputSketch2::RotationalBlendingSurface(const int shapeNumber, QPainterPath &contour, QVector<QVector3D>& ql, QVector<QVector3D>& qr){

    //Normal Plane

    for (int i = 0; i < ql.size(); ++i) {
        ql[i].setZ(ql[i].z()*lineLeveldiff);
        qr[i].setZ(qr[i].z()*lineLeveldiff);
    }

    QVector3D n;
    QVector3D pl, pr;
    u = 0;

    allShapesSampledPoints[shapeNumber].shapePoints.clear();

    QVector3D bottomLeft (contour.boundingRect().bottomLeft().x(), contour.boundingRect().bottomLeft().y() , ql[0].z());

    QVector3D topLeft (contour.boundingRect().topLeft().x(), contour.boundingRect().topLeft().y() , ql[0].z());
    QVector3D bottomRight (contour.boundingRect().bottomRight().x(), contour.boundingRect().bottomRight().y() , ql[0].z());

    n = QVector3D::crossProduct(topLeft - bottomLeft , bottomRight - bottomLeft);

    int nvertex = 0;

    QVector3D center = (ql[ql.size()/2] + qr[qr.size()/2]) / 2;

    while (u < ql.size()) {

        v = 0;

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

            pointsView.push_back(p);
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
// ----MOVED TO INPUTSKETCH1

void InputSketch2::CrossSectionalBlendingSurface(QPainterPath &tu, QVector<QVector3D> &ql, QVector<QVector3D> &qr){

    float u = 0, v = 0, u_step = 1, v_step = 0.1f;

    while (u < ql.size()) {

        v = 0;

        //ttransformed = AlignToAxis
        //Transform
        //Rotate
        //Scale


        while (v <= 1) {
            //p = transformed(v)


        }

    }

}

void InputSketch2::showNormals(){

    if (showNormalsToggle == true){
        showNormalsToggle = false;
    } else {
        showNormalsToggle = true;
    }
    update();

}

void InputSketch2::exportMesh(){

    if (pointsView.isEmpty()){


        QMessageBox msgBox;
        msgBox.setText("No Normals and Points to export");
        msgBox.setInformativeText("Please Estimate Normals and Points before export");
        msgBox.exec();

        return;
    }
    // Create Topology


    QVector<int> quadTopology (4);


    for (int i = 0; i < allShapesSampledPoints.size(); i++) {

        topology.clear();
        int npoints = 0;


        for (int j = 0; j < allShapesSampledPoints[i].shapePoints.size()-1 ; ++j) {

            //allShapesSampledPoints[i].shapePoints[j].push_back(allShapesSampledPoints[i].shapePoints[j].at(0));

            npoints = allShapesSampledPoints[i].shapePoints[j].size() * allShapesSampledPoints[i].shapePoints.size();

            for (int k = 0; k < allShapesSampledPoints[i].shapePoints[j].size() -1 ; ++k) {

                quadTopology[0] = (allShapesSampledPoints[i].shapePoints[j].size() * j) + k;
                quadTopology[1] = (allShapesSampledPoints[i].shapePoints[j].size() * j) + k + 1;
                quadTopology[2] = (allShapesSampledPoints[i].shapePoints[j].size() * (j + 1)) + k + 1;
                quadTopology[3] = (allShapesSampledPoints[i].shapePoints[j].size() * (j + 1)) + k;

                topology.push_back(quadTopology);

            }

            quadTopology[0] = allShapesSampledPoints[i].shapePoints[j].last().vertexNumber;
            quadTopology[1] = allShapesSampledPoints[i].shapePoints[j].first().vertexNumber;
            quadTopology[2] = allShapesSampledPoints[i].shapePoints[j+1].first().vertexNumber;
            quadTopology[3] = allShapesSampledPoints[i].shapePoints[j+1].last().vertexNumber;
            topology.push_back(quadTopology);

        }



        std::string outFile2 = names[i].toUtf8().constData();
        outFile2.append("_mesh.off");
        std::ofstream fOut;
        fOut.open(outFile2.c_str());


        fOut << "OFF" <<std::endl;

        //Vertices, Faces, Edges

        fOut << npoints  <<" " << topology.size()  <<" " << "0" <<std::endl;
        for (int l = 0; l < allShapesSampledPoints[i].shapePoints.size(); ++l) {
            for (int m = 0; m < allShapesSampledPoints[i].shapePoints[l].size(); ++m) {

                fOut << allShapesSampledPoints[i].shapePoints[l][m].point3D.x() <<" " << allShapesSampledPoints[i].shapePoints[l][m].point3D.y() << " " << allShapesSampledPoints[i].shapePoints[l][m].point3D.z() << std::endl;

            }
        }

        for (int m = 0; m < topology.size(); ++m) {
            fOut << 4 << " "<< topology[m][0] <<" "<< topology[m][1] <<" "<< topology[m][2] <<" "<< topology[m][3] << std::endl;
        }
        fOut.close();

    }







    //    for (int i = 0 ; i < exportPoints.size() ; i++) {

    //        //exportNpts(exportPoints[i], exportNormals[i], names[i]);
    //        exportOffModels(exportPoints[i], exportNormals[i], names[i]);
    //        //exportPlyModels(exportPoints[i], exportNormals[i], names[i]);

    //    }
}

bool InputSketch2::resizeMesh(QVector<QVector3D> &totalPoints){

    QVector4D *vertices;
    vertices = new QVector4D [ totalPoints.size() ];

    if ( totalPoints.size() > 0) {

        float minLim = std::numeric_limits < float >::min();
        float maxLim = std::numeric_limits < float >::max();
        QVector4D max(minLim ,minLim ,minLim ,1.0);
        QVector4D min(maxLim ,maxLim ,maxLim ,1.0);

        for ( int i = 0; i < totalPoints.size(); i ++) {
            float x = totalPoints[i].x(), y = totalPoints[i].y(), z = totalPoints[i].z();

            max.setX (qMax < float >( max.x(),x));
            max.setY (qMax < float >( max.y(),y));
            max.setZ (qMax < float >( max.z(),z));
            min.setX (qMin < float >( min.x(),x));
            min.setY (qMin < float >( min.y(),y));
            min.setZ (qMin < float >( min.z(),z));
            vertices [i] = QVector4D (x, y, z, 1.0);
        }

        QVector4D midpoint = (min + max) *0.5;
        float invdiag = 2.f / static_cast<float>((max - min).length());
        for ( int i = 0; i < totalPoints.size(); i ++) {
            vertices[i] = (vertices [i] - midpoint) *invdiag;
            vertices[i].setW(1);
            totalPoints[i].setX(vertices[i].x());
            totalPoints[i].setY(vertices[i].y());
            totalPoints[i].setZ(vertices[i].z());

        }
    }

    return true;
}

void InputSketch2::exportNpts(QVector<QVector3D> totalPoints, const QVector<QVector3D> totalNormals, const QString pathName){

    resizeMesh(totalPoints);

    //    std::string outFile = "shape";
    //    outFile.append("_");
    //    outFile.append(std::to_string(shapeNumber));
    //    outFile.append(".bnpts");

    std::string outFile = pathName.toUtf8().constData();

    outFile.append(".bnpts");

    std::ofstream fOut;
    fOut.open(outFile.c_str());

    //    fOut << "3" <<std::endl;
    //    fOut << totalPoints.size() <<std::endl;
    //    fOut << totalNormals.size() << std::endl;

    for(int i = 0; i < totalPoints.size(); i++){

        QVector3D p = totalPoints.at(i);
        QVector3D n = totalNormals.at(i);

        fOut << p.x() <<" " << p.y()<< " " << p.z() << " " << " " << n.x() <<" " << n.y()<< " " << n.z()<< std::endl;

    }
    //fOut << "0" << std::endl;
    fOut.close();

    //shapeNumber++;

}
void InputSketch2::exportOffModels(QVector<QVector3D> totalPoints, const QVector<QVector3D> totalNormals, const QString pathName){

    std::string outFile2 = pathName.toUtf8().constData();
    outFile2.append("_points.off");
    std::ofstream fOut;
    fOut.open(outFile2.c_str());


    fOut << "OFF" <<std::endl;
    fOut << totalPoints.size()*3 <<" " << totalPoints.size() <<" " << "0" <<std::endl;
    for(QVector3D p:totalPoints)
    {
        fOut << p.x() <<" " << p.y()<< " " << p.z()<< std::endl;
        fOut << p.x() <<" " << p.y()<< " " << p.z()<< std::endl;
        fOut << p.x() <<" " << p.y()<< " " << p.z()<< std::endl;
    }
    int i=0;
    for(QVector3D p:totalPoints)
    {
        fOut << "3" <<" " << i <<" " << i+1 << " " << i+2 << std::endl;
        i+=3;
    }

    fOut.close();

    std::string outFile3 = pathName.toUtf8().constData();
    outFile3.append("_normals.off");
    fOut.open(outFile3.c_str());
    fOut << "OFF" <<std::endl;
    fOut << totalNormals.size()*3 <<" " << totalNormals.size() <<" " << "0" <<std::endl;
    for(int i = 0; i < totalNormals.size(); i++)
    {

        QVector3D p = totalPoints.at(i);
        QVector3D n = totalNormals.at(i);

        fOut << p.x()+n.x() <<" " << p.y()+n.y()<< " " << p.z()+n.z()<< std::endl;
        fOut << p.x()+n.x() <<" " << p.y()+n.y()<< " " << p.z()+n.z()<< std::endl;
        fOut << p.x()+n.x() <<" " << p.y()+n.y()<< " " << p.z()+n.z()<< std::endl;
    }
    i=0;
    for(QVector3D n:totalNormals)
    {
        fOut << "3" <<" " << i <<" " << i+1 << " " << i+2 << std::endl;
        i+=3;
    }

    fOut.close();


}

void InputSketch2::exportPlyModels(QVector<QVector3D> totalPoints, const QVector<QVector3D> totalNormals, const QString pathName){

    resizeMesh(totalPoints);

    std::string outFile3 = pathName.toUtf8().constData();
    outFile3.append(".ply");
    std::ofstream fOut;
    fOut.open(outFile3.c_str());


    fOut << "ply" <<std::endl;
    fOut << "format ascii 1.0" << std::endl;
    fOut << "element vertex " << totalPoints.size()-1 <<  std::endl;

    fOut << "property float x" << std::endl;
    fOut << "property float y" << std::endl;
    fOut << "property float z" << std::endl;
    fOut << "property float nx" << std::endl;
    fOut << "property float ny" << std::endl;
    fOut << "property float nz" << std::endl;

    fOut << "element face 0" << std::endl;

    fOut << "property list uchar uint vertex" << std::endl;

    fOut << "end_header" << std::endl;

    for(int i = 0; i < totalPoints.size(); i++){

        QVector3D p = totalPoints.at(i);
        QVector3D n = totalNormals.at(i);

        fOut << p.x() <<" " << p.y()<< " " << p.z() << " " << n.x() <<" " << n.y()<< " " << n.z()<< std::endl;

    }

    fOut.close();

}


void InputSketch2::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget ) {

    Q_UNUSED(option);
    Q_UNUSED(widget);

    //QList< QPolygonF > subpaths =  curve.toSubpathPolygons();

    QFont font = painter->font() ;

    font.setPointSize ( 11 );

    painter->setFont(font);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(Qt::black, 1,Qt::SolidLine));

    painter->drawText(2, 15, interactionMode );

    if (paths.size() < 1) return;


    font.setPointSize ( 8 );

    painter->setFont(font);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(Qt::black, 1,Qt::SolidLine));


    for (int i = 0 ; i < paths.size(); i++) {


        QString s = QString::number(i);
        QPointF posText (paths[i].elementAt(0).x + 10, paths[i].elementAt(0).y);

        painter->drawText(posText, names[i] );

        painter->drawPath(paths[i]);

    }
    painter->setPen(QPen(Qt::darkRed, 2,Qt::SolidLine));

    if (totalPoints.isEmpty()){
        for (int i = 0; i < allShapesSampledPoints.size(); ++i) {
            painter->drawPath(allShapesSampledPoints[i].midPointsPath);
        }
    }

    //    painter->setPen(QPen(Qt::red, 2,Qt::SolidLine));
    //    painter->drawPath(lastmidPointsPath);

    if (!curve.isEmpty()){
        painter->setPen(QPen(Qt::blue, 2,Qt::SolidLine));
        painter->drawPath(curve);

    }


    if (!oversketchingCurve.isEmpty()){
        painter->setPen(QPen(Qt::red, 2,Qt::SolidLine));
        painter->drawPath(oversketchingCurve);
    }


    if (!pointsView.isEmpty() && showNormalsToggle == true) {

        int r,g,b;

        for (int i = 0; i < pointsView.size(); i++) {

            normalsView.push_back(pointsView[i].normalized());

            r = ((normalsView[i].x()+1)/2)*255;
            g = ((normalsView[i].y()+1)/2)*255;
            b = ((normalsView[i].y()+1)/2)*255;

            //Implement Grayscale

            int gray = (qRed(r) + qGreen(g) + qBlue(b))/3;

            painter->setPen(QPen(QColor(gray,gray,gray, 150),1,Qt::SolidLine));

            //painter->drawLine(pointsView[i].x(), pointsView[i].y(), pointsView[i].x(), pointsView[i].y());
        }

    }

    for (int i = 0; i < totalPoints.size(); ++i) {
        int r,g,b;
        r = ((totalNormals[i].x()+1)/2)*255;
        g = ((totalNormals[i].y()+1)/2)*255;
        b = totalNormals[i].z()*255;
        painter->setOpacity(1);
        painter->setPen(QPen(QColor(r,g,b),1,Qt::SolidLine));
        painter->drawLine(totalPoints[i].x(), totalPoints[i].y(), totalPoints[i].x(), totalPoints[i].y());

    }

    painter->setPen(QPen(QColor(Qt::black),2,Qt::SolidLine));

    foreach (QLineF Line, normalsContour) {

        painter->drawLine(Line);
    }

}

QVector<QPainterPath> InputSketch2::getPaths(){

    return paths;
}

QPainterPath InputSketch2::getCurve(){

    return curve;
}

QVector<QVector3D> InputSketch2::getPoints3D(){

    return pointsView;

}

QVector<QVector3D> InputSketch2::getNormals3D(){

    return normalsFor3Ddisks;
}


double InputSketch2::cosine_similarity_vectors(QVector<double>A, QVector<double>B)
{
    double mul = 0.0;
    double d_a = 0.0;
    double d_b = 0.0 ;

    if (A.size() != B.size())
    {
        throw std::logic_error("Vector A and Vector B are not the same size");
    }

    // Prevent Division by zero
    if (A.size() < 1)
    {
        throw std::logic_error("Vector A and Vector B are empty");
    }

    QVector<double>::iterator B_iter = B.begin();
    QVector<double>::iterator A_iter = A.begin();
    for( ; A_iter != A.end(); A_iter++ , B_iter++ )
    {
        mul += *A_iter * *B_iter;
        d_a += *A_iter * *A_iter;
        d_b += *B_iter * *B_iter;
    }

    if (d_a == 0.0f || d_b == 0.0f)
    {
        throw std::logic_error(
                    "cosine similarity is not defined whenever one or both "
                    "input vectors are zero-vectors.");
    }

    return mul / (sqrt(d_a) * sqrt(d_b));
}

float InputSketch2::getPathArea(QPainterPath p, float step)
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



