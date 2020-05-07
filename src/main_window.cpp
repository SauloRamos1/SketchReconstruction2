#include "include/main_window.h"

#include <QtWidgets>

/// TODO https://www.dgp.toronto.edu/projects/fast-winding-numbers/
/// TODO Poisson Reconstruction

MainWindow::MainWindow( QWidget* parent ): QMainWindow( parent )
{



    //TODO ->  FIX ALL CODE FOR STRIPES-rendering


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
    int windowWidth = 1280;
    int windowHeight = 720;


    canvas = std::make_shared< Canvas > ();
    layers = std::make_shared < Layers > ();

    canvas->setLayers(layers.get());
    layers->setCanvas(canvas.get());

    //    mediator.setCanvas( canvas );
    //    mediator.setLayers( layers );

    glcanvas = std::make_shared < OpenGLCanvas > () ;

    glcanvas->setMinimumSize(windowWidth*0.3, windowHeight );

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
    dw_sketch->setMinimumSize(windowWidth*0.47, windowHeight);
    //dw_sketch->resize(windowWidth*0.5, windowHeight);

    layers_dock = new QDockWidget( " Layers", this);

    QMainWindow *layers_window = new QMainWindow(layers_dock);

    layers_window->setParent(layers_dock);
    layers_dock->setWidget(layers.get());
    layers_dock->setFixedSize(windowWidth*0.05, windowHeight);


    addDockWidget (Qt::LeftDockWidgetArea, dw_sketch) ;
    addDockWidget(Qt::LeftDockWidgetArea, layers_dock);


    splitDockWidget(layers_dock, dw_sketch, Qt::Horizontal);

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
    toolbox->resize(50, qApp->screens()[0]->size().height());
    toolbox->setFrameShape(QFrame::StyledPanel);
    toolbox->setFrameShadow(QFrame::Raised);


    ac_newFile = std::make_shared< QAction >( "New File", this );
    ac_newFile->setIcon(QIcon(":/icons/newfile.png"));
    ac_loadSVG = std::make_shared< QAction >( "Load SVG", this );
    ac_loadSVG->setIcon(QIcon(":/icons/svgicon.png"));
    ac_loadIMG = std::make_shared< QAction >( "Load Image", this );
    ac_loadIMG->setIcon(QIcon(":/icons/image.png"));
    ac_saveSVG = std::make_shared< QAction >( "Save SVG", this );
    //ac_saveSVG->setIcon(QIcon(":/icons/image.png"));
    ac_exit = std::make_shared< QAction > ("Exit", this);


    fileMenu = new QMenu("File");
    fileMenu->addAction(ac_newFile.get());
    fileMenu->addSeparator();
    fileMenu->addAction(ac_loadSVG.get());
    fileMenu->addAction(ac_loadIMG.get());
    fileMenu->addSeparator();
    fileMenu->addAction(ac_saveSVG.get());
    fileMenu->addSeparator();
    fileMenu->addAction(QIcon(":/icons/exit.ico"), tr("E&xit"), qApp, &QApplication::closeAllWindows);

    fileMenuButton = new QPushButton( canvas.get());
    fileMenuButton->setToolTip("File");
    fileMenuButton->setIcon(QIcon(":/icons/file_white.png"));
    fileMenuButton->setGeometry(QRect(5, 5, 50, 50));
    fileMenuButton->setIconSize(fileMenuButton->size()*0.85);

    fileMenuButton->setMenu(fileMenu);

    movezoom_button = new QPushButton( canvas.get());
    movezoom_button->setToolTip("Move / Zoom");
    movezoom_button->setGeometry(QRect(5, 60, 50, 50));
    movezoom_button->setIcon(QIcon(":/icons/move_white.svg"));
    movezoom_button->setIconSize(movezoom_button->size()*0.8);

    openContour_button = new QPushButton( "Open \nContour" , canvas.get());
    openContour_button->setToolTip("Open Contour");
    openContour_button->setGeometry(QRect(5, 115, 50, 50));
    //openContour_button->setCheckable(true);

    closedContour_button = new QPushButton( "Closed \nContour" , canvas.get());
    closedContour_button->setToolTip("Closed Contour");
    closedContour_button->setGeometry(QRect(5, 170, 50, 50));

    stripe_button = new QPushButton( "Stripe" , canvas.get());
    stripe_button->setToolTip("Stripe");
    stripe_button->setGeometry(QRect(5, 225, 50, 50));

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
    openContourEffect_MenuButton->setGeometry(QRect(55, 115, 15, 50));
    openContourEffect_MenuButton->setMenu(openContourEffect_Menu);

    closedContourEffect_Menu = new QMenu("&tr");

    closedContourEffect_Menu->addAction(ac_selectOverlapEffect1.get());
    closedContourEffect_Menu->addAction(ac_selectOverlapEffect2.get());
    closedContourEffect_Menu->addAction(ac_selectOverlapEffect3.get());
    closedContourEffect_Menu->addAction(ac_selectOverlapEffect4.get());

    closedContourEffect_MenuButton = new QPushButton(canvas.get());
    closedContourEffect_MenuButton->setToolTip("Select Overlap Effect");
    closedContourEffect_MenuButton->setGeometry(QRect(55, 170, 15, 50));
    closedContourEffect_MenuButton->setMenu(closedContourEffect_Menu);

    selectCross_button = new QPushButton( "Cross" , canvas.get());
    selectCross_button->setToolTip("Cross Selection");
    selectCross_button->setGeometry(QRect(5, 280, 50, 50));
    selectCross_button->setIcon(QIcon(":/icons/cross.svg"));

    selectCrop_button = new QPushButton( "Brush" , canvas.get());
    selectCrop_button->setToolTip("Brush Selection");
    selectCrop_button->setGeometry(QRect(5, 335, 50, 50));
    selectCrop_button->setIcon(QIcon(":/icons/edit.svg"));

    selectErase_button  = new QPushButton( "Erase" , canvas.get());
    selectErase_button->setToolTip("Erase");
    selectErase_button->setGeometry(QRect(5, 390, 50, 50));
    selectErase_button->setIcon(QIcon(":/icons/eraser.svg"));


    //    ac_oversketch = std::make_shared< QAction >( "Oversketch", this );
    //    ac_smooth = std::make_shared< QAction >( "Smooth", this );
    //    ac_defRotAxis = std::make_shared< QAction >( "Define Medial Axis", this );
    //    ac_crossSecBlendSurface = std::make_shared< QAction > ("Edit Cross Sectional Blending Surface", this);

    //ac_showNormals = std::make_shared< QAction >( "Show Normals", this );

    ac_exportFiles = std::make_shared< QAction >( "Export Files", this );
    ac_view3DPoints = std::make_shared< QAction >( "View 3D Points", this );

    //    modeMenu = new QMenu("Editing \nMenu");
    //    modeMenu->addAction(ac_oversketch.get());
    //    modeMenu->addAction(ac_smooth.get());
    //    modeMenu->addAction(ac_defRotAxis.get());
    //    modeMenu->addAction(ac_crossSecBlendSurface.get());

    oversketch_button = new QPushButton("Over\nSketch" , canvas.get());
    oversketch_button->setToolTip("Oversketching Mode");
    oversketch_button->setGeometry(QRect(5, 445, 50, 50));

    smooth_button = new QPushButton("Smooth" , canvas.get());
    smooth_button->setToolTip("Smooth Contour");
    smooth_button->setGeometry(QRect(5, 500, 50, 50));

    defRotAxis_button = new QPushButton("Rotation\nAxis" , canvas.get());
    defRotAxis_button->setToolTip("Define Rotation Axis");
    defRotAxis_button->setGeometry(QRect(5, 555, 50, 50));

    //    modeMenuButton = new QPushButton("Editing \nMenu" , canvas.get());
    //    modeMenuButton->setToolTip("Select Mode");
    //    modeMenuButton->setGeometry(QRect(15, 650, 90, 70));
    //    modeMenuButton->setMenu(modeMenu);

    //    normalsMenu = new QMenu("Normals \nMenu");
    //    normalsMenu->addAction(ac_estimateShape.get());
    //    normalsMenu->addAction(ac_showNormals.get());


    view3dLines_button = new QPushButton ("View 3D ", canvas.get());
    view3dLines_button->setToolTip("View 3D");
    view3dLines_button->setGeometry(QRect(5, 610, 50, 40));

    ac_exportView = std::make_shared< QAction >( "Export View", this );
    ac_exportMesh = std::make_shared< QAction >( "Export Mesh", this );

    exportMenu = new QMenu("Export \nMesh");



    exportMenu->addAction(ac_exportView.get());
    exportMenu->addAction(ac_exportMesh.get());

    exportMenu_button = new QPushButton( canvas.get());
    exportMenu_button->setGeometry(QRect(5, 655, 50, 40));
    exportMenu_button->setToolTip("Export View & Mesh");
    exportMenu_button->setText("Export");
    //exportMenu_button->setIcon(QIcon(":/icons/Export.ico"));
    exportMenu_button->setMenu(exportMenu);


    //    layeringDepth = new QSpinBox(canvas.get());
    //    layeringDepth->setGeometry(QRect(560, 640, 40, 30));
    //    layeringDepth->setValue(10);
    //    layeringDepth->setSingleStep(10);

    showLabels = new QCheckBox("Show Labels", canvas.get());
    showLabels->setGeometry(QRect(100, 630, 100, 30));
    showLabels->setChecked(true);
    //showLabels->setTristate(true);


    depth =  new QLabel (" Depths Between Layers: ", canvas.get());
    depth->setGeometry(QRect(350, 635, 130, 30));

    layerDepthList = new QListWidget(canvas.get());
    layerDepthList->setGeometry(QRect(490, 615, 100, 70));
    QListWidgetItem *item = new QListWidgetItem ( "Layer 1 : 0", layerDepthList );
    item->setFlags( item->flags() | Qt::ItemIsEditable );


}


