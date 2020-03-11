#include "include/layers.h"


Layers::Layers(){


    openContourList = new QListWidget(this);
    closedContourList = new QListWidget(this);
    stripeContourList = new QListWidget(this);

    connect (openContourList, SIGNAL (itemClicked(QListWidgetItem*)), this, SLOT (selectOpenContourItem(QListWidgetItem *)));
    connect (closedContourList, SIGNAL (itemClicked(QListWidgetItem*)), this, SLOT (selectClosedContourItem(QListWidgetItem *)));
    connect (stripeContourList, SIGNAL (itemClicked(QListWidgetItem*)), this, SLOT (selectStripeContourItem(QListWidgetItem *)));

    connect (openContourList, SIGNAL (itemDoubleClicked(QListWidgetItem*)), this, SLOT (renameOpenContourItem(QListWidgetItem *)));
    connect (closedContourList, SIGNAL (itemDoubleClicked(QListWidgetItem*)), this, SLOT (renameClosedContourItem(QListWidgetItem *)));
    connect (stripeContourList, SIGNAL (itemDoubleClicked(QListWidgetItem*)), this, SLOT (renameStripeContourItem(QListWidgetItem *)));

    connect (openContourList, SIGNAL (itemChanged(QListWidgetItem*)), this, SLOT (sendRenamedOpenContourItem(QListWidgetItem *)));
    connect (closedContourList, SIGNAL (itemChanged(QListWidgetItem*)), this, SLOT (sendRenamedClosedContourItem(QListWidgetItem *)));
    connect (stripeContourList, SIGNAL (itemChanged(QListWidgetItem*)), this, SLOT (sendRenamedStripeContourItem(QListWidgetItem *)));

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


void Layers::renameOpenContourItem(QListWidgetItem *item){

    openContourList->editItem(item);

}
void Layers::renameClosedContourItem(QListWidgetItem *item){

    closedContourList->editItem(item);

}
void Layers::renameStripeContourItem(QListWidgetItem *item){

    stripeContourList->editItem(item);

}

void Layers::sendRenamedOpenContourItem(QListWidgetItem *item){

    canvas->renameOpenContour(openContourList->row(item),item->text());
}

void Layers::sendRenamedClosedContourItem(QListWidgetItem *item){

    canvas->renameClosedContour(closedContourList->row(item),item->text());
}

void Layers::sendRenamedStripeContourItem(QListWidgetItem *item){

    canvas->renameStripeContour(stripeContourList->row(item),item->text());
}

void Layers::selectOpenContourItem(QListWidgetItem *item){

    canvas->selectOpenContour(openContourList->row(item));

}
void Layers::selectClosedContourItem(QListWidgetItem *item){

    canvas->selectClosedContour(closedContourList->row(item));
}

void Layers::selectStripeContourItem(QListWidgetItem *item){

    canvas->selectStripeContour(stripeContourList->row(item));
}

void Layers::clearStripeList(){

    stripesNames.clear();
    for (int i = 0; i < stripeContourList->count()-1; ++i) {
        stripesNames.append(stripeContourList->item(i)->text());
    }

    stripeContourList->clear();
    stripeCounter++;

    for (int i = 0; i < stripeCounter-1; ++i) {
        receiveNamePaths(stripesNames[i],2);
    }
}

void Layers::removeItemOpenContourList(int item){
   openContourList->takeItem(item);

}
void Layers::removeItemClosedContourList(int item){
    closedContourList->takeItem(item);
}
void Layers::removeItemStripeContourList(int item){
    stripeContourList->takeItem(item);
}


#include "moc_layers.cpp"
