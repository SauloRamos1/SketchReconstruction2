#include "include/main_window.h"

#include <QtWidgets>

MainWindow::MainWindow( QWidget* parent ): QMainWindow( parent )
{


//************************************************************************************************
/// ........................................ STYLESHEET ..........................................
//************************************************************************************************


//    QFile file(":/qdarkstyle/style.qss");
//    file.open(QFile::ReadOnly);
//    QString styleSheet = QLatin1String(file.readAll());
//    setStyleSheet(styleSheet);
//    ensurePolished();

    QFile f(":qdarkstyle/style.qss");

    if (!f.exists())   {
        printf("Unable to set stylesheet, file not found\n");
    }
    else   {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        qApp->setStyleSheet(ts.readAll());
    }

    setWindowIcon(QIcon(":/icons/windowicon.png"));

    setAcceptDrops (true);


//************************************************************************************************
/// ........................................ LINKERS ..........................................
//************************************************************************************************


    canvas = std::make_shared< Canvas > ();
    layers = std::make_shared < Layers > ();

    canvas->setLayers(layers.get());
    layers->setCanvas(canvas.get());

//    mediator.setCanvas( canvas );
//    mediator.setLayers( layers );

    glcanvas = std::make_shared < OpenGLCanvas > () ;

    glcanvas->setMinimumSize( 250, 500 );

    glcanvas->setOpenGLMediator( &openglmediator );
    canvas->setOpenGlMediator( &openglmediator );

    openglmediator.setGlCanvas(glcanvas);
    openglmediator.setCanvas(canvas);


//************************************************************************************************
/// ........................................ QDockWidgets ..........................................
//************************************************************************************************

    dw_sketch = new QDockWidget ( " Sketch Segmentation", this);
    QMainWindow *canvas_window = new QMainWindow(dw_sketch);

    canvas_window->setParent(dw_sketch);

    dw_sketch->setWidget(canvas.get());
    dw_sketch->setMinimumSize( 900, 1000);


    layers_dock = new QDockWidget( " Layers", this);

    QMainWindow *layers_window = new QMainWindow(layers_dock);

    layers_window->setParent(layers_dock);
    layers_dock->setWidget(layers.get());
    layers_dock->setMinimumSize( 150, 600);

    layers->listWidget->resize(layers_dock->width(), layers_dock->height()-50);

    addDockWidget (Qt::LeftDockWidgetArea, dw_sketch) ;
    addDockWidget(Qt::LeftDockWidgetArea, layers_dock);


    splitDockWidget( dw_sketch, layers_dock, Qt::Horizontal);

    setCentralWidget( glcanvas.get() );

//************************************************************************************************
/// ........................................ Create Functions ..........................................
//************************************************************************************************

    createCanvas1Toolbar();

    createLayersDockToolbar();

    createCanvas1Actions();

    createLayersDockActions();

}