void MainWindow::createCanvas1Actions (){


    connect( ac_newFile.get(), &QAction::triggered, canvas.get(), &Canvas::newFile);
    connect( ac_loadSVG.get(), &QAction::triggered, canvas.get(), &Canvas::loadSVG);
    connect( ac_loadIMG.get(), &QAction::triggered, canvas.get(), &Canvas::loadIMG);

    connect( ac_saveSVG.get(), &QAction::triggered, canvas.get(), &Canvas::saveSVG);

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

    //connect( layeringDepth, SIGNAL(valueChanged(const int &)), canvas.get(),  SLOT(changeLayerDifference(const int &)));

    connect ( showLabels, SIGNAL (stateChanged(const int &)), canvas.get(), SLOT (showLabels (const int &)));

    //    connect( ac_selectCross_Selection.get(), &QAction::triggered, canvas.get(), &Canvas::crossSelection );
    //    connect( ac_selectCrop_Selection.get(), &QAction::triggered, canvas.get(), &Canvas::cropSelection );
    //    connect( ac_selectErase_Selection.get(), &QAction::triggered, canvas.get(), &Canvas::eraseSelection );

    /// ----- CANVAS 2 ACTIONS&& TOOLBAR MOVED TO CANVAS 1 -----

    connect( oversketch_button, SIGNAL(pressed()), canvas.get(), SLOT(setOversketchingMode()));
    connect( smooth_button, SIGNAL(pressed()), canvas.get(), SLOT(smoothSketch()));
    connect( defRotAxis_button, SIGNAL(pressed()), canvas.get(), SLOT(setDefRotAxisMode()));
    //    connect( ac_crossSecBlendSurface.get(), &QAction::triggered, canvas2.get(), &Canvas2::setCrossSecBlendSurfaceMode);

    //    connect( ac_estimateShape.get(), &QAction::triggered, canvas2.get(), &Canvas2::estimateShape );
    //    connect( ac_showNormals.get(), &QAction::toggled, canvas2.get(), &Canvas2::showNormals );

    connect( ac_exportView.get(), &QAction::triggered, canvas.get(), &Canvas::exportView);

    connect( canvas.get(), &Canvas::updateLayerListSignal, this, &MainWindow::updateLayerList);
    //connect (layerDepthList, SIGNAL (itemDoubleClicked(QListWidgetItem*)), this, SLOT (adjustDepth(QListWidgetItem *)));
    connect (layerDepthList, SIGNAL (itemChanged(QListWidgetItem*)), this, SLOT (setDepths(QListWidgetItem *)));

}

