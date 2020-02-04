#ifndef SCENE2_H
#define SCENE2_H


#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>


#include <QtWidgets>


#include <include/input_sketch2.h>

class Scene2: public QGraphicsScene
{

public:

    enum class Interaction {SKETCH, OVERSKETCHING_MODE, DEFINE_ROT_AXIS_MODE, DEFINE_CROSS_SEC_MODE};
    Scene2();

    // tell compiler that it is ok to use a default destructor
    ~Scene2() = default;

    void setOversketchingMode();

    void setDefRotAxisMode();

    void setCrossSecBlendSurfaceMode();

    void smoothSketch();

    void setCurvesSelection( const QVector<QPainterPath>& pathList, const QVector<QString>& namePathList, const QVector<int>& lineLevels );

    void selectPath ( const int pathIndex );

    void estimateShape();

    void showNormals();

    void exportMesh();

    QVector<QPainterPath> getPaths();

    QPainterPath getCurve();

    QVector<QVector3D> getPoints3D();

    QVector<QVector3D> getNormals3D();
protected:

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent (QGraphicsSceneMouseEvent *event);
    void wheelEvent( QGraphicsSceneWheelEvent *event );



private:

    InputSketch2 sketch2;
    Interaction status = Interaction::SKETCH;

};

#endif // SCENE2_H
