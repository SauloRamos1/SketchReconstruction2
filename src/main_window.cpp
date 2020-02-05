#include "include/main_window.h"

#include <QtWidgets>


MainWindow::MainWindow( QWidget* parent ): QMainWindow( parent )
{

    QFile file(":/qss/darkorange.qss");
    file.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(file.readAll());
    setStyleSheet(styleSheet);
    ensurePolished();

    setWindowIcon(QIcon(":/icons/windowicon.png"));

    setAcceptDrops (true);
    canvas = std::make_shared< Canvas >();
    //setCentralWidget( canvas.get() );

    canvas2 = std::make_shared < Canvas2> ();
    //setCentralWidget( canvas2.get() );

    layers = std::make_shared < Layers > ();

    canvas->setMediator( &mediator );
    canvas2->setMediator( &mediator );
    layers->setMediator( &mediator );


    mediator.setCanvas2( canvas2 );
    mediator.setLayers( layers );


    glcanvas = std::make_shared <OpenGLCanvas> () ;
    glcanvas->setMinimumSize( 250, 500);

    glcanvas->setOpenGLMediator(&openglmediator);
    canvas->setOpenGlMediator(&openglmediator);
    canvas2->setOpenGLMediator(&openglmediator);

    openglmediator.setGlCanvas(glcanvas);
    openglmediator.setCanvas(canvas);
    openglmediator.setCanvas2(canvas2);
    ///

    dw_sketch = new QDockWidget ( " Sketch Segmentation", this);
    QMainWindow *canvas_window = new QMainWindow(dw_sketch);

    //canvas1_toolbar = addToolBar("Default action");
    //canvas_window->addToolBar(canvas1_toolbar);
    //canvas1_toolbar->setParent(canvas.get());

    canvas_window->setParent(dw_sketch);

    dw_sketch->setWidget(canvas.get());
    dw_sketch->setMinimumSize( 550, 700);
    ///


    seg_sketch = new QDockWidget ( " Reconstruction Contours", this);
    QMainWindow *canvas2_window = new QMainWindow(seg_sketch);

    //    canvas2_toolbar = addToolBar("Default action");
    //    canvas2_window->addToolBar(canvas2_toolbar);
    //    canvas2_toolbar->setParent(canvas2.get());

    canvas2_window->setParent(seg_sketch);

    seg_sketch->setWidget( canvas2.get() );
    seg_sketch->setMinimumSize( 550, 700);

    ///
    ///
    ///
    layers_dock = new QDockWidget( " Layers", this);

    QMainWindow *layers_window = new QMainWindow(layers_dock);

    //    layers_toolbar = addToolBar("Default action");
    //    layers_window->addToolBar(layers_toolbar);
    //    layers_toolbar->setParent(layers.get());

    layers_window->setParent(layers_dock);

    layers_dock->setWidget(layers.get());
    layers_dock->setMinimumSize( 150, 600);
    //layers_dock->setMaximumWidth(150);

    layers->listWidget->resize(layers_dock->width(), layers_dock->height()-50);
    //layers->listWidget->move(0,200);
    ///


    addDockWidget (Qt::LeftDockWidgetArea, dw_sketch) ;
    addDockWidget (Qt::LeftDockWidgetArea, seg_sketch) ;
    addDockWidget(Qt::LeftDockWidgetArea, layers_dock);


    splitDockWidget( dw_sketch, seg_sketch, Qt::Horizontal);
    splitDockWidget( seg_sketch, layers_dock, Qt::Horizontal);

    setCentralWidget( glcanvas.get() );

    createCanvas1Toolbar();
    createCanvas2Toolbar();

    createLayersDockToolbar();

    createCanvas1Actions();
    createCanvas2Actions();

    createLayersDockActions();

    //dw_sketch->setWidget(window);

}

