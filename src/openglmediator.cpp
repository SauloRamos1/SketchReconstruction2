#include <tucanow/sphere.hpp>

#include "openglmediator.h"
#include "openglcanvas.h"


#include "include/openglcanvas.h"
#include "include/canvas.h"
#include "include/canvas2.h"

#include <QMessageBox>
#include <fstream>

/// This is the MODEL for MVC Standard



OpenGLMediator::OpenGLMediator()
{


}

void OpenGLMediator::setCanvas(const std::shared_ptr<Canvas> &cv)
{
    canvas = cv;
}

void OpenGLMediator::setGlCanvas(const std::shared_ptr< OpenGLCanvas >& gl)
{
    glcanvas = gl;
}

void OpenGLMediator::exportView(){

    if (vertices.size() == 0 ) {

        QMessageBox msgBox;
        msgBox.setText("No mesh to export.");
        msgBox.setInformativeText("No mesh to export.");
        msgBox.exec();
        return;
        
    }
    //Export OFF
    std::string outFile = "mesh.off";
    //outFile.append("mesh.off");
    std::ofstream fOut;
    fOut.open(outFile.c_str());


    fOut << "OFF" <<std::endl;

    //Vertices, Faces, Edges

    fOut << vertices.size()/3  <<" " << faces.size()/3 <<" " << "0" <<std::endl;
    for (int i = 0 ; i < vertices.size()/3; i++){
        fOut << vertices[3*i+0] << " " << vertices[3*i + 1] << " " << vertices[3*i+2] << std::endl;

    }

    for (int m = 0; m < faces.size()/3; m++) {
        fOut << 3 << " "<< faces[3*m+0] <<" "<< faces[3*m+1]  <<" "<< faces[3*m+2]  << std::endl;
    }
    fOut.close();

    qDebug () << "Exported OFF Mesh";

    //Export PLY

    std::string outFile3 = "stripe_mesh";
    std::ofstream fOut1;
    outFile3.append(".ply");

    fOut1.open(outFile3.c_str());


    fOut1 << "ply" <<std::endl;
    fOut1 << "format ascii 1.0" << std::endl;
    fOut1 << "element vertex " << vertices.size()/3 <<  std::endl;

    fOut1 << "property float x" << std::endl;
    fOut1 << "property float y" << std::endl;
    fOut1 << "property float z" << std::endl;

    fOut1 << "element face " << faces.size()/3 << std::endl;

    fOut1 << "property list uint8 int32 vertex_indices" << std::endl;

    fOut1 << "end_header" << std::endl;

    for (int i = 0 ; i < vertices.size()/3; i++){
        fOut1 << vertices[3*i+0] << " " << vertices[3*i + 1] << " " << vertices[3*i+2] << std::endl;
    }

    for (int m = 0; m < faces.size()/3; m++) {
        fOut1 << 3 << " "<< faces[3*m+0] <<" "<< faces[3*m+1]  <<" "<< faces[3*m+2]  << std::endl;
    }

    fOut1.close();

    qDebug () << "Exported PLY Mesh";
    //    pathsList.clear();
    //    svgPaths.clear();

    //    update();

}