void MainWindow::createLayersDockToolbar(){


    //    layerUp_button = new QPushButton("Up" , layers.get());
    //    layerUp_button->setToolTip("Move UP");
    //    layerUp_button->setGeometry(10, 20, 60, 30);
    //    layerUp_button->setIcon(QIcon(":/icons/arrow_up.svg"));

    //    layerDown_button = new QPushButton("Down" , layers.get());
    //    layerDown_button->setToolTip("Move Down");
    //    layerDown_button->setGeometry(80, 20, 60, 30);
    //    layerDown_button->setIcon(QIcon(":/icons/arrow_down.svg"));


}

void MainWindow::createLayersDockActions(){

    //    connect( layerUp_button, SIGNAL(pressed()), layers.get(), SLOT(layerUpSelectedCurve()) );
    //    connect( layerDown_button, SIGNAL(pressed()),  layers.get(), SLOT(layerDownSelectedCurve()));

}



void MainWindow::resizeEvent(QResizeEvent *){

    layers->openContourList->resize(layers_dock->width(), (layers_dock->height())/3);
    layers->closedContourList->resize(layers_dock->width(), (layers_dock->height())/3);
    layers->stripeContourList->resize(layers_dock->width(), (layers_dock->height())/3);;

    layers->openContourList->resize(layers_dock->width(), (layers_dock->height())/3);
    layers->closedContourList->resize(layers_dock->width(), (layers_dock->height())/3);
    layers->stripeContourList->resize(layers_dock->width(), (layers_dock->height())/3);

    //layers->openContourList->move(0,70);
    layers->closedContourList->move(0,layers->openContourList->height());
    layers->stripeContourList->move(0,layers->openContourList->height()+layers->closedContourList->height());

}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{

    dw_sketch->setFocus();
    canvas.get()->setFocus();
    //Set Focus on Mouse Move.. Can be changed to Mouse Over QDockWidget.

}

