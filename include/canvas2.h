#ifndef CANVAS2_H
#define CANVAS2_H


// Necessary for std::shared_ptr<T> and std::make_shared<T>
#include <memory>

#include <QGraphicsView>
#include <QtWidgets>
#include <QSlider>
#include <QDockWidget>
#include <QMainWindow>

#include "scene2.h"

class CanvasMediator;
class OpenGLMediator;

class Canvas2: public QGraphicsView
{

    Q_OBJECT

public:

    Canvas2();

    // tell compiler that it is ok to use a default destructor
    ~Canvas2() = default;

    void setOpenGLMediator (OpenGLMediator* glmed);
    void setMediator( CanvasMediator* med );
    void setCurvesSelection( const QVector<QPainterPath>& pathList, const QVector<QString>& namePathList, const QVector<int>& lineLevels );


public slots:

    //void selectCurve( int );
    void setOversketchingMode(  );

    void smoothSketch();

    void setDefRotAxisMode(  );

    void setCrossSecBlendSurfaceMode();

    void selectPath (const int pathIndex );

    void view3DPoints();

    void estimateShape();

    void showNormals();

    void exportMesh();

    QVector<QPainterPath> getPaths();


protected:

    std::shared_ptr< Scene2 > scene2;
    CanvasMediator* mediator = nullptr;
    OpenGLMediator* glmediator = nullptr;


};

#endif // CANVAS2_H