QVector<QVector3D> OpenGLMediator::createCylinder(int steps, QVector3D base, QVector3D top){

    ///CODE FROM https://github.com/curran/renderCyliner/blob/master/renderCylinder.c
    /// http://lifeofaprogrammergeek.blogspot.com/2008/07/rendering-cylinder-between-two-points.html

    QVector<QVector3D> diskVertex;

    int    i, j;
    float  phi, dphi = 2.*M_PI / (float)(steps);

    float R1 = 1.0f, R2 = 1.0f;
    float angle;
    int H = 1;

    float vx = top.x()-base.x();
    float vy = top.y()-base.y();
    float vz = top.z()-base.z();

    float v = sqrt( vx*vx + vy*vy + vz*vz );
    float ax;

    if (fabs(vz) < 1.0e-3) {
        ax = 57.2957795*acos( vx/v ); // rotation angle in x-y plane
        if ( vy <= 0.0 )
            ax = -ax;
    }
    else {
        ax = 57.2957795*acos( vz/v ); // rotation angle
        if ( vz <= 0.0 )
            ax = -ax;
    }

    float rx = -vy*vz;
    float ry = vx*vz;


    for(phi = 0.; phi <= 2.*M_PI; phi += dphi) {

        QVector3D b(R1*cos(phi), R1*sin(phi), 0);
        // QVector3D c(R2*cos(phi), R2*sin(phi), 0);

        QMatrix4x4 m1;
        m1.setToIdentity();
        m1.translate(base);

        if (fabs(vz) < 1.0e-3) {
            m1.rotate(90.0, 0, 1, 0.0); // Rotate & align with x axis
            m1.rotate(ax, -1.0, 0.0, 0.0); // Rotate to point 2 in x-y plane
        }
        else {
            m1.rotate(ax, rx, ry, 0.0); // Rotate about rotation vector
        }

        b = m1 * b;


        diskVertex.push_back(b);

    }


    return diskVertex;
}

QVector3D OpenGLMediator::CalculateSurfaceNormal(QVector3D p0, QVector3D p1, QVector3D p2){


    QVector3D normal = QVector3D::crossProduct(p1 - p0, p2 - p0);

    //    QVector3D p3, p4, p5; //sphere.hpp

    //    p3 = (p0 + p1)*0.5;
    //    p4 = (p0 + p2)*0.5;
    //    p5 = (p1 + p2)*0.5;

    //    p3.normalize();
    //    p4.normalize();
    //    p5.normalize();

    //    QVector<QVector3D> normals;
    //    normals.push_back(p3);
    //    normals.push_back(p4);
    //    normals.push_back(p5);
    normal.normalize();

    return normal;
}

void OpenGLMediator::clearTriangles(){

    vertices = std::vector< float >();
    faces = std::vector< unsigned int > ();
    normals = std::vector< float >();

    nvertices = 0;
}

//CREATE TRIANGLES, FACES AND NORMALS FOR RENDERING
void OpenGLMediator::viewOpenContours3D (const QList<QVector<QVector3D> > points3D){


    qDebug () << points3D.size();

    if( points3D.isEmpty() ) {
        QMessageBox msgBox;
        msgBox.setText("No OPEN contours to reconstruct");
        msgBox.setInformativeText("Please load contours before the reconstruction");
        msgBox.exec();

        return;
    }


    int steps = 8;


    for (int h = 0 ; h < points3D.size(); h++){

        QVector<QVector<QVector3D> > vertexMatrix;

        for (int i = 0; i < points3D[h].size()-1; i++){

            QVector<QVector3D> diskVertices = createCylinder(steps, points3D[h][i], points3D[h][i+1]);

            vertexMatrix.append(diskVertices);
            for (int j = 0; j < diskVertices.size(); j++) {

                QVector3D normal = diskVertices[j] - points3D[h][i];
                normal.normalize();
                normals.push_back(normal.x());
                normals.push_back(normal.y());
                normals.push_back(normal.z());

                vertices.push_back(diskVertices[j].x());
                vertices.push_back(diskVertices[j].y());
                vertices.push_back(diskVertices[j].z());

            }
        }


        QVector<QVector<int> > topology;

        QVector<int> quadTopology (4);

        QVector<int> lastQuad (4);

        //int nfaces = faces.size();

        for (int i = 0; i < vertexMatrix.size() - 1; ++i) {

            lastQuad[1] = vertexMatrix[i].size() * i;
            lastQuad[2] = vertexMatrix[i].size() * ( i+1 );

            for (int j = 0; j < vertexMatrix[i].size() - 1; ++j){

                quadTopology[0] = (vertexMatrix[i].size() * i) + j;
                quadTopology[1] = (vertexMatrix[i].size() * i) + j + 1;
                quadTopology[2] = (vertexMatrix[i].size() * (i + 1)) + j + 1;
                quadTopology[3] = (vertexMatrix[i].size() * (i + 1)) + j;

                lastQuad[0] =  (vertexMatrix[i].size() * i) + j + 1;
                lastQuad[3] = (vertexMatrix[i].size() * (i + 1)) + j + 1;

                faces.push_back(quadTopology[0]+nvertices);
                faces.push_back(quadTopology[1]+nvertices);
                faces.push_back(quadTopology[2]+nvertices);
                faces.push_back(quadTopology[0]+nvertices);
                faces.push_back(quadTopology[2]+nvertices);
                faces.push_back(quadTopology[3]+nvertices);

            }

            //        quadTopology[0] = vertexMatrix[i].last().vertexNumber;
            //        quadTopology[1] = vertexMatrix[i].first().vertexNumber;
            //        quadTopology[2] = vertexMatrix[i+1].first().vertexNumber;
            //        quadTopology[3] = vertexMatrix[i+1].last().vertexNumber;

            faces.push_back(lastQuad[0]+nvertices);
            faces.push_back(lastQuad[1]+nvertices);
            faces.push_back(lastQuad[2]+nvertices);
            faces.push_back(lastQuad[0]+nvertices);
            faces.push_back(lastQuad[2]+nvertices);
            faces.push_back(lastQuad[3]+nvertices);




        }
        nvertices = vertices.size()/3;

    }


    // render () ;
    //glcanvas->createTube(vertices, faces, normals);


    //exportMesh();


    //   render () ;



}

