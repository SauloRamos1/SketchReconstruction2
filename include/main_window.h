#ifndef MAINWINDOW_H
#define MAINWINDOW_H


// Necessary for std::shared_ptr<T> and std::make_shared<T>
#include <memory>

#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <QDockWidget>
#include <QPushButton>

#include "canvas.h"
#include "openglcanvas.h"

#include "canvas2.h"

#include "canvasmediator.h"
#include "openglmediator.h"

#include "layers.h"



class MainWindow: public QMainWindow
{

        Q_OBJECT

    public:

        MainWindow( QWidget* parent = nullptr );

        // tell compiler that it is ok to use a default destructor
        ~MainWindow() = default;

        void resizeEvent(QResizeEvent *) override;

        QString mode;

    protected:

        void createCanvas1Toolbar();
        void createCanvas2Toolbar();

        void createLayersDockToolbar();

        void createCanvas1Actions();
        void createCanvas2Actions();

        void createLayersDockActions();

    private:


        //MainWindow

        QDockWidget* dw_sketch; //Coloca o Sketch canvas dentro do QDockWidget.
        QDockWidget* seg_sketch;
        QDockWidget* layers_dock;


        //Canvas 1

        std::shared_ptr< Canvas > canvas;

        //QToolBar* canvas1_toolbar;

        //        std::shared_ptr< QAction > ac_movezoom;
        //        std::shared_ptr< QAction > ac_selectCross_Selection;
        //        std::shared_ptr< QAction > ac_selectCrop_Selection;
        //        std::shared_ptr< QAction > ac_selectErase_Selection;

        QFrame *toolbox;


        QMenu *fileMenu;
        QPushButton* fileMenuButton;


        std::shared_ptr< QAction > ac_newFile;
        std::shared_ptr< QAction > ac_loadSVG;
        std::shared_ptr< QAction > ac_loadIMG;
        std::shared_ptr< QAction > ac_exit;

        QPushButton *movezoom_button;

//        QButtonGroup *selectContour_Group;

        QPushButton *openContour_button;
        QPushButton *closedContour_button;
        QPushButton *stripe_button;


        QMenu *openContourEffect_Menu;
        QPushButton* openContourEffect_MenuButton;

        QMenu *closedContourEffect_Menu;
        QPushButton* closedContourEffect_MenuButton;

        //ac_selectOverlappingEffect1 = Contour
        //ac_selectOverlappingEffect2 = Halo
        //ac_selectOverlappingEffect3 = Hatching
        //ac_selectOverlappingEffect4 = Color
        //ac_selectOverlappingEffect5 = Twisting

        std::shared_ptr< QAction > ac_selectOverlapEffect1;
        std::shared_ptr< QAction > ac_selectOverlapEffect2;
        std::shared_ptr< QAction > ac_selectOverlapEffect3;
        std::shared_ptr< QAction > ac_selectOverlapEffect4;
        std::shared_ptr< QAction > ac_selectOverlapEffect5;

        QPushButton *view3dLines_button;

        QPushButton *selectCross_button;
        QPushButton *selectCrop_button;
        QPushButton *selectErase_button;


        // Canvas Mediator

        CanvasMediator mediator;

        //Canvas 2

        std::shared_ptr< Canvas2 > canvas2;

        QToolBar* canvas2_toolbar;

        QPushButton* modeMenuButton;
        QPushButton* normalsMenuButton;

        QPushButton *exportMesh_button;
        QPushButton *view3DPoints_button;
        //QMenuBar* canvas1_Menu;
        QMenuBar* canvas2_Menu;

        QMenu *modeMenu;
        QMenu *normalsMenu;

        std::shared_ptr< QAction > ac_oversketch;
        std::shared_ptr< QAction > ac_smooth;

        std::shared_ptr< QAction > ac_defRotAxis;
        std::shared_ptr< QAction > ac_crossSecBlendSurface;

        std::shared_ptr< QAction > ac_estimateShape;
        std::shared_ptr< QAction > ac_showNormals;

        std::shared_ptr< QAction > ac_view3DPoints;

        std::shared_ptr< QAction > ac_exportFiles; //Export npts, off and ply files



        //Layers Dock

        std::shared_ptr < Layers > layers;

        //        std::shared_ptr< QAction > ac_layerup;
        //        std::shared_ptr< QAction > ac_layerdown;


        QPushButton *layerUp_button;
        QPushButton *layerDown_button;

        //        QMenuBar* layers_Menu;
        //        QToolBar* layers_toolbar;

        // OpenGL Mediator

        OpenGLMediator openglmediator;


        //OpenGL / GLcanvas

        std::shared_ptr< OpenGLCanvas> glcanvas;




    };

    #endif // MAINWINDOW_H
