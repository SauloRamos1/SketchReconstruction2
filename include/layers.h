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

    QListWidget *openContourList;
    QListWidget *closedContourList;
    QListWidget *stripeContourList;

    QListWidgetItem *selectedItem = nullptr;

public slots:

    void receiveNamePaths (const QString& name, const int& type);

    void selectItem(QListWidgetItem *item);
    void renameItem(QListWidgetItem *item);
    void sendRenamedItem (QListWidgetItem *item);


protected:

    Canvas* canvas = nullptr;

private:





};



#endif // LAYERS_H
