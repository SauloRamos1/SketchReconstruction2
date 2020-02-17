#include "include/layers.h"


Layers::Layers(){


    listWidget = new QListWidget(this);


    connect (listWidget, SIGNAL (itemClicked(QListWidgetItem*)), this, SLOT (selectItem(QListWidgetItem *)));
    connect (listWidget, SIGNAL (itemDoubleClicked(QListWidgetItem*)), this, SLOT (renameItem(QListWidgetItem *)));
 //   connect (listWidget, SIGNAL (itemChanged(QListWidgetItem*)), this, SLOT (sendRenamedItem(QListWidgetItem *)));

}

void Layers::setCanvas( Canvas* cv )
{
    canvas = cv;
}

void Layers::receiveNamePaths(const QString& name){

    QListWidgetItem *item = new QListWidgetItem(name, listWidget);
    item->setFlags(item->flags() | Qt::ItemIsEditable);

}

void Layers::layerUpSelectedCurve(){

    if( selectedItem == nullptr ) {
        QMessageBox msgBox;
        msgBox.setText("No contour was selected.");
        msgBox.setInformativeText("Please select a curve before changing it's order");
        msgBox.exec();

        return;
    }

    if (listWidget->count() < 2){
        return;
    }

    if (listWidget->row(selectedItem) == 0){
        return;

    } else {

        int currentRow = listWidget->row(selectedItem);
        int previousRow = currentRow - 1;

        QListWidgetItem* itemOne = listWidget->takeItem(currentRow);

        listWidget->insertItem( previousRow, itemOne );

    }

}

void Layers::layerDownSelectedCurve(){

    if( selectedItem == nullptr ) {
        QMessageBox msgBox;
        msgBox.setText("No contour was selected.");
        msgBox.setInformativeText("Please select a curve before changing it's order");
        msgBox.exec();

        return;
    }

    if (listWidget->count() < 2){
        return;
    }

    if (listWidget->row(selectedItem) == listWidget->count() -1){
        return;

    } else {

        int currentRow = listWidget->row(selectedItem);
        int nextrow = currentRow + 1;

        QListWidgetItem* itemOne = listWidget->takeItem(currentRow);

        listWidget->insertItem( nextrow, itemOne );

    }

}


void Layers::renameItem(QListWidgetItem *item){

    listWidget->editItem(item);

}

void Layers::sendRenamedItem(QListWidgetItem *item){

    //mediator->renamePath(listWidget->row(item),item->text());
}

void Layers::selectItem(QListWidgetItem *item){

    //mediator->selectPath(listWidget->row(item));
    selectedItem = item;
}

#include "moc_layers.cpp"
