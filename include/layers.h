#ifndef LAYERS_H
#define LAYERS_H


#include <memory>

#include <QGraphicsView>
#include <QListWidget>


class CanvasMediator;

class Layers: public QWidget  {

    Q_OBJECT

public:

    Layers();

    ~Layers() = default;

    void setMediator( CanvasMediator* med );

    QListWidget *listWidget = new QListWidget(this);
    QListWidgetItem *selectedItem = nullptr;

public slots:

    void receiveNamePaths (const QString& namePath);

    void layerUpSelectedCurve();
    void layerDownSelectedCurve();

    void selectItem(QListWidgetItem *item);
    void renameItem(QListWidgetItem *item);
    void sendRenamedItem (QListWidgetItem *item);


protected:

    CanvasMediator* mediator = nullptr;

private:





};



#endif // LAYERS_H










