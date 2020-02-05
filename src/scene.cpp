#include <QMimeData>
#include <QUrl>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <scene.h>

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <math.h>


#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"
#define NANOSVGRAST_IMPLEMENTATION
#include "nanosvgrast.h"


Scene::Scene()
{
    setSceneRect(QRect (0,0,500,500));

    setBackgroundBrush(QColor(255,230,204));

    //addEllipse( 0, 0, 10, 10 );

    addRect(QRect(0,0,500,500), QPen(QColor(179,179,179),1,Qt::SolidLine), Qt::white);
    addItem ( &sketch );

}

void Scene::chooseDefaultInteraction(){
    status = Interaction::DEFAULT;
    sketch.interactionString = "Default";
    sketch.chooseDefaultInteraction();

    // qDebug () << "Move / Zoom Interaction";
    update();
}

void Scene::chooseMoveZoom_Interaction(){
    status = Interaction::MOVE_ZOOM;
    sketch.interactionString = "Move/Zoom Interaction";
    sketch.chooseMoveZoom_Interaction();

    // qDebug () << "Move / Zoom Interaction";
    update();
}

void Scene::chooseOpenContour_Interaction(){
    status = Interaction::OPENCONTOUR;
    sketch.interactionString = "Open Contour Sketching";
    sketch.chooseOpenContour_Interaction();
    update();
}
void Scene::chooseClosedContour_Interaction(){
    status = Interaction::CLOSEDCONTOUR;
    sketch.interactionString = "Closed Contour Sketching";
    sketch.chooseClosedContour_Interaction();
    update();
}

void Scene::chooseStripes_Interaction(){
    status = Interaction::STRIPES;
    sketch.interactionString = "Stripe Sketching";
    sketch.chooseStripes_Interaction();
    update();
}

void Scene::chooseCross_Selection(){
    status = Interaction::CROSS_SELECTION;
    sketch.interactionString = "Cross Selection Interaction";
    sketch.chooseCrossSelection();

    //qDebug () << "Cross Selection Interaction";
    update();
}
void Scene::chooseCrop_Selection(){
    status = Interaction::CROP_SELECTION;
    sketch.interactionString = "Brush Selection Interaction";
    sketch.chooseCropSelection();

    //qDebug () << "Crop Selection Interaction";
    update();
}
void Scene::chooseErase_Selection(){
    status = Interaction::ERASE_SELECTION;
    sketch.interactionString = "Erase Selection Interaction";
    sketch.chooseEraseSelection();
    //qDebug () << "Erase Selection Interaction";
    update();
}

void Scene::newFile()
{

    //setSceneRect( QRect( 0, 0, 500, 500 ) );

    sketch.clear();

    removeItem(imageItem.get());

    //addPixmap(imageItem->pixmap());

    update();

}

void Scene::loadSVG(const std::string& str)
{

    const char *file = str.c_str();

    NSVGimage* image;

    image = nsvgParseFromFile(file, "px", 96);

    int ncurvas = 0;

    QVector<QVector<float>> bezierCurvePointsList;

    for (NSVGshape *shape = image->shapes; shape != nullptr; shape = shape->next) {
        for (NSVGpath *path = shape->paths; path != nullptr; path = path->next) {

            for (int i = 0; i < path->npts-1; i += 3) {
                float* p = &path->pts[i*2];

                QVector<float> curva(8);
                curva[0] = (float)p[0] ;
                curva[1] = (float)p[1] ;
                curva[2] = (float)p[2] ;
                curva[3] = (float)p[3] ;
                curva[4] = (float)p[4] ;
                curva[5] = (float)p[5] ;
                curva[6] = (float)p[6] ;
                curva[7] = (float)p[7] ;



                if (!((curva[0] == curva [6]) && (curva[1] == curva[7]) && curva[2] == curva [4])){
                    bezierCurvePointsList.push_back(curva);
                    //qDebug () << curva;


                    ncurvas += 1;
                }

            }
        }
    }


    halfedge.svgPathsTohalfedgestructure(bezierCurvePointsList);

    QVector<QPainterPath> svgPaths;
    svgPaths = halfedge.separaciclos();

    sketch.drawPaths(svgPaths);

    cout << "# of Paths: " << svgPaths.size() << endl;

    update();

    // Delete
    nsvgDelete(image);
    qDebug () << ncurvas;
}

void Scene::loadIMG(const QString &fileName)
{
    setBackgroundImage( fileName );
}