void OpenGLMediator::viewClosedContours3D (const QVector<QVector3D> points3D, const QVector<QVector3D> normals3D){

    if( points3D.isEmpty() ) {
        QMessageBox msgBox;
        msgBox.setText("No CLOSED contours to reconstruct");
        msgBox.setInformativeText("Please load contours before the reconstruction");
        msgBox.exec();

        return;
    }
    //    std::vector< float > vertices, normals;
    //    std::vector< unsigned int > faces;

    //    CCvertices.clear();
    //    CCnormals.clear();
    //    CCfaces.clear();

    int steps = 16;
    int centerVertexNumber = 0;

    for (int i = 0; i < points3D.size(); i++){

        QVector<QVector3D> diskVertices = createCylinder(steps, points3D[i], points3D[i] + normals3D[i]);

        vertices.push_back(points3D[i].x());
        vertices.push_back(points3D[i].y());
        vertices.push_back(points3D[i].z());

        QVector3D normal = normals3D[i];

        normal.normalize();
        normals.push_back(normal.x());
        normals.push_back(normal.y());
        normals.push_back(normal.z());

        //  qDebug () << points3D[i];

        for (int j = 0; j < diskVertices.size(); j++) {

            QVector3D normal = normals3D[i];

            normal.normalize();
            normals.push_back(normal.x());
            normals.push_back(normal.y());
            normals.push_back(normal.z());

            //nvertices ++;

            vertices.push_back(diskVertices[j].x());
            vertices.push_back(diskVertices[j].y());
            vertices.push_back(diskVertices[j].z());

        }


        for (int k = 1; k < diskVertices.size(); k++) {

            faces.push_back(centerVertexNumber+nvertices);
            faces.push_back(centerVertexNumber+k+nvertices);
            faces.push_back(centerVertexNumber+k+1+nvertices);


        }


        faces.push_back(centerVertexNumber+nvertices);
        faces.push_back(centerVertexNumber+diskVertices.size()+nvertices);
        faces.push_back(centerVertexNumber+1+nvertices);



        centerVertexNumber += diskVertices.size()+1;


    }


    nvertices = vertices.size()/3;


    //setShape(points3D);

    //getMeshPath (vertices, faces, normals);

    //    qDebug () << "Vertices: " << vertices.size()/3;
    //    qDebug () << "Faces: " << faces.size();
    //    qDebug () << "Normais: " << normals.size()/3;

    // render ();
    //glcanvas->createTube(vertices, faces, normals);

    //    vertices.clear();
    //    faces.clear();
    //    normals.clear();

    //glcanvas->renderCylinder(points3D);
}