void MainWindow::createCanvas1Toolbar () {

    // Canvas 1

    //    ac_movezoom = std::make_shared< QAction >( "Move / Zoom", this );
    //    ac_selectCross_Selection = std::make_shared< QAction >( "Cross Selection", this );
    //    ac_selectCrop_Selection = std::make_shared< QAction >( "Brush Selection", this );
    //    ac_selectErase_Selection = std::make_shared< QAction >( "Erase", this );

    //ac_movezoom->setIcon(QIcon(":/icons/move.svg"));
    //ac_selectCross_Selection->setIcon(QIcon(":/icons/pencil.svg"));
    //ac_selectCrop_Selection->setIcon(QIcon(":/icons/edit.svg"));
    //ac_selectErase_Selection->setIcon(QIcon(":/icons/eraser.svg"));

    //canvas1_Menu = new QMenuBar (canvas1_toolbar) ;

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
    fileMenuButton->setGeometry(QRect(15, 10, 70, 70));
    fileMenuButton->setIconSize(fileMenuButton->size()*0.85);

    fileMenuButton->setMenu(fileMenu);

    movezoom_button = new QPushButton( canvas.get());
    movezoom_button->setToolTip("Move / Zoom");
    movezoom_button->setGeometry(QRect(15, 90, 70, 70));
    movezoom_button->setIcon(QIcon(":/icons/move_white.svg"));
    movezoom_button->setIconSize(movezoom_button->size()*0.8);

    openContour_button = new QPushButton( "Open Contour" , canvas.get());
    openContour_button->setToolTip("Open Contour");
    openContour_button->setGeometry(QRect(15, 170, 70, 70));
    openContour_button->setCheckable(true);

    closedContour_button = new QRadioButton( "Closed Contour" , canvas.get());
    closedContour_button->setToolTip("Closed Contour");
    closedContour_button->setGeometry(260, 25, 130, 20);

    stripe_button = new QRadioButton( "Stripe" , canvas.get());
    stripe_button->setToolTip("Stripe");
    stripe_button->setGeometry(260, 45, 130, 20);

    selectContour_Group = new QButtonGroup (canvas.get());
    selectContour_Group->setExclusive(true);
    selectContour_Group->addButton(openContour_button);
    selectContour_Group->addButton(closedContour_button);
    selectContour_Group->addButton(stripe_button);

    view3dLines_button = new QPushButton (" 3D ", canvas.get());
    view3dLines_button->setToolTip("View 3D");
    view3dLines_button->setGeometry(260, 65, 130, 20);
    //view3dLines_button->setIcon(QIcon(":/icons/pencil.svg"));


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

    //selectOverlapEffect_Menu->addAction(ac_selectOverlapEffect5.get()); //Twisting

    openContourEffect_MenuButton = new QPushButton(canvas.get());
    openContourEffect_MenuButton->setToolTip("Select Overlap Effect");
    openContourEffect_MenuButton->setGeometry(380, 5, 30, 20);
    openContourEffect_MenuButton->setMenu(openContourEffect_Menu);

    closedContourEffect_Menu = new QMenu("&tr");

    closedContourEffect_Menu->addAction(ac_selectOverlapEffect1.get());
    closedContourEffect_Menu->addAction(ac_selectOverlapEffect2.get());
    closedContourEffect_Menu->addAction(ac_selectOverlapEffect3.get());
    closedContourEffect_Menu->addAction(ac_selectOverlapEffect4.get());

    //selectOverlapEffect_Menu->addAction(ac_selectOverlapEffect5.get()); //Twisting

    closedContourEffect_MenuButton = new QPushButton(canvas.get());
    closedContourEffect_MenuButton->setToolTip("Select Overlap Effect");
    closedContourEffect_MenuButton->setGeometry(380, 25, 30, 20);
    closedContourEffect_MenuButton->setMenu(closedContourEffect_Menu);

    //selectOverlappingEffect_Button->setIcon(QIcon(":/icons/palette.svg"));

    selectCross_button = new QPushButton( "Cross" , canvas.get());
    selectCross_button->setToolTip("Cross Selection");
    selectCross_button->setGeometry(420, 5, 100, 20);
    selectCross_button->setIcon(QIcon(":/icons/cross.svg"));

    selectCrop_button = new QPushButton( "Brush" , canvas.get());
    selectCrop_button->setToolTip("Brush Selection");
    selectCrop_button->setGeometry(420, 25, 100, 20);
    selectCrop_button->setIcon(QIcon(":/icons/edit.svg"));

    selectErase_button  = new QPushButton( "Erase" , canvas.get());
    selectErase_button->setToolTip("Erase");
    selectErase_button->setGeometry(420, 45, 100, 20);
    selectErase_button->setIcon(QIcon(":/icons/eraser.svg"));


    //canvas1_toolbar->addAction(ac_movezoom.get());
    //canvas1_toolbar->addAction(ac_selectCross_Selection.get());
    //canvas1_toolbar->addAction(ac_selectCrop_Selection.get());
    //canvas1_toolbar->addAction(ac_selectErase_Selection.get());

    //canvas1_toolbar->setFixedSize(canvas->width()-1, 50);
    //canvas1_toolbar->addWidget(canvas1_Menu);

}


