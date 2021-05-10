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
    //setSceneRect(QRect (-50,0,750,750));
    setSceneRect(QRect (-30,0,520,520));

    //setBackgroundBrush(QColor(255,230,204));
    setBackgroundBrush(QColor(179,179,179));

    //addEllipse( 0, 0, 10,0 10 );

    // addRect(0,0,750,750, QPen(QColor(179,179,179),1,Qt::SolidLine), Qt::white);
    addRect(0,0,sceneRect().width(),sceneRect().height(), QPen(QColor(179,179,179),1,Qt::SolidLine), Qt::white);
    addItem ( &sketch );

    setFocus();

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
    status = Interaction::DEFAULT;
    sketch.interactionString = "Default Mode";

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

    std::cout << "# of Paths: " << svgPaths.size() << std::endl;

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

    if ( event->buttons() == Qt::LeftButton && status != Interaction::MOVE_ZOOM ){
        if (imageItem == nullptr){

        } else {
            if (imageItem->isSelected())
                imageItem->setSelected(false);
        }
    }


    if (event->buttons() == Qt::LeftButton &&  (status == Interaction::CROSS_SELECTION || status == Interaction::CROP_SELECTION || status == Interaction::ERASE_SELECTION)){
        setFocus();
        leftButtonIsPressed = true;
        sketch.createSelectionCurve( pos );
        //update();
    }

    if (event->buttons() == Qt::LeftButton && status == Interaction::OVERSKETCHING){
        setFocus();
        leftButtonIsPressed = true;
        sketch.createOversketchingCurve( pos );
        update();
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
        leftButtonIsPressed = true;
        sketch.createStripeContour( pos );
    }

    if ( event->buttons() == Qt::LeftButton && status == Interaction::MOVE_ZOOM && imageItem->isUnderMouse()){
        if (imageItem->isVisible()){
            imageItem->setSelected(true);
            startMovePos = pos;
        }

    } else if ( event->buttons() == Qt::LeftButton && status == Interaction::MOVE_ZOOM && !(imageItem->isUnderMouse())){
        if (imageItem->isVisible()){
            imageItem->setSelected(false);
        }
    }

    update();

}