void OpenGLMediator::viewStripes3D (const QVector<QVector3D> points3D){

    if( points3D.isEmpty() ) {
        QMessageBox msgBox;
        msgBox.setText("No STRIPE contours to reconstruct");
        msgBox.setInformativeText("Please load contours before the reconstruction");
        msgBox.exec();

        return;
    }
    //    std::vector< float > vertices, normals;
    //    std::vector< unsigned int > faces;

    //    Svertices.clear();
    //    Snormals.clear();
    //    Sfaces.clear();

    QPolygonF quad;
    QVector<QVector3D> pointsfor3Dleft, pointsfor3Dright;

    //vector1.insert( vector1.end(), vector2.begin(), vector2.end() );
    //TODO

    for (int i = 0 ; i < points3D.size()/2; i++) {
        pointsfor3Dleft.push_back(points3D[i]);
    }

    for (int i = points3D.size()/2 ; i < points3D.size(); i++) {
        pointsfor3Dright.push_back(points3D[i]);
    }

    QVector<QVector3D> quadMesh;
    int facesNumber = 0;
    QVector<int> quadTopology (4);

    for (int i = 0 ; i < pointsfor3Dleft.size()-1; i++) {

        QVector3D p(pointsfor3Dleft[i].x(),pointsfor3Dleft[i].y(), pointsfor3Dleft[i].z());
        QVector3D q(pointsfor3Dleft[i+1].x(),pointsfor3Dleft[i+1].y(), pointsfor3Dleft[i+1].z());
        QVector3D r(pointsfor3Dright[i+1].x(),pointsfor3Dright[i+1].y(), pointsfor3Dright[i+1].z());
        QVector3D s(pointsfor3Dright[i].x(),pointsfor3Dright[i].y(), pointsfor3Dright[i].z());

        //https://stackoverflow.com/questions/9806630/calculating-the-vertex-normals-of-a-quad

        QVector3D normal = CalculateSurfaceNormal(p,q,r); //Normal for 1st triangle
        QVector3D normal1 = CalculateSurfaceNormal(r,p,s);//Normal for 2nd triangle

        vertices.push_back(p.x());
        vertices.push_back(p.y());
        vertices.push_back(p.z());

        normals.push_back(normal.x());
        normals.push_back(normal.y());
        normals.push_back(normal.z());

        vertices.push_back(q.x());
        vertices.push_back(q.y());
        vertices.push_back(q.z());

        normals.push_back(normal.x());
        normals.push_back(normal.y());
        normals.push_back(normal.z());

        vertices.push_back(r.x());
        vertices.push_back(r.y());
        vertices.push_back(r.z());

        normals.push_back(normal.x());
        normals.push_back(normal.y());
        normals.push_back(normal.z());

        vertices.push_back(s.x());
        vertices.push_back(s.y());
        vertices.push_back(s.z());

        normals.push_back(normal.x());
        normals.push_back(normal.y());
        normals.push_back(normal.z());

        facesNumber += 4;

        quadTopology[0] = facesNumber - 4;
        quadTopology[1] = facesNumber - 3;
        quadTopology[2] = facesNumber - 2;
        quadTopology[3] = facesNumber - 1;

        faces.push_back(quadTopology[0]+nvertices);
        faces.push_back(quadTopology[1]+nvertices);
        faces.push_back(quadTopology[2]+nvertices);
        faces.push_back(quadTopology[0]+nvertices);
        faces.push_back(quadTopology[2]+nvertices);
        faces.push_back(quadTopology[3]+nvertices);

    }


    nvertices = vertices.size()/3;

    //    setShape(points3D);

    //    getMeshPath (vertices, faces, normals);

    // render();
    //glcanvas->createTube(vertices, faces, normals);

    //    vertices.clear();
    //    faces.clear();
    //    normals.clear();

    //glcanvas->renderCylinder(points3D);
}



