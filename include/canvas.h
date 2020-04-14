#ifndef CANVAS_H
#define CANVAS_H


// Necessary for std::shared_ptr<T> and std::make_shared<T>
#include <memory>

#include <openglmediator.h>
#include <layers.h>

#include <QGraphicsView>
#include <QColorDialog>
#include <QProcess>

#include "scene.h"

class Layers;
class OpenGLMediator;

class Canvas: public QGraphicsView
{
    Q_OBJECT

public:
        Canvas();

        // tell compiler that it is ok to use a default destructor
        ~Canvas() = default;

        void setLayers( Layers* ly );

        void setOpenGlMediator(OpenGLMediator *glmed);

protected:

        void createActions();


    public slots:

        void newFile();
        void loadSVG();
        void loadIMG();
        void saveSVG();

        void moveZoomInteraction();

        void setOverlapEffect1(bool checked);
        void setOverlapEffect2(bool checked);
        void setOverlapEffect3(bool checked);
        void setOverlapEffect4(bool checked);

        void setOpenContourInteraction();
        void setClosedContourInteraction();
        void setStripesInteraction();

        void crossSelection();
        void cropSelection();
        void eraseSelection();

        void sendCrossSelectionCurves();
        void sendCropSelectionCurves();

        void sendOpenPathNames();
        void sendClosedPathNames();
        void sendStripePathNames();

        void viewOverlapping3D();


        void exportMesh();

        void showLabels (const int checked);

        //void viewOpenContour3D();

        void changeLayerDifference(const int &difference);

        void selectOpenContour (const int openContourIndex);
        void selectClosedContour (const int closedContourIndex);
        void selectStripeContour (const int stripeContourIndex);

        void setOversketchingMode ();
        void smoothSketch ();
        void setDefRotAxisMode ();

        void renameOpenContour(int itemNumber, QString name);
        void renameClosedContour(int itemNumber, QString name);
        void renameStripeContour(int itemNumber, QString name);


        void updateStripeContourList();

        void removeItemLayersList();

        void updateLayerList();
protected:

        std::shared_ptr< Scene > scene;
        Layers* layers = nullptr;
        OpenGLMediator* glmediator = nullptr;

private:

        bool contour = true, halo = false, color = false, hatching = false;


        QString path;
signals:
        void updateLayerListSignal();
};

#endif // CANVAS_H
