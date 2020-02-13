#ifndef LAYERS_H
#define LAYERS_H


#include <memory>

#include <QGraphicsView>
#include <QListWidget>


class Canvas;

class Layers: public QWidget  {

    Q_OBJECT

public:

    Layers();

    ~Layers() = default;

    void linkCanvasDock( Canvas* canvasPointer );

    QListWidget *listWidget = new QListWidget(this);
    QListWidgetItem *selectedItem = nullptr;

    void receivePathNames(const QString& name);

public slots:

    void receiveNamePaths (const QString& namePath);

    void layerUpSelectedCurve();
    void layerDownSelectedCurve();

    void selectItem(QListWidgetItem *item);
    void renameItem(QListWidgetItem *item);
    void sendRenamedItem (QListWidgetItem *item);


    ///NEW


protected:

    Canvas* layerCanvasPointer = nullptr;

private:





};



#endif // LAYERS_H