//void OpenGLMediator::viewSketch3D(const QVector<QVector3D> points3D, const QVector<QString> pathNames){

//    if( points3D.isEmpty() ) {
//        QMessageBox msgBox;
//        msgBox.setText("No contours to reconstruct");
//        msgBox.setInformativeText("Please load contours before the reconstruction");
//        msgBox.exec();

//        return;
//    }


//    std::vector< float > vertices, normals;
//    std::vector< unsigned int > faces;

//    setShape(points3D);

//    getMeshPath (vertices, faces, normals);

//    glcanvas->createTube(vertices, faces, normals);

//    vertices.clear();
//    faces.clear();
//    normals.clear();

//    qDebug () << "Finished 2D View";

//    qDebug () << "Finished 3D View";

//}

//void OpenGLMediator::viewOverlapping3D (const QVector<QVector3D> points3D){

//    if( points3D.isEmpty() ) {
//        QMessageBox msgBox;
//        msgBox.setText("No contours to reconstruct");
//        msgBox.setInformativeText("Please load contours before the reconstruction");
//        msgBox.exec();

//        return;
//    }
//    std::vector< float > vertices, normals;
//    std::vector< unsigned int > faces;

//    exportPovrayPipe(points3D);

//    setShape(points3D);

//    getMeshPath (vertices, faces, normals);

//    glcanvas->createTube(vertices, faces, normals);

//    vertices.clear();
//    faces.clear();
//    normals.clear();

//    qDebug () << "Finished 2D View";

//    qDebug () << "Finished 3D View";

//    //glcanvas->renderCylinder(points3D);
//}

//void OpenGLMediator::viewStripe (){


//    glcanvas->loadGeneratedMesh("stripe_mesh.ply");

//}

bool OpenGLMediator::setShape(const QVector<QVector3D> &totalPoints){

    //std::size_t start = 0;

    curve_coordinates.clear();
    //curve_edges.clear();


    for (int i = 0; i < totalPoints.size(); ++i) {

        curve_coordinates.push_back(totalPoints[i].x());
        curve_coordinates.push_back(totalPoints[i].y());
        curve_coordinates.push_back(totalPoints[i].z());
    }

    return true;
}



bool OpenGLMediator::estimatePolygonNormals(const QPainterPath &path, QVector <QVector3D> &totalPoints, QVector <QVector3D> &totalNormals){


    QVector <QVector3D> knownPoints;
    QVector <QVector3D> knownNormals;

    QList<QPolygonF> pathPolygons = path.toSubpathPolygons();

    QPolygonF pathPolygon;

    foreach (QPolygonF poly, pathPolygons){

        pathPolygon.append(poly);
    }

    QLineF angleLine;
    QPointF point2D;
    QVector3D point3D;

    QVector3D normalVector;
    normalVector.setZ(0);



    //    foreach (QPointF pointOnPath, pathPolygon){



    //    }
    ///TODO FIND POINTS ON PATH TO RETRIVE ANGLE


    // Create normals for contour / stroke

    double contoursamplingvalue = 5.0; // ADJUSTABLE in Pixels
    double percent;

    double pathLength = path.length();

    for (double var = pathLength; var > 0 ; var = var - contoursamplingvalue){

        percent = var / pathLength;
        /* Set the origin: */

        point2D = static_cast<QPointF>( path.pointAtPercent(percent));
        angleLine.setP1(point2D);
        point3D.setX(static_cast<float>(point2D.x()));
        point3D.setY(static_cast<float>(point2D.y()));
        point3D.setZ(0);

        knownPoints.push_back(point3D);
        totalPoints.push_back(point3D);

        /* Set the angle and length: */

        angleLine.setAngle (path.angleAtPercent(percent)-90);
        angleLine.setLength(1);
        normalVector.setX(static_cast<float>(angleLine.x2())-static_cast<float>(angleLine.x1()));
        normalVector.setY(static_cast<float>(angleLine.y2())-static_cast<float>(angleLine.y1()));

        knownNormals.push_back(normalVector.normalized()); //PROBLEM ?? normalize
        totalNormals.push_back(normalVector.normalized());

    }

    //Estimate Normals

    QRectF pathBox = path.boundingRect();
    double xmin = pathBox.left();
    double xmax = pathBox.right();
    double ymin = pathBox.top();
    double ymax = pathBox.bottom();

    double spacing = 2.0; // ADJUSTABLE in Pixels

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

                p.setZ(n.z()*(static_cast<float>(pathBox.width())/5)); //Adjustable
                totalPoints.push_back(p);
                totalNormals.push_back(n);


                p.setZ(-n.z()*(static_cast<float>(pathBox.width())/5)); //Adjustable
                totalPoints.push_back(p);
                n.setZ(n.z()*-1);
                totalNormals.push_back(n);
            }

        }
    }

    return true;

}