void Scene::setOverlapEffect(bool contour, bool halo, bool color, bool hatching){

    sketch.contour = contour;
    sketch.halo = halo;
    sketch.color = color;
    sketch.hatching = hatching;

    sceneeffect_number = 0;

}




void Scene::setBackgroundImage( const QString& url )
{

    img = QPixmap (url);

    imageItem.reset();
    imageItem = std::make_shared< QGraphicsPixmapItem >();

    imageItem->setPixmap(img);
    imageItem->setFlag( QGraphicsItem::ItemIsMovable, true );
    imageItem->setFlag( QGraphicsItem::ItemIsSelectable, true );

    addItem( imageItem.get() );

    imageItem->stackBefore(&sketch);

    //    QPointF pos = sceneRect().center();
    //    pos -= QPointF( img.width()*0.5, img.height()*0.5 );
    //    imageItem->setPos( pos );

    //    QSizeF rect;
    //    rect.setWidth(imageItem->x());
    //    rect.setHeight(imageItem->y());

    //    sceneRect().setSize(rect);

    update();
}




QPainterPath Scene::crossSelection()
{
    return sketch.getCrossSelectionPath();
}

int Scene::SelectionLineLevel()
{
    return sketch.getLineLevel();
}

QVector<QPainterPath> Scene::cropSelection(){

    return sketch.getCropSelectionPaths();
}

void Scene::mousePressEvent(QGraphicsSceneMouseEvent *event){

    QPointF pos = event->scenePos();
    if (status == Interaction::DEFAULT){
        return;
    }
    if (event->buttons() == Qt::LeftButton && status == Interaction::OPENCONTOUR){
        setFocus();
        leftButtonIsPressed = true;
        sketch.createOpenContour( pos );
    }
    if (event->buttons() == Qt::LeftButton && status == Interaction::CLOSEDCONTOUR){
        setFocus();
        leftButtonIsPressed = true;
        sketch.createClosedContour( pos );
    }
    if (event->buttons() == Qt::LeftButton && status == Interaction::STRIPES){
        setFocus();
        sketch.createStripeContour( pos );
    }

    if ( event->buttons() == Qt::LeftButton && status == Interaction::MOVE_ZOOM && imageItem->isUnderMouse()){

        imageItem->setSelected(true);
        startMovePos = pos;

    } else if ( event->buttons() == Qt::LeftButton && status == Interaction::MOVE_ZOOM && !(imageItem->isUnderMouse())){

        imageItem->setSelected(false);
    }

    update();

}


void Scene::mouseMoveEvent(QGraphicsSceneMouseEvent *event){

    QPointF pos = event->scenePos();

    if (status == Interaction::DEFAULT){
        return;
    }
    if (event->buttons() == Qt::LeftButton && status == Interaction::OPENCONTOUR){
        sketch.addOpenContour( pos );
        QGraphicsScene::mouseMoveEvent(event);
    }
    if (event->buttons() == Qt::LeftButton && status == Interaction::CLOSEDCONTOUR){
        sketch.addClosedContour( pos );
        QGraphicsScene::mouseMoveEvent(event);
    }
    if (event->buttons() == Qt::LeftButton && status == Interaction::STRIPES){
        sketch.addStripeContour( pos );
        QGraphicsScene::mouseMoveEvent(event);
    }
    if (event->buttons() == Qt::LeftButton && status == Interaction::MOVE_ZOOM && imageItem->isSelected()){

        imageItem->moveBy(pos.x() - startMovePos.x(), pos.y() - startMovePos.y() );
        startMovePos = pos;
        update();
        QGraphicsScene::mouseMoveEvent(event);
    }

    sketch.mouseOverPoint(pos);

    update();
    QGraphicsScene::mouseMoveEvent(event);

}

