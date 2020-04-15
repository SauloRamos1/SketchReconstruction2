#ifndef INPUTSKETCH_H
#define INPUTSKETCH_H

#include <QGraphicsPathItem>
#include <QPainterPath>
#include <QMessageBox>
#include <QtMath>
#include <QVector3D>
#include <QInputDialog>
#include <QListWidget>


#include <QtSvg/QSvgGenerator>

#include "sketch_library.h"
#include "halfedge.h"

//#include "canvasmediator.h"


class QPainter;

class InputSketch : public QGraphicsPathItem
{


public:
    enum class Interaction {DEFAULT, OPENCONTOUR, CLOSEDCONTOUR, STRIPES, MOVE_ZOOM, CROSS_SELECTION, CROP_SELECTION, ERASE_SELECTION};

    InputSketch();

    void setOverlapEffect (bool contour, bool halo, bool color, bool hatching);

    void drawPaths (const QVector<QPainterPath>& readsvgPaths);

    int number_subpaths();

    void chooseDefaultInteraction();

    void chooseMoveZoom_Interaction();

    void chooseOpenContour_Interaction();
    void chooseClosedContour_Interaction();
    void chooseStripes_Interaction();

    void chooseCrossSelection();
    void chooseCropSelection();
    void chooseEraseSelection();

    void createSelectionCurve(const QPointF &pos);
    void addSelectionCurve(const QPointF &pos);

    void crossSelection();
    void cropSelection();
    int eraseSelection();

    QVector <HalfEdge*> cyclesList;
    QString interactionString = "Default Mode";


    QPainterPath getCrossSelectionPath() const;
    QVector<QPainterPath> getCropSelectionPaths() const;

    //QVector<QVector3D> getSketchedPaths ();

    QPolygonF getCurve() const;
    void joinPaths();

    void clear();

    void savePath();

    bool chaikinOnZ(QVector<QVector3D> &pointsFor3D);

    int twistingThickness = 10;

    int getLineLevel() const;

    void mouseOverPoint(const QPointF &pos);
    void finishBand();

    bool contour = true, halo = false, color = false, hatching = false;
    int lineLevel = 1;
    int layerDifference = 10;

    void createOpenContour(const QPointF &pos);
    void createClosedContour(const QPointF &pos);
    void createStripeContour(const QPointF &pos);

    void addOpenContour(const QPointF &pos);
    void addClosedContour(const QPointF &pos);
    void addStripeContour(const QPointF &pos);

    bool saveOpenContour();
    bool saveClosedContour();
    bool saveStripeContour();

    void increaseOpenContourLevelWhileDrawing();
    void decreaseOpenContourLevelWhileDrawing();

    void increaseLevel ();
    void decreaseLevel ();

    void increaseStripeContourLevelWhileDrawing();
    void decreaseStripeContourLevelWhileDrawing();

    QPainterPath closePath(QPainterPath pathToBeClosed);
    QPainterPath closeSVGSegments(const QVector<QPainterPath> &pathsToClose);

    QList<QVector<QVector3D>> getOpenContoursPoints();
    QVector<QVector3D> getClosedContoursPoints();

    QVector<QVector3D> getStripesPoints();

    void changeLayerDifference(const int &difference);
    QPainterPath getClosedContour();
    int getClosedContourLevel();
    void updateColorMap();

    QVector<QVector3D> getClosedContoursNormals();

    void estimateShapes();

    QString getPathNames();

    void setShowLabels (bool _showLabels);

    void selectOpenContour (const int openContourIndex);
    void selectClosedContour (const int closedContourIndex);
    void selectStripeContour (const int stripeContourIndex);


    void createOversketchingCurve(const QPointF &pos);
    void addOversketchingCurve(const QPointF &pos);

    void smooth();
    void defRotAxis(int direction);



    void renameOpenContour(int itemNumber, QString name);
    void renameClosedContour(int itemNumber, QString name);
    void renameStripeContour(int itemNumber, QString name);

    bool isOpenContoursEmpty();
    bool isClosedContoursEmpty();
    bool isStripeContoursEmpty();

    void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget ) override;
    void setSaveSVG();
    int getErasedTypeContour();
    int getErasedOpenContourNumber();
    int getErasedClosedContourNumber();
    int getErasedStripeContourNumber();

    QList<QString> getLayerList();
public slots:

protected:

    virtual QRectF boundingRect() const override;



