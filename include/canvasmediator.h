#ifndef CANVASMEDIATOR_H
#define CANVASMEDIATOR_H

#include <QList>
#include <QPainterPath>
#include <QDebug>

#include <memory>

class Canvas2;
class Layers;

class CanvasMediator
{

public:

    CanvasMediator();

    ~CanvasMediator() = default;

    void setCanvas2( const std::shared_ptr< Canvas2 >& cv2 );
    void setLayers( const std::shared_ptr< Layers >& ly );

    void sendPathsAndNames (QPainterPath path, int LineLevel);


    QVector <QString> namePathList;
    QVector <QPainterPath> pathList;
    QVector <int> lineLevels;



public slots:


    void sendNames (QString name);
    void sendSketchedPaths(const QVector<QPainterPath> pathsToCanvas2);
    void sendClosedPaths( const QPainterPath pathsToReconstruct, const int& lineLevel);
    void closeAndSendPaths( const QVector<QPainterPath>& pathsToReconstruct, const int& lineLevel);

    void closeAndSendSketchedPaths(const QVector<QPainterPath> &pathsToCanvas2);

    void selectPath ( int pathIndex );
    void renamePath(int pathIndex, QString name);

    QVector<QString> getPathNames ();
protected:

    std::shared_ptr< Canvas2 > canvas2 = nullptr;
    std::shared_ptr< Layers > layers = nullptr;

private:

    bool almostEqual (QPointF a, QPointF b);

};

#endif // CANVASMEDIATOR_H
