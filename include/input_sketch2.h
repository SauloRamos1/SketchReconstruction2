#ifndef INPUTSKETCH2_H
#define INPUTSKETCH2_H

#include <QGraphicsPathItem>
#include <QPainterPath>
#include <QMessageBox>
#include <QVector3D>
#include <QInputDialog>
#include <QDialogButtonBox>



#include <QHBoxLayout>
#include "sketch_library.h"

class QPainter;

class InputSketch2: public QGraphicsPathItem
{


public:
    InputSketch2 ();

    void create (const QPointF& pos);
    void add (const QPointF& pos);

    void smooth();

    void receiveSelectedPath (const QVector<QPainterPath>& pathList, const QVector<QString>& namePathList, const QVector<int>& lineLevels);

    void selectPath ( const int pathIndex );

    bool joinPaths(); //OverSketching

    void defRotAxis(int direction);

    void showCrossSectionalLine();
    void defCrossSecToEdit(int direction);

    void estimateShape();

    void showNormals();

    void exportMesh ();

    QVector<QPainterPath> getPaths();

    QPainterPath getCurve();

    QVector<QVector3D> getPoints3D();

    QString interactionMode = "Selection Mode";


    QVector<QVector3D> getNormals3D();
protected:

    void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget ) override;
    virtual QRectF boundingRect() const override;


private:

    bool showNormalsToggle = false;

    //Export Bnpts, OFF, PLY

    bool resizeMesh (QVector <QVector3D> &totalPoints);

    QPainterPath curve;
    int selectedPathIndex;

    struct Vertex{
        QVector3D point3D;
        int vertexNumber;
    };

    struct allSampledPoints{

        QVector<QVector3D> midPoints, ql, qr;
        QVector<double> anglesql, anglesqr;
        QPainterPath midPointsPath;
        QPainterPath crossSectionalRotationalPath;
        QVector<QVector<Vertex>> shapePoints; // Matrix (u,v) points
        //Incluir Paths and Names nesta Struct!! Mais facil organizacao
    };

    QVector<allSampledPoints> allShapesSampledPoints;



    QPainterPath oversketchingCurve;
    QPainterPath smoothedCurve;
    QPainterPath newMedialAxis;

    QVector<QPainterPath> paths;
    QVector<QString> names;

    QVector <QVector3D> totalPoints;
    QVector <QVector3D> totalNormals;

    QVector <QVector<QVector3D>> exportPoints;
    QVector <QVector<QVector3D>> exportNormals;

    //Export
    QVector<QVector<int>> topology;

    int contourSamplingStep = 5;
    int u = 0,v = 0, u_step = 1, v_step = 5;

    QVector <QVector3D> pointsView;
    QVector <QVector3D> normalsView;
    QVector <QVector3D> normalsFor3Ddisks;

    void exportNpts (QVector<QVector3D> totalPoints, const QVector<QVector3D> totalNormals, const QString pathName);

    void exportOffModels (QVector<QVector3D> totalPoints, const QVector<QVector3D> totalNormals, const QString pathName);

    void exportPlyModels (QVector<QVector3D> totalPoints, const QVector<QVector3D> totalNormals, const QString pathName);

    // Rotational Blending Surface

    void samplePointsForRotationalBlendingSurface (QPainterPath &selectedCurve);

    void RotationalBlendingSurface (const int shapeNumber, QPainterPath &contour, QVector<QVector3D>& ql, QVector<QVector3D>& qr);
    void DataForHRBF (const int shapeNumber, QPainterPath &contour, QVector<QVector3D>& ql, QVector<QVector3D>& qr);

    void CrossSectionalBlendingSurface (QPainterPath &tu, QVector<QVector3D>& ql, QVector<QVector3D>& qr);

    void findSymmetricalMedialAxis (QPainterPath contour, QVector<QVector3D> &temp_ql, QVector<QVector3D> &temp_qr);

    double cosine_similarity_vectors(QVector<double> A, QVector<double>B);

    QPainterPath lastmidPointsPath;

    int lineLeveldiff = 0;

    QVector <QLineF> normalsContour;

    float getPathArea(QPainterPath p, float step);
};


#endif // INPUTSKETCH2_H
