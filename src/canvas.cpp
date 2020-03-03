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
}

void Canvas::newFile()
{
    scene->newFile();
}

void Canvas::loadSVG()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),"/", tr("Scalable Vector Graphics(*.svg)"));

    std::string str = fileName.toStdString();

    if (fileName.isEmpty()) return;
    scene->loadSVG(str);
}

void Canvas::loadIMG()
{

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),"/", tr("Images (*.png *.jpg)"));

    if (fileName.isEmpty()) return;
    scene->loadIMG(fileName);
}



void Canvas::setOverlapEffect1(bool checked)
{
    contour = checked;
    scene->setOverlapEffect(contour, halo, color, hatching);
}

void Canvas::setOverlapEffect2(bool checked)
{
    halo = checked;
    scene->setOverlapEffect(contour, halo, color, hatching);
}

void Canvas::setOverlapEffect3(bool checked)
{
    color = checked;
    scene->setOverlapEffect(contour, halo, color, hatching);
}

void Canvas::setOverlapEffect4(bool checked)
{
    hatching = checked;
    scene->setOverlapEffect(contour, halo, color, hatching);
}

void Canvas::moveZoomInteraction(){
    //this->setCursor(Qt::OpenHandCursor);
    scene->chooseMoveZoom_Interaction();
}

void Canvas::setOpenContourInteraction(){
    if (scene->getInteraction() == 1){
        scene->chooseDefaultInteraction();

        return;
    }

    scene->chooseOpenContour_Interaction();
}

void Canvas::setClosedContourInteraction(){
    scene->chooseClosedContour_Interaction();
}

void Canvas::setStripesInteraction(){
    scene->chooseStripes_Interaction();
}

void Canvas::crossSelection(){
    //this->setCursor(Qt::CrossCursor);
    scene->chooseCross_Selection();
}

void Canvas::cropSelection(){
    //this->setCursor(Qt::CrossCursor);
    scene->chooseCrop_Selection();
}

void Canvas::eraseSelection(){
    //this->setCursor(Qt::CrossCursor);
    scene->chooseErase_Selection();
}

void Canvas::sendOpenPathNames(){

   layers->receiveNamePaths(scene->getPathNames(), 0);

}

void Canvas::sendClosedPathNames(){

   layers->receiveNamePaths(scene->getPathNames(), 1);

}

void Canvas::sendStripePathNames(){

   layers->receiveNamePaths(scene->getPathNames(), 2);

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

    if (scene->getInteraction() == 1){
          glmediator->viewOpenContours3D(scene->getOpenContoursPoints());

    } else if (scene->getInteraction() == 2){

        scene->estimateShapes();
        glmediator->viewClosedContours3D(scene->getClosedContoursPoints(), scene->getClosedContoursNormals());
        //glmediator->render();

    } else if (scene->getInteraction() == 3){
        glmediator->viewStripes3D(scene->getStripes());
        //glmediator->render();
    }
    glmediator->render();
}

void Canvas::exportMesh(){

    glmediator->exportMesh();

}

void Canvas::showLabels(const int checked)
{
    if (checked == 0){
        scene->showLabels(false);
    } else {
        scene->showLabels(true);
    }
}

void Canvas::changeLayerDifference(const int &difference)
{
    scene->changeLayerDifference (difference);
}

void Canvas::selectOpenContour(const int openContourIndex)
{
    scene->chooseOpenContour_Interaction();
    scene->selectOpenContour(openContourIndex);
}

void Canvas::selectClosedContour(const int closedContourIndex)
{
    scene->chooseClosedContour_Interaction();
    scene->selectClosedContour(closedContourIndex);
}

void Canvas::selectStripeContour(const int stripeContourIndex)
{
    scene->chooseStripes_Interaction();
    scene->selectStripeContour(stripeContourIndex);
}

void Canvas::setOversketchingMode (){
    scene->setOversketchingMode();
}

void Canvas::smoothSketch (){
    scene->smoothSketch();
}

void Canvas::setDefRotAxisMode (){
    scene->setDefRotAxisMode();
}


#include "moc_canvas.cpp"
