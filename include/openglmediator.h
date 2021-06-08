#ifndef OPENGLMEDIATOR_H
#define OPENGLMEDIATOR_H


#define XCUBE 0
#define INV_MULT 1
#define GAUSSIAN 2
#define MULTIQUADRIC 3
#define GEN_INV_MULT 4

#include <memory>
#include <vector>

#include <QVector>
#include <QPainterPath>
#include <QVector3D>

#include <QProgressDialog>
// RBF Exporter
#include "xCubeEigen/RBF.h"
#include "xCubeEigen/XCube.h"
#include "xCubeEigen/InverseMultiquadrics.h"
#include "xCubeEigen/Multiquadrics.h"
#include "xCubeEigen/GeneralInverseMultiquadrics.h"
#include "xCubeEigen/Gaussian.h"
#include "xCubeEigen/HBRBF.h"
#include "xCubeEigen/HBRBFEval.h"
#include "xCubeEigen/VTKExport.h"
#include "xCubeEigen/polyg/SurfacePolygonizer.h"

// -----------

class Canvas;
class OpenGLCanvas;

class OpenGLMediator

{

public:

    OpenGLMediator();

    ~OpenGLMediator() = default;

    void setCanvas( const std::shared_ptr< Canvas >& cv );
    //    void setCanvas2( const std::shared_ptr< Canvas2 >& cv2 );
    void setGlCanvas( const std::shared_ptr< OpenGLCanvas >& glcanvas);

    //    void viewSketch3D(const QVector<QVector3D> points3D, const QVector<QString> pathNames);
    //    void viewOverlapping3D (const QVector<QVector3D> points3D);


    // void viewStripe();
    void viewOpenContours3D(const QList<QVector<QVector3D>> points3D,  const QList<QString> openContourListNames);
    void viewClosedContours3D(const QVector<QVector3D> points3D,  const QVector<QVector3D> normals3D);
    void viewStripes3D(const QList<QVector<QVector3D>> points3D);

    QVector<QVector3D> createCylinder(int steps, QVector3D base, QVector3D top);
    QVector<QVector3D> createDisk(int steps, QVector3D base, QVector3D top);
    void exportView();

    int nvertices = 0;
    void clearTriangles();
    void render(bool finalRender);

    void exportOpenContours3D();

    void exportStripes3D(const QList<QVector<QVector3D>> points3D);

    void exportHRBFMesh();
    void exportRBSMesh();

    void exportFinalPlyModel(const QString fileName);

    void createRBSData(QString name, const int shapeNumber, QPainterPath contour, QVector<QVector3D> ql, QVector<QVector3D> qr, double contourDepth);

    void createHRBFData(QString name, const int shapeNumber, QPainterPath contour, double contourDepth);

    void clearRBSMeshes();


public slots:

protected:

    std::shared_ptr< Canvas > canvas = nullptr;
    //std::shared_ptr< Canvas2 > canvas2 = nullptr;
    std::shared_ptr< OpenGLCanvas > glcanvas = nullptr;



private:

    //Model


    bool setCurve(const QPolygonF& sketch);

    bool setShape(const QVector<QVector3D> &totalPoints);

    QPainterPath resizePath (QPainterPath &path);

    std::vector< float > vertices, normals;
    std::vector< unsigned int > faces;


    std::vector< float > CCvertices, CCnormals;
    std::vector< unsigned int > CCfaces;
    std::vector< float > Svertices, Snormals;
    std::vector< unsigned int > Sfaces;


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

    struct Vertex{
        QVector3D point3D;
        int vertexNumber;
    };

    float getPathArea(QPainterPath p, float step);
    QList<QString> rbfDataFiles;

    struct RBSMesh {
        QList<QList<Vertex>> meshData;
        QVector<QVector<int>> meshTopology;
        int meshSize;
        QVector<QVector3D> meshNormals;
        QString name;
    };


    QList<RBSMesh> rbsMeshesList;
    QVector <QVector3D> hrbfCentersForNormals;

    struct OpenContourMesh{
        std::vector< float > OCvertices, OCnormals;
        std::vector< unsigned int > OCfaces;
        QString name;
    };

    QList<OpenContourMesh> openContourMeshesList;

};

//template<typename VertexList, typename EdgeList>
//bool OpenGLMediator::setCurve(const VertexList& vertex_coordinates,
//                     const EdgeList& edges_list )
//{

//    return true;
//}

#endif // OPENGLMEDIATOR_H
