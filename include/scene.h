#ifndef SCENE_H
#define SCENE_H


#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneDragDropEvent>
#include <QGraphicsSceneWheelEvent>
#include <QKeyEvent>
#include <QMimeData>
#include <QUrl>
#include <QImage>
#include <QPixmap>
#include <QLabel>
#include <QGraphicsPixmapItem>
#include <QFileDialog>

#include <include/input_sketch.h>
#include <include/halfedge.h>

#include <memory>


class Scene: public QGraphicsScene
{
    Q_OBJECT
public:

    enum class Interaction {DEFAULT, MOVE_ZOOM, OPENCONTOUR, CLOSEDCONTOUR, STRIPES, CROSS_SELECTION, CROP_SELECTION, ERASE_SELECTION};

    Scene();

    // tell compiler that it is ok to use a default destructor
    ~Scene() = default;

    QPainterPath crossSelection();
    int SelectionLineLevel ();
    QVector<QPainterPath> cropSelection();

    void newFile();
    void loadSVG (const std::string& str);
    void loadIMG (const QString& fileName);


    void chooseDefaultInteraction();
    void chooseMoveZoom_Interaction();

    void chooseOpenContour_Interaction();
    void chooseClosedContour_Interaction();
    void chooseStripes_Interaction();


    void chooseCross_Selection ();
    void chooseCrop_Selection ();
    void chooseErase_Selection ();

    void setOverlapEffect(bool contour, bool halo, bool color, bool hatching);

    void setBackgroundImage( const QString& url );

    QList<QVector<QVector3D>> getOpenContoursPoints ();
    QVector<QVector3D> getClosedContoursPoints ();
    QVector<QVector3D> getClosedContoursNormals ();

    QVector<QVector3D> getStripes ();

    QPainterPath getClosedContour();
    int getClosedContourLevel();

    int sceneeffect_number = 0;

    Interaction status = Interaction::DEFAULT;

    int getInteraction();
    void changeLayerDifference(const int &difference);

    void estimateShapes();

    ///NEW LAYER IMPLEMENTATION
    QString getPathNames();

    void showLabels (bool showLabels);

private:


    InputSketch sketch;
    HalfEdgeGraph halfedge;
    std::shared_ptr< QGraphicsPixmapItem > imageItem;

    QPixmap img;
    QPointF startMovePos;


    qreal scale;

    int level = 2;


    QPointF lastpos;

    bool leftButtonIsPressed = false;
    bool rightButtonIsPressed = false;

signals:

    void openContourDone();
    void closedContourDone();
    void stripeContourDone();

    void crossSelectionDone();
    void cropSelectionDone();


protected:

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent (QGraphicsSceneMouseEvent *event);

    void wheelEvent (QGraphicsSceneWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);



};

#endif // SCENE_H