void MainWindow::createCanvas1Actions (){


    connect( ac_newFile.get(), &QAction::triggered, canvas.get(), &Canvas::newFile);
    connect( ac_loadSVG.get(), &QAction::triggered, canvas.get(), &Canvas::loadSVG);
    connect( ac_loadIMG.get(), &QAction::triggered, canvas.get(), &Canvas::loadIMG);

    //    connect( ac_loadSVG.get(), &QAction::triggered, canvas.get(), SLOT(loadFile("SVG(*.svg") ));
    //    connect( ac_loadIMG.get(), &QAction::triggered, canvas.get(), SLOT(loadFile("JPEG (*.jpg *.jpeg);;PNG (*.png);; BMP (*.bmp)") ));

    connect( openContour_button, SIGNAL(clicked(bool)), this, SLOT(openContourButtonClicked()));

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

    //    connect( ac_selectCross_Selection.get(), &QAction::triggered, canvas.get(), &Canvas::crossSelection );
    //    connect( ac_selectCrop_Selection.get(), &QAction::triggered, canvas.get(), &Canvas::cropSelection );
    //    connect( ac_selectErase_Selection.get(), &QAction::triggered, canvas.get(), &Canvas::eraseSelection );

}

void MainWindow::openContourButtonClicked(){

    canvas.get()->setOpenContourInteraction();
    checked = !checked;
    if (checked){
        openContour_button->setChecked(true);
    } else {
        openContour_button->setChecked(false);
    }
}

void MainWindow::createCanvas2Toolbar(){

    // Canvas2

    ac_oversketch = std::make_shared< QAction >( "Oversketch", this );

    ac_smooth = std::make_shared< QAction >( "Smooth", this );

    ac_defRotAxis = std::make_shared< QAction >( "Define Medial Axis", this );
    ac_crossSecBlendSurface = std::make_shared< QAction > ("Edit Cross Sectional Blending Surface", this);

    ac_estimateShape = std::make_shared< QAction >( "Estimate Shapes", this );
    ac_showNormals = std::make_shared< QAction >( "Show Normals", this );

    ac_exportFiles = std::make_shared< QAction >( "Export Files", this );
    ac_view3DPoints = std::make_shared< QAction >( "View 3D Points", this );


    modeMenu = new QMenu("Mode Menu");
    modeMenu->addAction(ac_oversketch.get());
    modeMenu->addAction(ac_smooth.get());
    modeMenu->addAction(ac_defRotAxis.get());
    modeMenu->addAction(ac_crossSecBlendSurface.get());

    modeMenuButton = new QPushButton("Mode Menu" , canvas2.get());
    modeMenuButton->setToolTip("Select Mode");
    modeMenuButton->setGeometry(50, 20, 100, 30);
    modeMenuButton->setMenu(modeMenu);

    normalsMenu = new QMenu("Normals Menu");
    normalsMenu->addAction(ac_estimateShape.get());
    normalsMenu->addAction(ac_showNormals.get());

    normalsMenuButton = new QPushButton("Normals Menu" , canvas2.get());
    normalsMenuButton->setToolTip("Normals");
    normalsMenuButton->setGeometry(160, 20, 100, 30);
    normalsMenuButton->setMenu(normalsMenu);

    exportMesh_button = new QPushButton("Export Mesh" , canvas2.get());
    exportMesh_button->setToolTip("Export OFF/PLY/NPTS");
    exportMesh_button->setGeometry(270, 20, 100, 30);

    view3DPoints_button = new QPushButton("View 3D Points" , canvas2.get());
    view3DPoints_button->setToolTip("View 3D Points");
    view3DPoints_button->setGeometry(380, 20, 100, 30);

    //    canvas2_Menu->addAction(ac_exportFiles.get());

    //    canvas2_Menu->addAction(ac_view3DPoints.get());


    //    //canvas2_Menu->setGeometry(0,0, canvas2->width()-1, 50);
    //    canvas2_toolbar->setFixedSize(canvas2->width()-1, 50);
    //    canvas2_toolbar->addWidget(canvas2_Menu);


}