void Scene::mouseMoveEvent(QGraphicsSceneMouseEvent *event){

    setFocus();
    QPointF pos = event->scenePos();


    if (status == Interaction::DEFAULT){

    }
    if (event->buttons() == Qt::LeftButton && (status == Interaction::CROSS_SELECTION || status == Interaction::CROP_SELECTION || status == Interaction::ERASE_SELECTION)){
        sketch.addSelectionCurve( pos );
        QGraphicsScene::mouseMoveEvent(event);
    }
    if (event->buttons() == Qt::LeftButton && status == Interaction::OVERSKETCHING){
        sketch.addOversketchingCurve( pos );
        QGraphicsScene::mouseMoveEvent(event);
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
        if(imageItem->isVisible()){
            imageItem->moveBy(pos.x() - startMovePos.x(), pos.y() - startMovePos.y() );
            startMovePos = pos;
            update();
            QGraphicsScene::mouseMoveEvent(event);
        }
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
    if (status == Interaction::MOVE_ZOOM){
        return;
    }

    if (status == Interaction::OVERSKETCHING){

        static bool first_run = true;

        //bool sketch_has_changed = sketch.joinPaths();

        sketch.joinPaths();


        update ();
        QGraphicsScene::mouseReleaseEvent(event);
    }

    if (event->button() & Qt::LeftButton && status == Interaction::OPENCONTOUR){
        if (sketch.saveOpenContour()){
            emit openContourDone();
        }
        leftButtonIsPressed = false;

        QGraphicsScene::mouseReleaseEvent(event);

    }

    if (event->button() & Qt::LeftButton && status == Interaction::CLOSEDCONTOUR){
        leftButtonIsPressed = false;
        if (sketch.saveClosedContour()){
            emit closedContourDone();
        }


        QGraphicsScene::mouseReleaseEvent(event);

    }

    if (event->button() & Qt::LeftButton && status == Interaction::STRIPES){

        if (sketch.saveStripeContour()){
            emit stripeContourDone();
        }
        leftButtonIsPressed = false;
        QGraphicsScene::mouseReleaseEvent(event);

    }

    if (event->button() & Qt::RightButton && status == Interaction::STRIPES){

        sketch.finishBand();
        //emit stripeContourDone();
        emit stripeContourFinished();
        QGraphicsScene::mouseReleaseEvent(event);

    }

    if (event->button() & Qt::LeftButton && status == Interaction::CROSS_SELECTION){
        leftButtonIsPressed = false;
        if (sketch.crossSelection()){
            emit closedContourDone();
        }

    }
    if (event->button() & Qt::LeftButton && status == Interaction::CROP_SELECTION) {

        sketch.cropSelection();
        leftButtonIsPressed = false;
        emit closedContourDone();

    }
    if (event->button() & Qt::LeftButton && status == Interaction::ERASE_SELECTION) {

        leftButtonIsPressed = false;
        if (sketch.eraseSelection() != -1){
            emit eraseSelectionDone();
        }




    }

    update ();
}

void Scene::wheelEvent ( QGraphicsSceneWheelEvent *event )
{

    if (status == Interaction::MOVE_ZOOM && imageItem->isSelected()){

        if (event->delta() > 0 ){

            imageItem->setTransformationMode(Qt::SmoothTransformation);

            imageItem->setScale(imageItem.get()->scale()*1.01);

//            QMatrix m = imageItem->matrix();
//            m.scale (1.01,1.01);
//            imageItem->setMatrix( m );

        } else if (event->delta() < 0 ){

            imageItem->setTransformationMode(Qt::SmoothTransformation);
            imageItem->setScale(imageItem.get()->scale()*0.99);
//            QMatrix m = imageItem->matrix();
//            m.scale (0.99,0.99);
//            imageItem->setMatrix( m );

        }
    }  else if (status == Interaction::STRIPES) {

        if (event->delta() > 0 ){


            if (sketch.twistingThickness < 25){
                sketch.twistingThickness +=1;
                qDebug () << sketch.twistingThickness;
                //sketch.savePath();
            }
        } else if (event->delta() < 0 ){



            if (sketch.twistingThickness > -25){
                sketch.twistingThickness -=1;
                qDebug () << sketch.twistingThickness;
                //sketch.savePath();

            }
        }

    }

    if (status == Interaction::DEFINE_ROT_AXIS_MODE){
        if ( event->delta() > 0 ) {

            sketch.defRotAxis(1);
            update();

        } else if (event->delta() < 0) {

            sketch.defRotAxis(-1);
            update();
        }
    }
    event->accept();

    update ();
}

void Scene::keyPressEvent(QKeyEvent *event){

    setFocus();

    if ( event->key() == Qt::Key_Plus && status == Interaction::OPENCONTOUR ){
        if ( leftButtonIsPressed ){

            if(sketch.increaseOpenContourLevelWhileDrawing()){
            emit openContourDone();
            }

        } else {

            //sketch.saveOpenContour();

            sketch.increaseLevel();

        }
    }

    if ( event->key() == Qt::Key_Minus && status == Interaction::OPENCONTOUR ){

        if (sketch.lineLevel == 1){
            return;
        }

        if ( leftButtonIsPressed ){

            if(sketch.decreaseOpenContourLevelWhileDrawing()){
            emit openContourDone();
            }
        } else {

            //sketch.saveOpenContour();

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

    if ( event->key() == Qt::Key_Plus && (status == Interaction::CROSS_SELECTION || status == Interaction::CROP_SELECTION)  && !leftButtonIsPressed ){
        sketch.increaseLevel();
        qDebug () << "Increase";
    }

    if ( event->key() == Qt::Key_Minus &&  (status == Interaction::CROSS_SELECTION || status == Interaction::CROP_SELECTION) && !leftButtonIsPressed ){

        if (sketch.lineLevel == 1){
            return;
        }
        sketch.decreaseLevel();
    }

    if ( event->key() == Qt::Key_Plus && status == Interaction::STRIPES ){
        qDebug () << leftButtonIsPressed;
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
    emit updateLayerList();
    update();
}

void Scene::changeLayerDifference (const int &difference){

    sketch.changeLayerDifference(difference);

}

void Scene::estimateShapes()
{
    sketch.estimateShapes();
}

QList<QVector<QVector3D>> Scene::getOpenContoursPoints()
{
    return sketch.getOpenContoursPoints();
}

QVector<QVector3D> Scene::getClosedContoursPoints()
{
    return sketch.getClosedContoursPoints();
}

QVector<QVector3D> Scene::getClosedContoursNormals()
{
    return sketch.getClosedContoursNormals();
}

QList<QVector<QVector3D>> Scene::getStripes()
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

    if (status == Interaction::OPENCONTOUR)
        return 1;

    if (status == Interaction::CLOSEDCONTOUR)
        return 2;

    if  (status == Interaction::STRIPES)
        return 3;

    return 0;
}

bool Scene::isOpenContoursEmpty(){

    return sketch.isOpenContoursEmpty();
}

bool Scene::isClosedContoursEmpty(){
    return sketch.isClosedContoursEmpty();
}

bool Scene::isStripeContoursEmpty(){
    return sketch.isStripeContoursEmpty();
}

/// === NEW LAYER IMPLEMENTATION
///
///

QString Scene::getPathNames(){

    return sketch.getPathNames();

}

void Scene::showLabels(bool showLabels)
{
    sketch.setShowLabels (showLabels);

}

void Scene::selectOpenContour(const int openContourIndex)
{
    sketch.selectOpenContour(openContourIndex);
}

void Scene::selectClosedContour(const int closedContourIndex)
{
    sketch.selectClosedContour(closedContourIndex);
}

void Scene::selectStripeContour(const int stripeContourIndex)
{
    sketch.selectStripeContour(stripeContourIndex);
}

void Scene::renameOpenContour(int itemNumber, QString name){
    sketch.renameOpenContour(itemNumber,name);
}

void Scene::renameClosedContour(int itemNumber, QString name){
    sketch.renameClosedContour(itemNumber, name);
}

void Scene::renameStripeContour(int itemNumber, QString name){
    sketch.renameStripeContour(itemNumber,name);
}

void Scene::setOversketchingMode(){

    status = Interaction::OVERSKETCHING;
    sketch.interactionString = "Oversketching Mode";
    update();

}

void Scene::smoothSketch(){

    sketch.smooth();
    update();

}

void Scene::setDefRotAxisMode(){

    if (status == Interaction::DEFINE_ROT_AXIS_MODE){
        status = Interaction::DEFAULT;
        sketch.interactionString = "Define Rotation Axis Mode";
        qDebug () << "Selection  Mode";
    } else {
        status = Interaction::DEFINE_ROT_AXIS_MODE;
        sketch.interactionString = "Define Rotation Axis Mode";
    }

    qDebug () << "Define Rot Axis Mode";
    update();
}


void Scene::saveSvg(QString path)
{
    QSvgGenerator generator;
    generator.setFileName(path);
    generator.setSize(QSize(520, 520));
    generator.setViewBox(QRect(-30,0,520,520));
    generator.setTitle("SVG SketchCanvas");
    generator.setDescription("An SVG drawing created by the SVG Generator QT.");

    QPainter painter;
    sketch.setSaveSVG();
    painter.begin(&generator);
    sketch.paint(&painter,nullptr,nullptr);
    painter.end();

}

int Scene::getErasedTypeContour(){

    return (sketch.getErasedTypeContour());
}

int Scene::getErasedOpenItemNumber(){

    return (sketch.getErasedOpenContourNumber());
}

int Scene::getErasedClosedItemNumber(){

    return (sketch.getErasedClosedContourNumber());
}

int Scene::getErasedStripeItemNumber(){

    return (sketch.getErasedStripeContourNumber());
}

QList<QString> Scene::getLayerList() {

    return sketch.getLayerList();
}

void Scene::setDepths (int row, QString depth){
    sketch.setDepths(row,depth);
}


#include "moc_scene.cpp"
