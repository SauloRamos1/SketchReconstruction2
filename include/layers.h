#ifndef LAYERS_H
#define LAYERS_H


#include <memory>

#include <QGraphicsView>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMessageBox>

#include <canvas.h>

class Canvas;

class Layers: public QWidget  {

    Q_OBJECT

public:

    Layers();

    ~Layers() = default;

    void setCanvas( Canvas* cv );

    QListWidget *listWidget;
    QListWidgetItem *selectedItem = nullptr;

public slots:

    void receiveNamePaths (const QString& name);

    void layerUpSelectedCurve();
    void layerDownSelectedCurve();

    void selectItem(QListWidgetItem *item);
    void renameItem(QListWidgetItem *item);
    void sendRenamedItem (QListWidgetItem *item);


protected:

    Canvas* canvas = nullptr;

private:





};



#endif // LAYERS_H