void Scene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){

    QPointF pos = event->scenePos();

    if (status == Interaction::DEFAULT){
        return;
    }

    if (event->button() & Qt::LeftButton && status == Interaction::OPENCONTOUR){
        sketch.saveOpenContour();
        leftButtonIsPressed = false;
        QGraphicsScene::mouseReleaseEvent(event);
    }

    if (event->button() & Qt::LeftButton && status == Interaction::CLOSEDCONTOUR){
        sketch.saveClosedContour();
        leftButtonIsPressed = false;
        emit closedContourDone();
        QGraphicsScene::mouseReleaseEvent(event);
    }

    if (event->button() & Qt::LeftButton && status == Interaction::STRIPES){
        sketch.saveStripeContour();
        QGraphicsScene::mouseReleaseEvent(event);
    }

    if (event->button() & Qt::RightButton && status == Interaction::STRIPES){

        sketch.finishBand();
        QGraphicsScene::mouseReleaseEvent(event);
    }

    if (event->button() & Qt::LeftButton && status == Interaction::CROSS_SELECTION){

        sketch.crossSelection();
        emit crossSelectionDone();

    }
    if (event->button() & Qt::LeftButton && status == Interaction::CROP_SELECTION) {

        sketch.cropSelection();
        emit cropSelectionDone();


    }
    if (event->button() & Qt::LeftButton && status == Interaction::ERASE_SELECTION) {

        sketch.eraseSelection();

    }

    update ();
}

void Scene::wheelEvent ( QGraphicsSceneWheelEvent *event )
{

    if (status == Interaction::MOVE_ZOOM && imageItem->isSelected()){

        if (event->delta() > 0 ){

            QMatrix m = imageItem->matrix();
            m.scale (1.01,1.01);
            imageItem->setMatrix( m );

        } else if (event->delta() < 0 ){

            QMatrix m = imageItem->matrix();
            m.scale (0.99,0.99);
            imageItem->setMatrix( m );

        }
    }  else if (status == Interaction::STRIPES) {

        if (event->delta() > 0 ){


            if (sketch.twistingThickness < 25){
                sketch.twistingThickness +=1;
                //sketch.savePath();
            }
        } else if (event->delta() < 0 ){



            if (sketch.twistingThickness > -25){
                sketch.twistingThickness -=1;
                //sketch.savePath();

            }
        }

    }
    event->accept();

    update ();
}

void Scene::keyPressEvent(QKeyEvent *event){


    if ( event->key() == Qt::Key_Plus && status == Interaction::OPENCONTOUR ){
        if ( leftButtonIsPressed ){

            sketch.increaseOpenContourLevelWhileDrawing();

        } else {

            sketch.saveOpenContour();

            sketch.increaseLevel();

        }
    }

    if ( event->key() == Qt::Key_Minus && status == Interaction::OPENCONTOUR ){

        if (sketch.lineLevel == 1){
            return;
        }

        if ( leftButtonIsPressed ){


            sketch.decreaseOpenContourLevelWhileDrawing();

        } else {

            sketch.saveOpenContour();

            sketch.decreaseLevel();

        }
    }

    if ( event->key() == Qt::Key_Plus && status == Interaction::CLOSEDCONTOUR && !leftButtonIsPressed ){
        sketch.increaseLevel();
    }

    if ( event->key() == Qt::Key_Minus && status == Interaction::CLOSEDCONTOUR && !leftButtonIsPressed ){

        if (sketch.lineLevel == 1){
            return;
        }
        sketch.decreaseLevel();
    }

    if ( event->key() == Qt::Key_Plus && status == Interaction::STRIPES ){
        if ( leftButtonIsPressed ){

            sketch.increaseStripeContourLevelWhileDrawing();

        } else {

            sketch.increaseLevel();
        }
    }

    if ( event->key() == Qt::Key_Minus && status == Interaction::STRIPES ){

        if (sketch.lineLevel == 1){
            return;
        }

        if ( leftButtonIsPressed ){

            sketch.decreaseStripeContourLevelWhileDrawing();


        } else {

            sketch.decreaseLevel();

        }
    }

    sketch.updateColorMap();
}


QVector<QVector3D> Scene::getOpenContoursPoints()
{
    return sketch.getOpenContoursPoints();
}

//QVector<QVector3D> Scene::getClosedContoursPoints()
//{
//    return sketch.getClosedContoursPoints();
//}

QVector<QVector3D> Scene::getStripes()
{
    return sketch.getStripesPoints();
}



QPainterPath Scene::getClosedContour()
{
    return sketch.getClosedContour();
}

int Scene::getClosedContourLevel()
{
    return sketch.getClosedContourLevel();
}

int Scene::getInteraction(){

    if (status == Interaction::OPENCONTOUR){
        return 1;
    }

    if (status == Interaction::CLOSEDCONTOUR){
        return 2;
    }
    if  (status == Interaction::STRIPES){
        return 3;
    }
    return 0;
}






#include "moc_scene.cpp"