QPainterPath OpenGLMediator::resizePath(QPainterPath &path){

    if (!path.isEmpty()){

        double minLim = std::numeric_limits <double>::min();
        double maxLim = std::numeric_limits <double>::max();

        QPointF max (minLim,minLim);
        QPointF min (maxLim,maxLim);

        for ( int i = 0; i < path.elementCount() ; i++){

            max.setX (qMax < double > ( max.x(), path.elementAt(i).x));
            max.setY (qMax < double > ( max.y(), path.elementAt(i).y));

            min.setX (qMin < double >( min.x(), path.elementAt(i).x));
            min.setY (qMin < double >( min.y(), path.elementAt(i).y));
        }

        QPointF midpoint = (min - max) * 0.5;

        QLineF distMaxMin(max,min);

        double invdiag = 2.0 / distMaxMin.length();

        for ( int i = 0; i < path.elementCount() ; i++){

            path.setElementPositionAt(i, (path.elementAt(i).x - midpoint.x()) * invdiag, (path.elementAt(i).y - midpoint.y()) * invdiag);

        }

    }

    qDebug () << path;

    return path;
}

void OpenGLMediator::render()
{
    glcanvas->createTube(vertices, faces, normals);
}

bool OpenGLMediator::getMeshPath(std::vector<float> &vertex_coordinates, std::vector<unsigned int> &triangle_list, std::vector<float> &normal_list){


    for (size_t i = 0; i < curve_coordinates.size()/3; i++) {

        Sphere s;
        s.setCenter(curve_coordinates[3*i + 0], curve_coordinates[3*i + 1], curve_coordinates[3*i + 2]);

        //s.setCenter(totalPoints[i].x(),totalPoints[i].y(),totalPoints[i].z());
        s.setRadius(1.0f); //Adjustable

        std::vector<float> vertices, normals;
        std::vector<unsigned int> faces;


        size_t num_subdivisions = 2;
        s.getMesh(vertices, faces, normals, num_subdivisions);


        size_t num_vertices = vertex_coordinates.size()/3;

        for (auto &e : faces){
            e += num_vertices;
        }

        std::copy(vertices.begin(), vertices.end(), std::back_inserter (vertex_coordinates));
        std::copy(faces.begin(), faces.end(), std::back_inserter(triangle_list));
        std::copy(normals.begin(), normals.end(), std::back_inserter(normal_list));

    }

    return true;
}

bool OpenGLMediator::setCurve(const QPolygonF& pol_){

    //std::size_t start = 0;

    //    curve_coordinates.clear();
    //    curve_edges.clear();

    for (int i = 0 ; i < pol_.size(); ++i){

        QPointF p = pol_[i];

        curve_coordinates.push_back(static_cast<float>(p.x()));
        curve_coordinates.push_back(static_cast<float>(p.y()));

    }

    return true;
}