private:

    /// Open Contour
    ///

    // TER uma Lista de Itens para manda para o Layers


    struct QPainterPath3D {
        QPainterPath contour;
        int level;
        QString name;
        int attachClosedContour = 0;
        float maior_z = 0;
    };

    struct Stripe3D {
        QPainterPath contour, leftLine, rightLine;
        int level;
        QString name;
    };


    QPainterPath openContour;
    QList<QPainterPath3D> sameOpenContourList;
    QList<QList<QPainterPath3D>> openContourList;

    /// --------
    /// Closed Contour

    QPainterPath closedContour;
    int closedContourIntersects = -1;
    QList<QPainterPath3D> closedContourList;

    /// --------
    /// Stripe

    QPainterPath stripeContour;
    QList<Stripe3D> sameStripeContourList;
    QList<QList<Stripe3D>> stripeContourList;


    /// --------
    /// Gray Scale Lines
    ///

    QList<QVector3D> lineColorMap, shapeColorMap;
    QList<QBrush> hatchingMap;


    int numberOfLayers = 1;

    QVector <int> levelList;

    bool showLabels = true;

    QPainterPath selectedPath;



    /// --------
    ///
    ///

    QList<QString> names;

    QPainterPath curve;

    QVector <QPainterPath> svgPaths;

    QVector <QPainterPath> pathsList;

    QVector <QPainterPath> selectedPathsList;


    QVector <QRectF> rectList;

    QPointF startMovePoint;
    int pathToMove = -1;
    int indexToMove = -1;

    QPainterPath selectedPathOnCrossSelection;
    QVector <QPainterPath> selectedPathsOnCropSelection;


    // SELECTION CONTOURS
    Interaction statusSketch = Interaction::DEFAULT;


    // Overlapping Variables

    bool lineInside = false;


    QPolygonF curvePolygon;
    QPolygonF pathPolygon;
    QPolygonF currentPolySugestion;

    QVector <QPolygonF> polygonList;

    QVector <QPainterPath> linesToPolygon;


    int selectedLineLevel = 0;

    QVector <QPointF> twistingPoints;
    QVector <int> twistingSize;

    QVector<QPainterPath>secondbandLineListLeft, secondbandLineListRight ;
    QVector<int> secondbandLineLeftLevelList, secondbandLineRightLevelList;
  //  QVector<QPainterPath> finalSecondBandLineListLeft, finalSecondBandLineListRight;

    QVector <QPainterPath> pathsfor3D;
    QVector <int> levelListfor3D;
    QPointF mousePointer;


    bool front = true;//True = front, False = Back
    //Front/True -> Red Line on front
    //Back/False -> Blue Line on front

    QPainterPath smoothPath(QPainterPath &path);
    QPainterPath finalTotalPath;
    QList<QPolygonF> finalPolygonStripe;
    QList<QPolygonF> finalPolygonStripeQuad;
    QVector<QVector3D> triangleMesh;

    int stripeNumber = 0;

    bool almostEqual (QPointF a, QPointF b);



    //************************************************************************************************
    /// ................................... InputSketch 2 ..........................................
    //************************************************************************************************
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
        QString name;
        QPainterPath contour; //Path, contour, curve
    };

    QVector<allSampledPoints> allShapesSampledPoints;

    QPainterPath lastmidPointsPath;

    void receiveSelectedPath (const QPainterPath &path, const QString &name, const int &lineLevel) ;


    void RotationalBlendingSurface (const int shapeNumber, QPainterPath &contour, QVector<QVector3D>& ql, QVector<QVector3D>& qr);


    QVector <QVector3D> pointsFor3Ddisks;
    QVector <QVector3D> normalsFor3Ddisks;

    void DataForHRBF (const int shapeNumber, QPainterPath &contour, QVector<QVector3D>& ql, QVector<QVector3D>& qr);

//    QVector <QVector3D> totalPoints;
//    QVector <QVector3D> totalNormals;

    QVector <QLineF> normalsContour;

    float getPathArea(QPainterPath p, float step);

    int lastSelected = 5;
    int selectedOpenContour;
    int selectedClosedContour;
    int selectedStripeContour;

    int nOpenContours = 0;
    int nClosedContours = 0;
    int nStripeContours = 0;

    QPainterPath oversketchingCurve;


    void samplePointsForRotationalBlendingSurface(QPainterPath &selectedCurve);

    bool saveSVGFlag = false;
};

#endif // INPUTSKETCH_H