void MainWindow::createCanvas2Actions (){

    connect( ac_smooth.get(), &QAction::triggered, canvas2.get(), &Canvas2::smoothSketch );

    connect( ac_oversketch.get(), &QAction::triggered, canvas2.get(), &Canvas2::setOversketchingMode);
    connect( ac_defRotAxis.get(), &QAction::triggered, canvas2.get(), &Canvas2::setDefRotAxisMode);
    connect( ac_crossSecBlendSurface.get(), &QAction::triggered, canvas2.get(), &Canvas2::setCrossSecBlendSurfaceMode);

    connect( ac_estimateShape.get(), &QAction::triggered, canvas2.get(), &Canvas2::estimateShape );
    connect( ac_showNormals.get(), &QAction::toggled, canvas2.get(), &Canvas2::showNormals );

    connect( exportMesh_button, SIGNAL(pressed()), canvas2.get(), SLOT(exportMesh()) );
    connect( view3DPoints_button, SIGNAL(pressed()), canvas2.get(), SLOT(view3DPoints()) );


}

void MainWindow::createLayersDockToolbar(){

    //    ac_layerup = std::make_shared< QAction >( "Layer Up", this );
    //    ac_layerup->setIcon(QIcon(":/icons/arrow_up.svg"));
    //    ac_layerdown = std::make_shared< QAction >( "Layer Down", this );
    //    ac_layerdown->setIcon(QIcon(":/icons/arrow_down.svg"));


    //layers_Menu = new QMenuBar (layers_toolbar) ;

    layerUp_button = new QPushButton("Up" , layers.get());
    layerUp_button->setToolTip("Move UP");
    layerUp_button->setGeometry(10, 20, 60, 30);
    layerUp_button->setIcon(QIcon(":/icons/arrow_up.svg"));

    layerDown_button = new QPushButton("Down" , layers.get());
    layerDown_button->setToolTip("Move Down");
    layerDown_button->setGeometry(80, 20, 60, 30);
    layerDown_button->setIcon(QIcon(":/icons/arrow_down.svg"));

    //    layers_toolbar->addAction(ac_layerup.get());
    //    layers_toolbar->addAction(ac_layerdown.get());

    //    layers_toolbar->setFixedSize(layers->width()-1, 50);
    //layers_toolbar->addWidget(layers_Menu);


}

void MainWindow::createLayersDockActions(){

    connect( layerUp_button, SIGNAL(pressed()), layers.get(), SLOT(layerUpSelectedCurve()) );
    connect( layerDown_button, SIGNAL(pressed()),  layers.get(), SLOT(layerDownSelectedCurve()));

}

void MainWindow::resizeEvent(QResizeEvent *)
{

    layers->listWidget->resize(layers_dock->size().width(), layers_dock->size().height()-70);

    layers->listWidget->move(0,70);

}



#include "moc_main_window.cpp"