bool OpenGLMediator::getMesh(std::vector<float> &vertex_coordinates, std::vector<unsigned int> &triangle_list, std::vector<float> &normal_list){

    //    vertex_coordinates.clear();
    //    triangle_list.clear();
    //    normal_list.clear();

    for (size_t i = 0; i < curve_coordinates.size()/2; ++i) {

        Sphere s;
        s.setCenter(curve_coordinates[2*i + 0], curve_coordinates[2*i + 1],0.f);

        s.setRadius(10.0f);

        std::vector<float> vertices, normals;
        std::vector<unsigned int> faces;

        size_t num_subdivisions = 2;
        s.getMesh(vertices, faces, normals, num_subdivisions);


        size_t num_vertices = vertex_coordinates.size()/3;

        for (auto &e : faces){
            e += num_vertices;
        }

        std::copy(vertices.begin(), vertices.end(), std::back_inserter (vertex_coordinates));
        std::copy(faces.begin(), faces.end(), std::back_inserter(triangle_list));
        std::copy(normals.begin(), normals.end(), std::back_inserter(normal_list));

    }

    return true;

}

void OpenGLMediator::exportNpts(QVector<QVector3D> totalPoints, const QVector<QVector3D> totalNormals, const QVector<QString> pathNames){

    resizeMesh(totalPoints);

    //    std::string outFile = "shape";
    //    outFile.append("_");
    //    outFile.append(std::to_string(shapeNumber));
    //    outFile.append(".bnpts");

    std::string outFile = pathNames[shapeNumber].toUtf8().constData();

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

void OpenGLMediator::exportOffModels(QVector<QVector3D> totalPoints, const QVector<QVector3D> totalNormals, const QVector<QString> pathNames){

    std::string outFile2 = pathNames[shapeNumber].toUtf8().constData();
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

    std::string outFile3 = pathNames[shapeNumber].toUtf8().constData();
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

void OpenGLMediator::exportPlyModels(QVector<QVector3D> totalPoints, const QVector<QVector3D> totalNormals, const QVector<QString> pathNames){

    resizeMesh(totalPoints);

    std::string outFile3 = pathNames[shapeNumber].toUtf8().constData();
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

void OpenGLMediator::exportPovrayPipe( QVector<QVector3D> points3D) {

    std::string outFile3 = "pipe";
    outFile3.append(".pov");
    std::ofstream fOut;
    fOut.open(outFile3.c_str());


    fOut << "#include \"colors.inc\"" << std::endl;

    fOut << "background{ White }" << std::endl;

    fOut << "camera {\n location <0,0,-2> \n look_at <0,0,0> \n}" << std::endl;

    fOut << "light_source {\n <4,6,-10> \n White \n}" << std::endl;

    for (int i = 0; i < points3D.size()-1; ++i) {
        fOut << "cylinder{ <" << points3D[i].x()/500 <<"," << points3D[i].y()/500 << "," <<points3D[i].z()/200 << ">,<"<< points3D[i+1].x()/500 <<"," << points3D[i+1].y()/500 << "," <<points3D[i+1].z()/200<<">,0.01"
                                                                                                                                                                                                               "\n texture{pigment{color rgb<0,0,1>*0.6}finish { phong 0.4}}scale <1,1,1> rotate<0,0,0> translate<0,0,0>}" << std::endl;
        ///fOut << "cylinder{ <" << points3D[i].x()/500 <<"," << points3D[i].y()/500 << "," <<points3D[i].z()/200 << ">,<"<< points3D[i+1].x()/500 <<"," << points3D[i+1].y()/500 << "," <<points3D[i+1].z()/200<<">,0.01"
        ///    "\n texture{pigment{color rgb<0,0,1>*0.6}finish { phong 0.4}}scale <1,1,1> rotate<0,0,0> translate<0,0,0>}" << std::endl;
    }

    fOut.close();

}



bool OpenGLMediator::resizeMesh(QVector<QVector3D> &totalPoints){

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
