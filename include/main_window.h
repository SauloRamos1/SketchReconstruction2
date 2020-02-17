#ifndef MAINWINDOW_H
#define MAINWINDOW_H


// Necessary for std::shared_ptr<T> and std::make_shared<T>
#include <memory>

#include <QMainWindow>
#include <QToolBar>
#include <QAction>
#include <QDockWidget>
#include <QPushButton>
#include <QSpinBox>

#include "canvas.h"
#include "openglcanvas.h"

//#include "canvas2.h"


//#include "canvasmediator.h"
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
        void createLayersDockToolbar();
        void createCanvas1Actions();
        void createLayersDockActions();

    private:


        //MainWindow

        QDockWidget* dw_sketch; //Coloca o Sketch canvas dentro do QDockWidget.
        QDockWidget* layers_dock;

        //Canvas 1

        std::shared_ptr< Canvas > canvas;
        QFrame *toolbox;

        QMenu *fileMenu;
        QPushButton* fileMenuButton;

        std::shared_ptr< QAction > ac_newFile;
        std::shared_ptr< QAction > ac_loadSVG;
        std::shared_ptr< QAction > ac_loadIMG;
        std::shared_ptr< QAction > ac_exit;

        QPushButton *movezoom_button;

        QPushButton *openContour_button;
        QPushButton *closedContour_button;
        QPushButton *stripe_button;

        QMenu *openContourEffect_Menu;
        QPushButton* openContourEffect_MenuButton;

        QMenu *closedContourEffect_Menu;
        QPushButton* closedContourEffect_MenuButton;

        std::shared_ptr< QAction > ac_selectOverlapEffect1;
        std::shared_ptr< QAction > ac_selectOverlapEffect2;
        std::shared_ptr< QAction > ac_selectOverlapEffect3;
        std::shared_ptr< QAction > ac_selectOverlapEffect4;
        std::shared_ptr< QAction > ac_selectOverlapEffect5;

        QPushButton *view3dLines_button;

        std::shared_ptr< QAction > ac_showNormals;

        QPushButton *selectCross_button;
        QPushButton *selectCrop_button;
        QPushButton *selectErase_button;

        QSpinBox *layeringDepth;
        QLabel *depth;

        QCheckBox *checkbox;

        // Canvas Mediator

        //CanvasMediator mediator;

        QPushButton* modeMenuButton;
        QPushButton* normalsMenuButton;

        QPushButton *exportMesh_button;
        QPushButton *view3DPoints_button;

        std::shared_ptr< QAction > ac_view3DPoints;


        // OpenGL Mediator

        OpenGLMediator openglmediator;

        std::shared_ptr< QAction > ac_exportFiles; //Export npts, off and ply files

        //OpenGL / GLcanvas

        std::shared_ptr< OpenGLCanvas > glcanvas;

        //Layers Dock

        std::shared_ptr < Layers > layers;

        QPushButton *layerUp_button;
        QPushButton *layerDown_button;

        QMenu *modeMenu;

        std::shared_ptr< QAction > ac_oversketch;
        std::shared_ptr< QAction > ac_smooth;

        std::shared_ptr< QAction > ac_defRotAxis;
        std::shared_ptr< QAction > ac_crossSecBlendSurface;




    };

    #endif // MAINWINDOW_H
