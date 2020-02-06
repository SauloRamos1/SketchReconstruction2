#ifndef OPENGLMEDIATOR_H
#define OPENGLMEDIATOR_H

#include <memory>
#include <vector>

#include <QVector>
#include <QPainterPath>


class Canvas;
class Canvas2;
class OpenGLCanvas;

class OpenGLMediator

{

public:

    OpenGLMediator();

    ~OpenGLMediator() = default;

    void setCanvas( const std::shared_ptr< Canvas >& cv );
    void setCanvas2( const std::shared_ptr< Canvas2 >& cv2 );
    void setGlCanvas( const std::shared_ptr< OpenGLCanvas >& glcanvas);

//    void viewSketch3D(const QVector<QVector3D> points3D, const QVector<QString> pathNames);
//    void viewOverlapping3D (const QVector<QVector3D> points3D);


   // void viewStripe();
    void viewOpenContours3D(const QVector<QVector3D> points3D);
    void viewClosedContours3D(const QVector<QVector3D> points3D,  const QVector<QVector3D> normals3D);
    void viewStripes3D(const QVector<QVector3D> points3D);

    QVector<QVector3D> createCylinder(int steps, QVector3D base, QVector3D top);
    void exportMesh();

protected:

    std::shared_ptr< Canvas > canvas = nullptr;
    std::shared_ptr< Canvas2 > canvas2 = nullptr;
    std::shared_ptr< OpenGLCanvas > glcanvas = nullptr;


private:

    //Model


    bool setCurve(const QPolygonF& sketch);

    bool setShape(const QVector<QVector3D> &totalPoints);

    QPainterPath resizePath (QPainterPath &path);

    std::vector< float > vertices, normals;
    std::vector< unsigned int > faces;

    std::vector< float > OCvertices, OCnormals;
    std::vector< unsigned int > OCfaces;
    std::vector< float > CCvertices, CCnormals;
    std::vector< unsigned int > CCfaces;
    std::vector< float > Svertices, Snormals;
    std::vector< unsigned int > Sfaces;

    void render();


    bool getMesh(std::vector<float> &vertex_coordinates,
            std::vector<unsigned int> &triangle_list,
            std::vector<float> &normal_list);

    bool getMeshPath(std::vector<float> &vertex_coordinates,
            std::vector<unsigned int> &triangle_list,
            std::vector<float> &normal_list);


    std::vector<float> curve_coordinates;
    std::vector<unsigned int> curve_edges;

    // Normal Estimation

    bool estimatePolygonNormals ( const QPainterPath &path, QVector <QVector3D> &totalPoints, QVector <QVector3D> &totalNormals);

    // Export

    void exportNpts (QVector<QVector3D> totalPoints, const QVector<QVector3D> totalNormals, const QVector<QString> pathNames);

    void exportOffModels (QVector<QVector3D> totalPoints, const QVector<QVector3D> totalNormals, const QVector<QString> pathNames);

    void exportPlyModels (QVector<QVector3D> totalPoints, const QVector<QVector3D> totalNormals, const QVector<QString> pathNames);

    int shapeNumber = 0;

    bool resizeMesh (QVector <QVector3D> &totalPoints);

    void exportPovrayPipe(QVector<QVector3D> points3D);

    QVector3D CalculateSurfaceNormal(QVector3D p0, QVector3D p1, QVector3D p2);


};

//template<typename VertexList, typename EdgeList>
//bool OpenGLMediator::setCurve(const VertexList& vertex_coordinates,
//                     const EdgeList& edges_list )
//{

//    return true;
//}

#endif // OPENGLMEDIATOR_H