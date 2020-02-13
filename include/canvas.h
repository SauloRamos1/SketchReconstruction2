#ifndef CANVAS_H
#define CANVAS_H


// Necessary for std::shared_ptr<T> and std::make_shared<T>
#include <memory>

#include <openglmediator.h>

#include <QGraphicsView>
#include <QColorDialog>

#include "scene.h"
#include "include/layers.h"

class Layers;
class OpenGLMediator;

class Canvas: public QGraphicsView
{
    Q_OBJECT

public:
        Canvas();

        // tell compiler that it is ok to use a default destructor
        ~Canvas() = default;

        void linkLayersDock( Layers* layerPointer );

        void setOpenGlMediator(OpenGLMediator *glmed);


protected:

        void createActions();


    public slots:

        void newFile();
        void loadSVG();
        void loadIMG();

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

        void sendPathNames();

        void viewOverlapping3D();


        //void viewOpenContour3D();

        void changeLayerDifference(const int &difference);

protected:

        std::shared_ptr< Scene > scene;
        Layers* canvasLayerPointer = nullptr;
        OpenGLMediator* glmediator = nullptr;

private:

        bool contour = true, halo = false, color = false, hatching = false;

};

#endif // CANVAS_H