void MainWindow::updateLayerList(){

    //qDebug () << "Update on MainWindow";
    layerDepthList->clear();
    QList<QString> a = canvas->getLayerList();
    //qDebug () << a;

    for (int i = 0 ; i < a.size(); i++){
        QString label;

        label = "Layer ";
        label.append(QString::number(i+1));
        label.append(" : ");
        label.append(a[i]);

        QListWidgetItem *item = new QListWidgetItem ( label, layerDepthList );
        item->setFlags( item->flags() | Qt::ItemIsEditable );
    }



}

void MainWindow::adjustDepth(QListWidgetItem *item){

    //() << "DOUBLE CLICKED";

    layerDepthList->editItem(item);

}

void MainWindow::setDepths(QListWidgetItem *item){

    //() << "Modified on MainWindow";

    if (item->text().size()>3){
        return;
    }

    canvas->setDepths(layerDepthList->row(item), item->text());
    QString label;

    label = "Layer ";
    label.append(QString::number(layerDepthList->row(item) + 1));
    label.append(" : ");
    label.append(item->text());

    QListWidgetItem *it = new QListWidgetItem(label);
    it->setFlags( it->flags() | Qt::ItemIsEditable );

    layerDepthList->insertItem(layerDepthList->row(item),it);
    layerDepthList->takeItem(layerDepthList->row(item));

}






#include "moc_main_window.cpp"
