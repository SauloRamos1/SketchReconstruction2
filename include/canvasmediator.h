#ifndef CANVASMEDIATOR_H
#define CANVASMEDIATOR_H

#include <QList>
#include <QPainterPath>
#include <QDebug>

#include <memory>

class Canvas;
class Layers;

class CanvasMediator
{

public:

    CanvasMediator();

    ~CanvasMediator() = default;

    void setCanvas( const std::shared_ptr< Canvas >& cv );
    void setLayers( const std::shared_ptr< Layers >& ly );

    void sendPathsAndNames (QPainterPath path, int LineLevel);


    QVector <QString> namePathList;
    QVector <QPainterPath> pathList;
    QVector <int> lineLevels;



public slots:


    void sendNames (QString name);

    void selectPath ( int pathIndex );
    void renamePath(int pathIndex, QString name);

    QVector<QString> getPathNames ();
protected:

    std::shared_ptr< Canvas > canvas = nullptr;
    std::shared_ptr< Layers > layers = nullptr;

private:

    bool almostEqual (QPointF a, QPointF b);

};

#endif // CANVASMEDIATOR_H
