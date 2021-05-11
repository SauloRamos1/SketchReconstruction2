#include "include/canvas.h"

Canvas::Canvas()
{
    scene = std::make_shared< Scene >();

    setScene( scene.get() );

    setMouseTracking( true );
    setInteractive( true );
    setAcceptDrops( true );

    createActions();
}

void Canvas::setLayers( Layers *ly ){

    layers = ly;
}


void Canvas::setOpenGlMediator (OpenGLMediator *glmed){

    glmediator = glmed;

}

void Canvas::createActions()
{
    //    connect( scene.get(), &Scene::crossSelectionDone, this, &Canvas::sendCrossSelectionCurves );
    //    connect( scene.get(), &Scene::cropSelectionDone, this, &Canvas::sendCropSelectionCurves );

    connect( scene.get(), &Scene::openContourDone, this, &Canvas::sendOpenPathNames);
    connect( scene.get(), &Scene::closedContourDone, this, &Canvas::sendClosedPathNames);
    connect( scene.get(), &Scene::stripeContourDone, this, &Canvas::sendStripePathNames);

    connect (scene.get(), &Scene::stripeContourFinished, this, &Canvas::updateStripeContourList);


    connect (scene.get(), &Scene::eraseSelectionDone, this, &Canvas::removeItemLayersList);

    connect (scene.get(), &Scene::updateLayerList,this, &Canvas::updateLayerList );
}

void Canvas::newFile()
{
    //scene->newFile();

    QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
    qApp->quit();
}

void Canvas::loadSVG()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),"/", tr("Scalable Vector Graphics(*.svg)"));

    std::string str = fileName.toStdString();

    if (fileName.isEmpty()) return;
    scene->loadSVG(str);
    update();
}

void Canvas::loadIMG()
{

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),"/", tr("Images (*.png *.jpg)"));

    if (fileName.isEmpty()) return;
    scene->loadIMG(fileName);
    update();
}

void Canvas::saveSVG(){

    QString newPath = QFileDialog::getSaveFileName(this, tr("Save SVG"), path, tr("SVG files (*.svg)"));

    if (newPath.isEmpty())
        return;

    path = newPath;

    scene->saveSvg(path);
}



void Canvas::setOverlapEffect1(bool checked)
{
    contour = checked;
    scene->setOverlapEffect(contour, halo, color, hatching);
    update();
}

void Canvas::setOverlapEffect2(bool checked)
{
    halo = checked;
    scene->setOverlapEffect(contour, halo, color, hatching);
    update();
}

void Canvas::setOverlapEffect3(bool checked)
{
    color = checked;
    scene->setOverlapEffect(contour, halo, color, hatching);
    update();
}

void Canvas::setOverlapEffect4(bool checked)
{
    hatching = checked;
    scene->setOverlapEffect(contour, halo, color, hatching);
    update();
}

void Canvas::moveZoomInteraction(){
    //this->setCursor(Qt::OpenHandCursor);
    scene->chooseMoveZoom_Interaction();
    update();
}

void Canvas::setOpenContourInteraction(){
//    if (scene->getInteraction() == 1){
//        scene->chooseDefaultInteraction();

//        return;
//    }

    scene->chooseOpenContour_Interaction();
    update();
}

void Canvas::setClosedContourInteraction(){
    scene->chooseClosedContour_Interaction();
    update();
}

void Canvas::setStripesInteraction(){
    scene->chooseStripes_Interaction();
    update();
}

void Canvas::crossSelection(){
    //this->setCursor(Qt::CrossCursor);
    scene->chooseCross_Selection();
    update();
}

void Canvas::cropSelection(){
    //this->setCursor(Qt::CrossCursor);
    scene->chooseCrop_Selection();
    update();
}

void Canvas::eraseSelection(){
    //this->setCursor(Qt::CrossCursor);
    scene->chooseErase_Selection();
    update();
}

void Canvas::sendOpenPathNames(){

    layers->receiveNamePaths(scene->getPathNames(), 0);

}

void Canvas::sendClosedPathNames(){

    layers->receiveNamePaths(scene->getPathNames(), 1);

}

void Canvas::sendStripePathNames(){

    layers->receiveNamePaths(scene->getPathNames(), 2);
    update();

}

void Canvas::renameOpenContour(int itemNumber, QString name){
    scene->renameOpenContour(itemNumber,name);
    update();
}
void Canvas::renameClosedContour(int itemNumber, QString name){
    scene->renameClosedContour(itemNumber, name);
    update();
}
void Canvas::renameStripeContour(int itemNumber, QString name){
    scene->renameStripeContour(itemNumber, name);
    update();
}

