#include "include/layers.h"


Layers::Layers(){


    openContourList = new QListWidget(this);
    closedContourList = new QListWidget(this);
    stripeContourList = new QListWidget(this);

    //    connect (listWidget, SIGNAL (itemClicked(QListWidgetItem*)), this, SLOT (selectItem(QListWidgetItem *)));
    //   connect (listWidget, SIGNAL (itemDoubleClicked(QListWidgetItem*)), this, SLOT (renameItem(QListWidgetItem *)));
    //   connect (listWidget, SIGNAL (itemChanged(QListWidgetItem*)), this, SLOT (sendRenamedItem(QListWidgetItem *)));

}

void Layers::setCanvas( Canvas* cv )
{
    canvas = cv;
}

void Layers::receiveNamePaths(const QString& name, const int& type){

    if (type == 0) {
        QListWidgetItem *item = new QListWidgetItem(name, openContourList);
        item->setFlags(item->flags() | Qt::ItemIsEditable);

    }
    if (type == 1) {
        QListWidgetItem *item = new QListWidgetItem(name, closedContourList);
        item->setFlags(item->flags() | Qt::ItemIsEditable);

    }
    if (type == 2) {
        QListWidgetItem *item = new QListWidgetItem(name, stripeContourList);
        item->setFlags(item->flags() | Qt::ItemIsEditable);

    }


}


void Layers::renameItem(QListWidgetItem *item){

    //listWidget->editItem(item);

}

void Layers::sendRenamedItem(QListWidgetItem *item){

    //mediator->renamePath(listWidget->row(item),item->text());
}

void Layers::selectItem(QListWidgetItem *item){

    //mediator->selectPath(listWidget->row(item));
    selectedItem = item;
}

#include "moc_layers.cpp"