void MainWindow::createCanvas1Toolbar () {

    // Canvas 1

    toolbox = new QFrame(canvas.get());
    toolbox->setObjectName(QString::fromUtf8("toolbox"));
    toolbox->resize(100, qApp->screens()[0]->size().height());
    toolbox->setFrameShape(QFrame::StyledPanel);
    toolbox->setFrameShadow(QFrame::Raised);


    ac_newFile = std::make_shared< QAction >( "New File", this );
    ac_newFile->setIcon(QIcon(":/icons/newfile.png"));
    ac_loadSVG = std::make_shared< QAction >( "Load SVG", this );
    ac_loadSVG->setIcon(QIcon(":/icons/svgicon.png"));
    ac_loadIMG = std::make_shared< QAction >( "Load Image", this );
    ac_loadIMG->setIcon(QIcon(":/icons/image.png"));
    ac_exit = std::make_shared< QAction > ("Exit", this);


    fileMenu = new QMenu("File");
    fileMenu->addAction(ac_newFile.get());
    fileMenu->addSeparator();
    fileMenu->addAction(ac_loadSVG.get());
    fileMenu->addAction(ac_loadIMG.get());
    fileMenu->addSeparator();
    fileMenu->addAction(QIcon(":/icons/exit.ico"), tr("E&xit"), qApp, &QApplication::closeAllWindows);

    fileMenuButton = new QPushButton( canvas.get());
    fileMenuButton->setToolTip("File");
    fileMenuButton->setIcon(QIcon(":/icons/file_white.png"));
    fileMenuButton->setGeometry(QRect(15, 10, 90, 70));
    fileMenuButton->setIconSize(fileMenuButton->size()*0.85);

    fileMenuButton->setMenu(fileMenu);

    movezoom_button = new QPushButton( canvas.get());
    movezoom_button->setToolTip("Move / Zoom");
    movezoom_button->setGeometry(QRect(15, 90, 90, 70));
    movezoom_button->setIcon(QIcon(":/icons/move_white.svg"));
    movezoom_button->setIconSize(movezoom_button->size()*0.8);

    openContour_button = new QPushButton( "Open \nContour" , canvas.get());
    openContour_button->setToolTip("Open Contour");
    openContour_button->setGeometry(QRect(15, 170, 90, 70));
    //openContour_button->setCheckable(true);

    closedContour_button = new QPushButton( "Closed \nContour" , canvas.get());
    closedContour_button->setToolTip("Closed Contour");
    closedContour_button->setGeometry(QRect(15, 250, 90, 70));

    stripe_button = new QPushButton( "Stripe" , canvas.get());
    stripe_button->setToolTip("Stripe");
    stripe_button->setGeometry(QRect(15, 330, 90, 70));

    ac_selectOverlapEffect1 = std::make_shared< QAction >("Contour", this );
    ac_selectOverlapEffect1.get()->setCheckable(true);
    ac_selectOverlapEffect1.get()->setChecked(true);

    ac_selectOverlapEffect2 = std::make_shared< QAction >("Halo", this );
    ac_selectOverlapEffect2.get()->setCheckable(true);

    ac_selectOverlapEffect3 = std::make_shared< QAction >("Color", this );
    ac_selectOverlapEffect3.get()->setCheckable(true);

    ac_selectOverlapEffect4 = std::make_shared< QAction >("Hatching", this);
    ac_selectOverlapEffect4.get()->setCheckable(true);

    ac_selectOverlapEffect5 = std::make_shared< QAction >("Twisting Tape", this);


    openContourEffect_Menu = new QMenu("&tr");

    openContourEffect_Menu->addAction(ac_selectOverlapEffect1.get());
    openContourEffect_Menu->addAction(ac_selectOverlapEffect2.get());

    openContourEffect_MenuButton = new QPushButton(canvas.get());
    openContourEffect_MenuButton->setToolTip("Select Overlap Effect");
    openContourEffect_MenuButton->setGeometry(QRect(90, 170, 15, 70));
    openContourEffect_MenuButton->setMenu(openContourEffect_Menu);

    closedContourEffect_Menu = new QMenu("&tr");

    closedContourEffect_Menu->addAction(ac_selectOverlapEffect1.get());
    closedContourEffect_Menu->addAction(ac_selectOverlapEffect2.get());
    closedContourEffect_Menu->addAction(ac_selectOverlapEffect3.get());
    closedContourEffect_Menu->addAction(ac_selectOverlapEffect4.get());

    closedContourEffect_MenuButton = new QPushButton(canvas.get());
    closedContourEffect_MenuButton->setToolTip("Select Overlap Effect");
    closedContourEffect_MenuButton->setGeometry(QRect(90, 250, 15, 70));
    closedContourEffect_MenuButton->setMenu(closedContourEffect_Menu);

    selectCross_button = new QPushButton( "Cross" , canvas.get());
    selectCross_button->setToolTip("Cross Selection");
    selectCross_button->setGeometry(QRect(15, 410, 90, 70));
    selectCross_button->setIcon(QIcon(":/icons/cross.svg"));

    selectCrop_button = new QPushButton( "Brush" , canvas.get());
    selectCrop_button->setToolTip("Brush Selection");
    selectCrop_button->setGeometry(QRect(15, 490, 90, 70));
    selectCrop_button->setIcon(QIcon(":/icons/edit.svg"));

    selectErase_button  = new QPushButton( "Erase" , canvas.get());
    selectErase_button->setToolTip("Erase");
    selectErase_button->setGeometry(QRect(15, 570, 90, 70));
    selectErase_button->setIcon(QIcon(":/icons/eraser.svg"));


    ac_oversketch = std::make_shared< QAction >( "Oversketch", this );
    ac_smooth = std::make_shared< QAction >( "Smooth", this );
    ac_defRotAxis = std::make_shared< QAction >( "Define Medial Axis", this );
    ac_crossSecBlendSurface = std::make_shared< QAction > ("Edit Cross Sectional Blending Surface", this);

    //ac_showNormals = std::make_shared< QAction >( "Show Normals", this );

    ac_exportFiles = std::make_shared< QAction >( "Export Files", this );
    ac_view3DPoints = std::make_shared< QAction >( "View 3D Points", this );

    modeMenu = new QMenu("Editing \nMenu");
    modeMenu->addAction(ac_oversketch.get());
    modeMenu->addAction(ac_smooth.get());
    modeMenu->addAction(ac_defRotAxis.get());
    modeMenu->addAction(ac_crossSecBlendSurface.get());

    modeMenuButton = new QPushButton("Editing \nMenu" , canvas.get());
    modeMenuButton->setToolTip("Select Mode");
    modeMenuButton->setGeometry(QRect(15, 650, 90, 70));
    modeMenuButton->setMenu(modeMenu);

    //    normalsMenu = new QMenu("Normals \nMenu");
    //    normalsMenu->addAction(ac_estimateShape.get());
    //    normalsMenu->addAction(ac_showNormals.get());

    exportMesh_button = new QPushButton("Export \nMesh" , canvas.get());
    exportMesh_button->setToolTip("Export OFF & PLY");
    exportMesh_button->setGeometry(QRect(15, 810, 90, 70));


    view3dLines_button = new QPushButton ("View 3D ", canvas.get());
    view3dLines_button->setToolTip("View 3D");
    view3dLines_button->setGeometry(QRect(15, 730, 90, 70));

    depth =  new QLabel ("  Depth Between Layers: ", canvas.get());
    depth->setGeometry(QRect(730, 900, 120, 30));

    layeringDepth = new QSpinBox(canvas.get());
    layeringDepth->setGeometry(QRect(850, 900, 40, 30));
    layeringDepth->setValue(10);
    layeringDepth->setSingleStep(10);

    checkbox = new QCheckBox("Show Labels", canvas.get());
    checkbox->setGeometry(QRect(600, 900, 100, 30));

}