void Canvas::updateStripeContourList(){
    layers->clearStripeList();
    layers->receiveNamePaths(scene->getPathNames(), 2);
}

void Canvas::removeItemLayersList()
{
    if (scene->getErasedTypeContour() == 0){

        layers->removeItemOpenContourList(scene->getErasedOpenItemNumber());

    }

    if (scene->getErasedTypeContour() == 1){

        layers->removeItemClosedContourList(scene->getErasedClosedItemNumber());

    }

    if (scene->getErasedTypeContour() == 2){

        layers->removeItemStripeContourList(scene->getErasedStripeItemNumber());

    }
}

void Canvas::sendCrossSelectionCurves()
{
    //    if ( mediator == nullptr ) return;
    //    if ( scene->crossSelection().isEmpty()) return;

    //    mediator->sendClosedPaths( scene->crossSelection(), scene->SelectionLineLevel() );

}

void Canvas::sendCropSelectionCurves(){

    //    if ( mediator == nullptr ) return;
    //    if ( scene->cropSelection().isEmpty()) return;

    //    mediator->closeAndSendPaths( scene->cropSelection(), scene->SelectionLineLevel() );
}


//void Canvas::viewOpenContour3D(){

//     glmediator->viewOpenContours3D(scene->getOpenContoursPoints());

//}

void Canvas::viewOverlapping3D(){

    if (glmediator == nullptr) return;

    glmediator->clearTriangles();

    if (!scene->isOpenContoursEmpty()){

        glmediator->viewOpenContours3D(scene->getOpenContoursPoints());

    }

    if (!scene->isClosedContoursEmpty()){


        scene->estimateShapes();
        glmediator->viewClosedContours3D(scene->getClosedContoursPoints(), scene->getClosedContoursNormals());

    }

    if (!scene->isStripeContoursEmpty()){

        glmediator->viewStripes3D(scene->getStripes());

    }

//    if (scene->getInteraction() == 1){
//        glmediator->viewOpenContours3D(scene->getOpenContoursPoints());

//    } else if (scene->getInteraction() == 2){

//        scene->estimateShapes();
//        glmediator->viewClosedContours3D(scene->getClosedContoursPoints(), scene->getClosedContoursNormals());
//        //glmediator->render();

//    } else if (scene->getInteraction() == 3){
//        glmediator->viewStripes3D(scene->getStripes());
//        //glmediator->render();
//    }
    glmediator->render();
}

void Canvas::exportView(){

    glmediator->exportView();

}

void Canvas::exportMesh(){

    if (glmediator == nullptr) return;

    if (!scene->isOpenContoursEmpty()){

        glmediator->exportOpenContours3D(scene->getOpenContoursPoints());

    }

    if (!scene->isClosedContoursEmpty()){

        //glmediator->exportClosedContours3D(scene->getClosedContoursPoints(), scene->getClosedContoursNormals());
        //glmediator->exportRBFMesh(scene->getRbfDataFiles());
        qDebug() << scene->getRbfDataFiles() ;

    }

    if (!scene->isStripeContoursEmpty()){

        glmediator->exportStripes3D(scene->getStripes());

    }



}

void Canvas::showLabels(const int checked)
{
    if (checked == 0){
        scene->showLabels(false);
    } else {
        scene->showLabels(true);
    }
    update();
}

void Canvas::changeLayerDifference(const int &difference)
{
    scene->changeLayerDifference (difference);
    update();
}

void Canvas::selectOpenContour(const int openContourIndex)
{
    scene->chooseOpenContour_Interaction();
    scene->selectOpenContour(openContourIndex);
    update();
}

void Canvas::selectClosedContour(const int closedContourIndex)
{
    scene->chooseClosedContour_Interaction();
    scene->selectClosedContour(closedContourIndex);
    update();
}

void Canvas::selectStripeContour(const int stripeContourIndex)
{
    scene->chooseStripes_Interaction();
    scene->selectStripeContour(stripeContourIndex);
    update();
}

void Canvas::setOversketchingMode (){
    scene->setOversketchingMode();
    update();
}

void Canvas::smoothSketch (){
    scene->smoothSketch();
    update();
}

void Canvas::setDefRotAxisMode (){
    scene->setDefRotAxisMode();
    update();
}

void Canvas::updateLayerList(){

    emit updateLayerListSignal();

}
QList<QString> Canvas::getLayerList(){
    return scene->getLayerList();
}

void Canvas::setDepths(int row, QString depth)
{
    scene->setDepths(row, depth);

}



#include "moc_canvas.cpp"