void MainWindow::createCanvas1Actions (){


    connect( ac_newFile.get(), &QAction::triggered, canvas.get(), &Canvas::newFile);
    connect( ac_loadSVG.get(), &QAction::triggered, canvas.get(), &Canvas::loadSVG);
    connect( ac_loadIMG.get(), &QAction::triggered, canvas.get(), &Canvas::loadIMG);

    connect( openContour_button, SIGNAL(pressed()), canvas.get(), SLOT(setOpenContourInteraction()));
    connect( closedContour_button, SIGNAL(pressed()), canvas.get(), SLOT(setClosedContourInteraction() ));
    connect( stripe_button, SIGNAL(pressed()), canvas.get(), SLOT(setStripesInteraction() ));

    connect( view3dLines_button, SIGNAL(pressed()), canvas.get(), SLOT (viewOverlapping3D()));

    connect( ac_selectOverlapEffect1.get(), &QAction::triggered, canvas.get(), &Canvas::setOverlapEffect1);
    connect( ac_selectOverlapEffect2.get(), &QAction::triggered, canvas.get(), &Canvas::setOverlapEffect2);
    connect( ac_selectOverlapEffect3.get(), &QAction::triggered, canvas.get(), &Canvas::setOverlapEffect3);
    connect( ac_selectOverlapEffect4.get(), &QAction::triggered, canvas.get(), &Canvas::setOverlapEffect4);
    connect( ac_selectOverlapEffect5.get(), &QAction::triggered, canvas.get(), &Canvas::setStripesInteraction);


    connect( movezoom_button, SIGNAL(pressed()), canvas.get(), SLOT(moveZoomInteraction() ));
    connect( selectCross_button, SIGNAL(pressed()), canvas.get(), SLOT(crossSelection() ));
    connect( selectCrop_button, SIGNAL(pressed()), canvas.get(), SLOT(cropSelection() ));
    connect( selectErase_button, SIGNAL(pressed()), canvas.get(), SLOT(eraseSelection() ));

    connect( layeringDepth, SIGNAL(valueChanged(const int &)), canvas.get(),  SLOT(changeLayerDifference(const int &)));

    //    connect( ac_selectCross_Selection.get(), &QAction::triggered, canvas.get(), &Canvas::crossSelection );
    //    connect( ac_selectCrop_Selection.get(), &QAction::triggered, canvas.get(), &Canvas::cropSelection );
    //    connect( ac_selectErase_Selection.get(), &QAction::triggered, canvas.get(), &Canvas::eraseSelection );

    /// ----- CANVAS 2 ACTIONS&& TOOLBAR MOVED TO CANVAS 1 -----

    //    connect( ac_oversketch.get(), &QAction::triggered, canvas2.get(), &Canvas2::setOversketchingMode);
    //    connect( ac_smooth.get(), &QAction::triggered, canvas2.get(), &Canvas2::smoothSketch );
    //    connect( ac_defRotAxis.get(), &QAction::triggered, canvas2.get(), &Canvas2::setDefRotAxisMode);
    //    connect( ac_crossSecBlendSurface.get(), &QAction::triggered, canvas2.get(), &Canvas2::setCrossSecBlendSurfaceMode);

    //    connect( ac_estimateShape.get(), &QAction::triggered, canvas2.get(), &Canvas2::estimateShape );
    //    connect( ac_showNormals.get(), &QAction::toggled, canvas2.get(), &Canvas2::showNormals );

    connect( exportMesh_button, SIGNAL(pressed()), canvas.get(), SLOT(exportMesh()));

}

void MainWindow::createLayersDockToolbar(){


    layerUp_button = new QPushButton("Up" , layers.get());
    layerUp_button->setToolTip("Move UP");
    layerUp_button->setGeometry(10, 20, 60, 30);
    layerUp_button->setIcon(QIcon(":/icons/arrow_up.svg"));

    layerDown_button = new QPushButton("Down" , layers.get());
    layerDown_button->setToolTip("Move Down");
    layerDown_button->setGeometry(80, 20, 60, 30);
    layerDown_button->setIcon(QIcon(":/icons/arrow_down.svg"));


}

void MainWindow::createLayersDockActions(){

    connect( layerUp_button, SIGNAL(pressed()), layers.get(), SLOT(layerUpSelectedCurve()) );
    connect( layerDown_button, SIGNAL(pressed()),  layers.get(), SLOT(layerDownSelectedCurve()));

}

void MainWindow::resizeEvent(QResizeEvent *){

    layers->listWidget->resize(layers_dock->size().width(), layers_dock->size().height()-70);
    layers->listWidget->move(0,70);

}

#include "moc_main_window.cpp"

