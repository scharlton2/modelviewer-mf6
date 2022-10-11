#include "mainwindow.h"
#include "ui_mainwindow.h"

//#include <QCloseEvent>
//#include <QDebug.h>
//#include <QFileDialog>
//#include <QMessageBox>
//#include <QSaveFile>
//#include <QScreen>

//
// vtkMFCWindow*    m_MFCWindow                        ->   QVTKOpenGLNativeWidget*   mainWidget
//                  m_MFCWindow->GetRenderWindow()                                    mainWidget->renderWindow()
//                  m_MFCWindow->GetInteractor()                                      mainWidget->interactor()
//
// CMainFrame                                          ->   MainWindow
//
// vtkRenderer *m_Renderer                             ->   MvView::renderer

//
// connect(fileMenu, SIGNAL(aboutToShow()), this, SLOT(updateFileActions()));                       // Qt4-style
// connect(fileMenu, SIGNAL(aboutToShow), this, SLOT(updateFileActions));                           // Qt4-style -- error -- QObject::connect: Parentheses expected
// connect(fileMenu, &QMenu::aboutToShow, this, &MainWindow::updateFileActions);                    // New-style
// connect(fileMenu, qOverload<int>(&QMenu::aboutToShow), this, &MainWindow::updateFileActions);    // New-style with overload

// 
// MainWindow::setCurrentFile   ->  mvDoc::setCurrentFile
// MainWindow::onFileOpen       ->  mvDoc::onFileOpen
// MainWindow::onFileNew        ->  mvDoc::onFileNew
//

// call QWidget::update() to schedule a paint event
// call QWidget::repaint() to force a paint event

#include <QtWidgets>

#include <vtkConeSource.h>
#include <vtkEventQtSlotConnect.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkInteractorStyleTrackballActor.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkVersion.h>

#include "mvdoc.h"
#include "mvview.h"
#include "displaysizedialog.h"

#include "bitmapresolutiondialog.h"

#include "mvManager.h"
#include "mvGUISettings.h"
#include "mvSaveCurrentDirectory.h"

const QString MainWindow::geometryKey    = "geometry";
const QString MainWindow::recentFilesKey = "recentFileList";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , doc{nullptr}
    , view{nullptr}
    //, gui{nullptr}
    //, _modifiedFlag{false}

{
#if defined(Q_OS_WINDOWS)
    //{{ unit testing
    // std::string mvSaveCurrentDirectory::GetFullPath(const char *szMore, const char *szDirectory)
    std::string            s = mvSaveCurrentDirectory::GetFullPath(".\\New folder\\Greenport_Domain.shp", "D:\\Issues\\modflow\\model viewer for mf6\\issue-51");
    assert(s.compare("D:\\Issues\\modflow\\model viewer for mf6\\issue-51\\New folder\\Greenport_Domain.shp") == 0);
    //}}
#endif

    vtkNew<vtkNamedColors> colors;

    doc     = new MvDoc(this);
    view    = new MvView(doc);
    doc->addView(view);

    setCentralWidget(view->mainWidget());

    createActions();
    createMenus();
    createStatusBar();

    readSettings();

    setCurrentFile(QString());

    updateRecentFileActions();
    updateStatusBar();
}

MainWindow::~MainWindow()
{
}

void MainWindow::createActions()
{
    // File->New...
    //const QIcon newIcon     = QIcon::fromTheme("document-new", QIcon(":/images/new.png"));
    newAct      = new QAction(/* newIcon, */ tr("&New..."), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new mvmv6 file"));
    connect(newAct, &QAction::triggered, this, &MainWindow::onFileNew);

    // File->Open...
    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing mvmv6 file"));
    connect(openAct, &QAction::triggered, this, &MainWindow::onFileOpen);

    // File->Close
    closeAct = new QAction(tr("&Close"), this);
#if !defined(WIN32)
    closeAct->setShortcuts(QKeySequence::Close);
#endif
    closeAct->setStatusTip(tr("Close the current mvmv6 file"));
    connect(closeAct, &QAction::triggered, this, &MainWindow::onFileClose);

    // File->Save
    saveAction = new QAction(tr("&Save"), this);
    saveAction->setShortcuts(QKeySequence::Save);
    saveAction->setStatusTip(tr("Save the current mvmf6 file"));
    connect(saveAction, &QAction::triggered, this, &MainWindow::onFileSave);

    // File->Save As...
    saveAsAction = new QAction(tr("Save &As..."), this);
    saveAction->setShortcuts(QKeySequence::SaveAs);
    saveAsAction->setStatusTip(tr("Save the current mvmf6 file with a new name"));
    connect(saveAsAction, &QAction::triggered, this, &MainWindow::onFileSaveAs);

    // -----------------------------

    // File->Export As Bmp...
    exportAsBmpAction = new QAction(tr("Export As &Bmp..."), this);
    exportAsBmpAction->setStatusTip(tr("Export the current display as a bitmap file"));
    connect(exportAsBmpAction, &QAction::triggered, this, &MainWindow::onFileExportAsBmp);


    // File->Export Ani&mation...
    exportAnimationAction = new QAction(tr("Export Ani&mation..."), this);
    exportAnimationAction->setStatusTip(tr("Export an animation as a sequence of bitmap files"));
    connect(exportAnimationAction, &QAction::triggered, this, &MainWindow::onExportAnimation);

    // File->Preferences...
    preferencesAction = new QAction(tr("Pre&ferences..."), this);
    preferencesAction->setStatusTip(tr("Set application preferences"));
    connect(preferencesAction, &QAction::triggered, this, &MainWindow::onPreferences);


    // -----------------------------

    //QAction *printAction;
    //QAction *printPreviewAction;
    //QAction *printSetupAction;

    // -----------------------------

    //QAction *preferencesAction;

    // -----------------------------

    // Recent Files
    for (int i = 0; i < MainWindow::MaxRecentFiles; ++i)
    {
        recentFileActions[i] = new QAction(this);
        recentFileActions[i]->setVisible(false);
        connect(recentFileActions[i], &QAction::triggered, this, &MainWindow::openRecentFile);
    }

    // -----------------------------

    // File->Exit
    //const QIcon exitIcon     = QIcon::fromTheme("application-exit");
    exitAct = new QAction(/* exitIcon, */ tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Quit the application"));
    connect(exitAct, &QAction::triggered, qApp, &QApplication::closeAllWindows);


    // Show->Solid
    showSolidAction    = new QAction(tr("&Solid"), this);
    showSolidAction->setCheckable(true);
    showSolidAction->setStatusTip(tr("Display scalar data as a solid"));
    connect(showSolidAction, &QAction::triggered, this, &MainWindow::onShowSolid);

    // Show->Isosurfaces
    showIsosurfacesAction = new QAction(tr("&Isosurfaces"), this);
    showIsosurfacesAction->setCheckable(true);
    showIsosurfacesAction->setStatusTip(tr("Display scalar data as a set of isosurfaces"));
    connect(showIsosurfacesAction, &QAction::triggered, this, &MainWindow::onShowIsosurfaces);

    // Show->None
    showNoneAction = new QAction(tr("N&one"), this);
    showNoneAction->setCheckable(true);
    showNoneAction->setStatusTip(tr("Do not display scalar data"));
    connect(showNoneAction, &QAction::triggered, this, &MainWindow::onShowNone);

    // -----------------------------

    // Show->Vectors
    showVectorsAction = new QAction(tr("&Vectors"), this);
    showVectorsAction->setCheckable(true);
    showVectorsAction->setStatusTip(tr("Show or hide vector data"));
    connect(showVectorsAction, &QAction::triggered, this, &MainWindow::onShowVectors);

    // Show->Pathlines
    showPathlinesAction = new QAction(tr("&Pathlines"), this);
    showPathlinesAction->setCheckable(true);
    showPathlinesAction->setStatusTip(tr("Show or hide pathlines"));
    connect(showPathlinesAction, &QAction::triggered, this, &MainWindow::onShowPathlines);

    // Show->Model Features
    showModelFeaturesAction = new QAction(tr("Model &Features"), this);
    showModelFeaturesAction->setCheckable(true);
    showModelFeaturesAction->setStatusTip(tr("Show or hide model features"));
    connect(showModelFeaturesAction, &QAction::triggered, this, &MainWindow::onModelFeatures);

    // -----------------------------

    // Show->Grid Shell
    showGridShellAction = new QAction(tr("Grid S&hell"), this);
    showGridShellAction->setCheckable(true);
    showGridShellAction->setStatusTip(tr("Show or hide the outer shell containing all active cells"));
    connect(showGridShellAction, &QAction::triggered, this, &MainWindow::onShowGridShell);

    // Show->Grid Lines
    showGridLinesAction = new QAction(tr("Grid Li&nes"), this);
    showGridLinesAction->setCheckable(true);
    showGridLinesAction->setStatusTip(tr("Show or hide grid lines"));
    connect(showGridLinesAction, &QAction::triggered, this, &MainWindow::onShowGridLines);

    // Show->Axes Symbol
    showAxesAction = new QAction(tr("A&xes Symbol"), this);
    showAxesAction->setCheckable(true);
    showAxesAction->setStatusTip(tr("Show or hide the axes symbol"));
    connect(showAxesAction, &QAction::triggered, this, &MainWindow::onShowAxes);

    // Show->Bounding Box
    showBoundingBoxAction = new QAction(tr("&Bounding Box"), this);
    showBoundingBoxAction->setCheckable(true);
    showBoundingBoxAction->setStatusTip(tr("Show or hide the box that bounds the grid"));
    connect(showBoundingBoxAction, &QAction::triggered, this, &MainWindow::onShowBoundingBox);

    // Show->Overlay
    showOverlayAction = new QAction(tr("Ov&erlay"), this);
    showOverlayAction->setCheckable(true);
    showOverlayAction->setStatusTip(tr("Show or hide overlay"));
    connect(showOverlayAction, &QAction::triggered, this, &MainWindow::onShowOverlay);

    // -----------------------------

    // Show->Time
    showTimeAction = new QAction(tr("&Time"), this);
    showTimeAction->setCheckable(true);
    showTimeAction->setStatusTip(tr("Show or hide simulation time"));
    connect(showTimeAction, &QAction::triggered, this, &MainWindow::onShowTime);

    // Show->Color Bar
    showColorBarAction = new QAction(tr("&Color Bar"), this);
    showColorBarAction->setCheckable(true);
    showColorBarAction->setStatusTip(tr("Show or hide the color bar"));
    connect(showColorBarAction, &QAction::triggered, this, &MainWindow::onShowColorBar);


    // Action->Set Size of Display Area...
    setSizeAction    = new QAction(tr("Set Si&ze of Display Area..."), this);
    setSizeAction->setStatusTip(tr("Set the size of the display area"));
    connect(setSizeAction, &QAction::triggered, this, &MainWindow::setSize);


    // Action->View From Direction->+x
    viewFromPx = new QAction(tr("+x"), this);
    viewFromPx->setStatusTip(tr("View from the positive x axis towards the negative x direction"));
    connect(viewFromPx, &QAction::triggered, this, &MainWindow::onViewFromPx);
    
    // Action->View From Direction->-x
    viewFromNx = new QAction(tr("-x"), this);
    viewFromNx->setStatusTip(tr("View from the negative x axis towards the positive x direction"));
    connect(viewFromNx, &QAction::triggered, this, &MainWindow::onViewFromNx);

    // Action->View From Direction->+y
    viewFromPy = new QAction(tr("+y"), this);
    viewFromPy->setStatusTip(tr("View from the positive y axis towards the negative y direction"));
    connect(viewFromPy, &QAction::triggered, this, &MainWindow::onViewFromPy);

    // Action->View From Direction->-y
    viewFromNy = new QAction(tr("-y"), this);
    viewFromNy->setStatusTip(tr("View from the negative y axis towards the positive y direction"));
    connect(viewFromNy, &QAction::triggered, this, &MainWindow::onViewFromNy);

    // Action->View From Direction->+z
    viewFromPz = new QAction(tr("+z"), this);
    viewFromPz->setStatusTip(tr("View from the positive z axis towards the negative z direction"));
    connect(viewFromPz, &QAction::triggered, this, &MainWindow::onViewFromPz);

    // Action->View From Direction->-z
    viewFromNz = new QAction(tr("-z"), this);
    viewFromNz->setStatusTip(tr("View from the negative z axis towards the positive z direction"));
    connect(viewFromNz, &QAction::triggered, this, &MainWindow::onViewFromNz);

    // Action->View From Next Direction
    viewFromNextDirection = new QAction(tr("View From &Next Direction"), this);
    viewFromNextDirection->setStatusTip(tr("Change to the next preset viewpoint"));
    viewFromNextDirection->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_D));
    connect(viewFromNextDirection, &QAction::triggered, this, &MainWindow::onViewFromNextDirection);

    // -----------------------------

    // Action->Set Projection To->Perspective
    setProjectionToPerspective = new QAction(tr("&Perspective"), this);
    setProjectionToPerspective->setCheckable(true);
    setProjectionToPerspective->setStatusTip(tr("Parallel lines appear parallel to each other as they extend to infinity"));
    connect(setProjectionToPerspective, &QAction::triggered, doc, &MvDoc::onPerspectiveProjection);

    // Action->Set Projection To->Parallel
    setProjectionToParallel = new QAction(tr("Pa&rallel"), this);
    setProjectionToParallel->setCheckable(true);
    setProjectionToParallel->setStatusTip(tr("Parallel lines appear to converge at infinity"));
    connect(setProjectionToParallel, &QAction::triggered, doc, &MvDoc::onParallelProjection);


    // Action->Save Viewpoint
    saveViewpoint = new QAction(tr("&Save Viewpoint"), this);
    saveViewpoint->setStatusTip(tr("Save the current viewpoint"));
    saveViewpoint->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_W));
    connect(saveViewpoint, &QAction::triggered, view, &MvView::onSaveViewpoint);


    // Action->Recall Viewpoint
    recallViewpoint = new QAction(tr("&Recall Viewpoint"), this);
    recallViewpoint->setStatusTip(tr("Restore the saved viewpoint"));
    recallViewpoint->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Z));
    connect(recallViewpoint, &QAction::triggered, view, &MvView::onRecallViewpoint);


    // Toolbox->Data
    dataAction = new QAction(tr("&Data"), this);
    dataAction->setCheckable(true);
    dataAction->setStatusTip(tr("Show or hide the Data Toolbox"));
    connect(dataAction, &QAction::triggered, doc, &MvDoc::onToolboxData);


    // Toolbox->Color Bar
    colorBarAction = new QAction(tr("&Color Bar"), this);
    colorBarAction->setCheckable(true);
    colorBarAction->setStatusTip(tr("Show or hide the Color Bar Toolbox"));
    connect(colorBarAction, &QAction::triggered, doc, &MvDoc::onToolboxColorBar);


    // Toolbox->Geometry
    geometryAction = new QAction(tr("Geo&metry"), this);
    geometryAction->setCheckable(true);
    geometryAction->setStatusTip(tr("Show or hide the Geometry Toolbox"));
    connect(geometryAction, &QAction::triggered, doc, &MvDoc::onToolboxGeometry);
}

void MainWindow::updateFileActions()
{
    ///////////////////////////////////////////////////////////////////////////
    // File
    ///////////////////////////////////////////////////////////////////////////
#if !defined(NDEBUG)
    qDebug() << "updateFileActions\n";
#endif
    
    // File->Close
    closeAct->setEnabled(this->doc->_manager->GetDataFileList() && !this->isAnimating());

    // File->Save
    saveAction->setEnabled(this->doc->_manager->GetDataFileList() != nullptr &&
                                      !this->isAnimating());

    // File->Save As
    saveAsAction->setEnabled(this->doc->_manager->GetDataFileList() != nullptr &&
                                      !this->isAnimating());

    // File->Export As Bmp...
    exportAsBmpAction->setEnabled(this->view->renderer->VisibleActorCount() > 0 &&
                                  !this->isAnimating());

    // File->Export Animation...
    exportAnimationAction->setEnabled(this->view->renderer->VisibleActorCount() > 0 &&
                                      !this->isAnimating());

    // File->Preferences...
    // preferencesAction->setEnabled(true);
}

void MainWindow::updateShowActions()
{
    ///////////////////////////////////////////////////////////////////////////
    // Show
    ///////////////////////////////////////////////////////////////////////////
#if !defined(NDEBUG)
    qDebug() << "updateShowActions\n";
#endif

    // Show->Solid
    showSolidAction->setChecked(this->doc->_manager->IsSolidVisible());
    showSolidAction->setEnabled(this->doc->_manager->GetDataFileList() && !this->isAnimating());

    // Show->Isosurfaces
    showIsosurfacesAction->setChecked(this->doc->_manager->AreIsosurfacesVisible());
    showIsosurfacesAction->setEnabled(this->doc->_manager->GetDataFileList() && !this->isAnimating() && this->doc->_manager->GetGridDisplayMode() == MV_GRID_INTERPOLATED);

    // Show->None
    showNoneAction->setChecked(!this->doc->_manager->IsSolidVisible() && !this->doc->_manager->AreIsosurfacesVisible() && this->doc->_manager->GetDataFileList());
    showNoneAction->setEnabled(this->doc->_manager->GetDataFileList() && !this->isAnimating());

    // -----------------------------

    // Show->Vectors
    showVectorsAction->setChecked(this->doc->_manager->AreVectorsVisible());
    showVectorsAction->setEnabled(this->doc->_manager->HasVectorData() && !this->isAnimating());

    // Show->Pathlines
    showPathlinesAction->setChecked(this->doc->_manager->ArePathlinesVisible());
    showPathlinesAction->setEnabled(this->doc->_manager->HasPathlineData() && !this->isAnimating());

    // Show->Model Features
    showModelFeaturesAction->setChecked(this->doc->_manager->AreModelFeaturesVisible());
    showModelFeaturesAction->setEnabled(this->doc->_manager->HasModelFeatures() && !this->isAnimating());

    // -----------------------------

    // Show->Grid Shell
    showGridShellAction->setChecked(this->doc->_manager->IsGridShellVisible());
    showGridShellAction->setEnabled(this->doc->_manager->GetDataFileList() && !this->isAnimating());

    // Show->Grid Lines
    if (this->doc->_manager->GetGridType() == GridType::MV_STRUCTURED_GRID)
    {
        showGridLinesAction->setChecked(this->doc->_manager->AreActivatedGridLinesVisible());
    }
    else if (this->doc->_manager->GetGridType() == GridType::MV_LAYERED_GRID)
    {
        showGridLinesAction->setChecked(this->doc->_manager->IsGridLayerVisible());
    }
    else if (this->doc->_manager->GetGridType() == GridType::MV_UNSTRUCTURED_GRID)
    {
        showGridLinesAction->setChecked(false);
    }
    showGridLinesAction->setEnabled(this->doc->_manager->GetDataFileList() && !this->isAnimating() && this->doc->_manager->GetGridDisplayMode() == MV_GRID_INTERPOLATED);

    // Show->Axes Symbol
    showAxesAction->setChecked(this->doc->_manager->AreAxesVisible());
    showAxesAction->setEnabled(this->doc->_manager->GetDataFileList() && !this->isAnimating());

    // Show->Bounding Box
    showBoundingBoxAction->setChecked(this->doc->_manager->IsBoundingBoxVisible());
    showBoundingBoxAction->setEnabled(this->doc->_manager->GetDataFileList() && !this->isAnimating());

    // Show->Overlay
    showOverlayAction->setChecked(this->doc->_manager->IsOverlayVisible());
    showOverlayAction->setEnabled(this->doc->_manager->HasOverlay() && !this->isAnimating());

    // Show->Time
    showTimeAction->setChecked(this->doc->_manager->IsTimeLabelVisible());
    showTimeAction->setEnabled(this->doc->_manager->GetDataFileList() && !this->isAnimating());

    // Show->Color Bar
    showColorBarAction->setChecked(this->doc->_manager->IsColorBarVisible());
    showColorBarAction->setEnabled(this->doc->_manager->GetDataFileList() && !this->isAnimating());
}

void MainWindow::updateActionActions()
{
    ///////////////////////////////////////////////////////////////////////////
    // Action
    ///////////////////////////////////////////////////////////////////////////
#if !defined(NDEBUG)
    qDebug() << "updateActionActions\n";
#endif

    // Action->View From Direction->+x
    view->onUpdateViewFrom(viewFromPx);

    // Action->View From Direction->-x
    view->onUpdateViewFrom(viewFromNx);

    // Action->View From Direction->+y
    view->onUpdateViewFrom(viewFromPy);

    // Action->View From Direction->-y
    view->onUpdateViewFrom(viewFromNy);

    // Action->View From Direction->+z
    view->onUpdateViewFrom(viewFromPz);

    // Action->View From Direction->-z
    view->onUpdateViewFrom(viewFromNz);

    // Action->View From &Next Direction
    view->onUpdateViewFrom(viewFromNextDirection);

    // Action->Set Projection To->Perspective
    doc->onUpdatePerspectiveProjection(setProjectionToPerspective);

    // Action->Set Projection To->Parallel
    doc->onUpdateParallelProjection(setProjectionToParallel);

    // Action->Recall Viewpoint
    view->onUpdateRecallViewport(recallViewpoint);
}

void MainWindow::updateToolboxActions()
{
    ///////////////////////////////////////////////////////////////////////////
    // Toolbox
    ///////////////////////////////////////////////////////////////////////////
#if !defined(NDEBUG)
    qDebug() << "updateToolboxActions\n";
#endif

    // Toolbox->Data
    doc->onUpdateToolboxData(dataAction);

    // Toolbox->Color Bar
    doc->onUpdateToolboxColorBar(colorBarAction);

    // Toolbox->Geometry
    doc->onUpdateToolboxGeometry(geometryAction);
}

bool MainWindow::isAnimating() const
{
    return false;
}

void MainWindow::createMenus()
{
    ///////////////////////////////////////////////////////////////////////////
    // File
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    connect(fileMenu, &QMenu::aboutToShow, this, &MainWindow::updateFileActions);

    // File->New...
    fileMenu->addAction(newAct);

    // File->Open...
    fileMenu->addAction(openAct);

    // File->Close
    fileMenu->addAction(closeAct);

    // File->Save
    fileMenu->addAction(saveAction);

    // File->Save As...
    fileMenu->addAction(saveAsAction);

    // -----------------------------
    fileMenu->addSeparator();

    // File->Export As Bmp...
    fileMenu->addAction(exportAsBmpAction);

    // File->Export Animation...
    fileMenu->addAction(exportAnimationAction);

    // -----------------------------
    fileMenu->addSeparator();

    // File->Preferences...
    fileMenu->addAction(preferencesAction);

    // -----------------------------
    recentFileSeparatorAction = fileMenu->addSeparator();

    for (int i = 0; i < MainWindow::MaxRecentFiles; ++i)
    {
        fileMenu->addAction(recentFileActions[i]);
    }

    // -----------------------------
    fileMenu->addSeparator();

    // File->Exit
    fileMenu->addAction(exitAct);

    ///////////////////////////////////////////////////////////////////////////
    // Show
    QMenu *showMenu = menuBar()->addMenu(tr("&Show"));
    connect(showMenu, &QMenu::aboutToShow, this, &MainWindow::updateShowActions);

    // Show->Solid
    showMenu->addAction(showSolidAction);

    // Show->Isosurfaces
    showMenu->addAction(showIsosurfacesAction);

    // Show->None
    showMenu->addAction(showNoneAction);

    // -----------------------------
    showMenu->addSeparator();

    // Show->Vectors
    showMenu->addAction(showVectorsAction);

#if 0 // @todo
    // Show->Pathlines
    showMenu->addAction(showPathlinesAction);
#endif

    // Show->Model Features
    showMenu->addAction(showModelFeaturesAction);

    // -----------------------------
    showMenu->addSeparator();

    // Show->Grid Shell
    showMenu->addAction(showGridShellAction);

    // Show->Grid Lines
    showMenu->addAction(showGridLinesAction);

    // Show->Axes Symbol
    showMenu->addAction(showAxesAction);

    // Show->Bounding Box
    showMenu->addAction(showBoundingBoxAction);

    // Show->Overlay
    showMenu->addAction(showOverlayAction);

    // -----------------------------
    showMenu->addSeparator();

    // Show->Grid Shell
    showMenu->addAction(showGridShellAction);

    // Show->Grid Lines
    showMenu->addAction(showGridLinesAction);

    // Show->Axes Symbol
    showMenu->addAction(showAxesAction);

    // Show->Bounding Box
    showMenu->addAction(showBoundingBoxAction);

    // Show->Overlay
    showMenu->addAction(showOverlayAction);

    // -----------------------------
    showMenu->addSeparator();

    // Show->Time
    showMenu->addAction(showTimeAction);

    // Show->Color Bar
    showMenu->addAction(showColorBarAction);

    ///////////////////////////////////////////////////////////////////////////
    // Action
    QMenu *actionMenu = menuBar()->addMenu(tr("&Action"));
    connect(actionMenu, &QMenu::aboutToShow, this, &MainWindow::updateActionActions);

    // Action->Set Size of Display Area...
    actionMenu->addAction(setSizeAction);

    // -----------------------------
    actionMenu->addSeparator();

    QMenu *viewFromDirection = actionMenu->addMenu(tr("View From &Direction"));

    viewFromDirection->addAction(viewFromPx);
    viewFromDirection->addAction(viewFromNx);
    viewFromDirection->addAction(viewFromPy);
    viewFromDirection->addAction(viewFromNy);
    viewFromDirection->addAction(viewFromPz);
    viewFromDirection->addAction(viewFromNz);

    // Action->View From Next Direction
    actionMenu->addAction(viewFromNextDirection);

    // -----------------------------
    actionMenu->addSeparator();

    // Set &Projection To
    QMenu *setProjectionTo = actionMenu->addMenu(tr("Set &Projection To"));

    // Action->Set Projection To->Perspective
    setProjectionTo->addAction(setProjectionToPerspective);

    // Action->Set Projection To->Parallel
    setProjectionTo->addAction(setProjectionToParallel);

    // -----------------------------
    actionMenu->addSeparator();

    // Action->Save Viewpoint
    actionMenu->addAction(saveViewpoint);

    // Action->Recall Viewpoint
    actionMenu->addAction(recallViewpoint);

    ///////////////////////////////////////////////////////////////////////////
    // Toolbox
    QMenu *toolboxMenu = menuBar()->addMenu(tr("&Toolbox"));
    connect(toolboxMenu, &QMenu::aboutToShow, this, &MainWindow::updateToolboxActions);

    // Toolbox->Data
    toolboxMenu->addAction(dataAction);

    // -----------------------------
    toolboxMenu->addSeparator();

    // Toolbox->Color Bar
    toolboxMenu->addAction(colorBarAction);

    // -----------------------------
    toolboxMenu->addSeparator();

    // Toolbox->Geometry
    toolboxMenu->addAction(geometryAction);
}

void MainWindow::createStatusBar()
{
    //QLabel *model = new QLabel();
    model = new QLabel();
    model->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    model->setText("12345678901234567890");   
    QSize sz = model->sizeHint();
    model->setMinimumSize(sz);
    model->setText("");
    statusBar()->addPermanentWidget(model);

    //QLabel *data = new QLabel();
    data = new QLabel();
    data->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    data->setMinimumSize(sz);
    data->setText("");
    statusBar()->addPermanentWidget(data);

    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::updateStatusBar()
{
    assert(doc);
    setModelName(doc->modelName());
    setDataName(doc->activeScalarName());
}

void MainWindow::setDataName(const QString &name)
{
    data->setText(name);
}

void MainWindow::setModelName(const QString &name)
{
    model->setText(name);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (doc->maybeSave())
    {
        writeSettings();
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

//void MainWindow::newFile()
//{
//    if (maybeSave())
//    {
//        setCurrentFile(QString());
//    }
//}

void MainWindow::onFileNew()
{
    if (doc->maybeSave())
    {
        doc->onFileNew();
    }
}


//  void CDocManager::OnFileOpen()
//    BOOL CDocManager::DoPromptFileName(CString& fileName, UINT nIDSTitle, DWORD lFlags, BOOL bOpenFileDialog, CDocTemplate* pTemplate)
//    CDocument* CWinApp::OpenDocumentFile(LPCTSTR lpszFileName)
//    CDocument* CDocManager::OpenDocumentFile(LPCTSTR lpszFileName)
//      CDocument* CDocManager::OpenDocumentFile(LPCTSTR lpszFileName, BOOL bAddToMRU)
//        CDocument* CSingleDocTemplate::OpenDocumentFile(LPCTSTR lpszPathName, BOOL bAddToMRU, BOOL bMakeVisible)
//          CDocument* CSingleDocTemplate::OpenDocumentFile(LPCTSTR lpszPathName, BOOL bAddToMRU, BOOL bMakeVisible)
//            // SaveModified or maybeSave
//            BOOL CMvDoc::OnOpenDocument(LPCTSTR lpszPathName)

//>	mvmf6.exe!CMvDoc::OnOpenDocument(const char * lpszPathName) Line 239	C++
// 	mfc140d.dll!CSingleDocTemplate::OpenDocumentFile(const char * lpszPathName, int bAddToMRU, int bMakeVisible) Line 169	C++
// 	mfc140d.dll!CDocManager::OpenDocumentFile(const char * lpszFileName, int bAddToMRU) Line 1056	C++
// 	mfc140d.dll!CDocManager::OpenDocumentFile(const char * lpszFileName) Line 965	C++
// 	mfc140d.dll!CWinApp::OpenDocumentFile(const char * lpszFileName) Line 86	C++
// 	mfc140d.dll!CDocManager::OnFileOpen() Line 924	C++
// 	mfc140d.dll!CWinApp::OnFileOpen() Line 30	C++

// @todo after manipulating the view set modified flag

void MainWindow::onFileOpen()
{
    //if (!maybeSave()) return;

    //QString fileName = QFileDialog::getOpenFileName(this, tr("Open"), QString(), tr("MvMf6 Files (*.mvmf6)"));

    //if (fileName.isEmpty())
    //{
    //    return;
    //}

    ////{{ application MainWindow::loadFile(QString fileName) -> MvDoc::openDocument(QString fileName, QWidget* parent = nullptr)
    //delete doc->_manager;
    //delete doc->_gui;

    //QApplication::setOverrideCursor(Qt::WaitCursor);

    //std::string errorMsg;
    //doc->_gui     = new mvGUISettings();
    //doc->_manager = new mvManager();
    //doc->_manager->Deserialize(QDir::toNativeSeparators(fileName).toLocal8Bit().data(), doc->_gui, errorMsg);
    //if (errorMsg.size())
    //{
    //    QMessageBox::information(this, "Error", errorMsg.c_str());
    //}

    //// view->renderer->GetViewProps()->RemoveAllItems();
    //view->renderer->RemoveAllViewProps();
    //vtkSmartPointer<vtkPropCollection> props = doc->_manager->GetPropCollection();
    //props->InitTraversal();
    //for (int i = 0; i < props->GetNumberOfItems(); i++)
    //{
    //    view->addViewProp(props->GetNextProp());
    //}
    //view->applyViewSettings(doc->_gui);

    //this->updateActions();
    //this->updateStatusBar();
    //this->setCurrentFile(fileName);

    //QApplication::restoreOverrideCursor();
    ////}} application MainWindow::loadFile(QString fileName)

    //////
    doc->onFileOpen();
}

void MainWindow::onFileClose()
{
    doc->onFileClose();
    updateStatusBar();
}

//bool MainWindow::isModified() const
//{
//    return _modifiedFlag;
//}

bool MainWindow::onFileSave()
{
    if (doc->_currentFile.isEmpty())
    {
        return onFileSaveAs();
    }
    else
    {
        return doc->saveFile(doc->currentFile());
    }
}


bool MainWindow::onFileSaveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save As"),
                                                    doc->currentFile(),
                                                    tr("MvMf6 Files (*.mvmf6);;All Files (*.*)"));
    if (!fileName.isEmpty())
    {
        fileName = QDir::toNativeSeparators(fileName);
        return doc->saveFile(fileName);
    }
    return false;
}

void MainWindow::onFileExportAsBmp()
{
    view->onFileExportAsBmp(this);
}

void MainWindow::onExportAnimation()
{
    view->onFileExportAnimation(this);
}

void MainWindow::onPreferences()
{
    doc->onPreferences();
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    doc->setCurrentFile(fileName);
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void MainWindow::readSettings()
{
    // default settings use QCoreApplication::organizationName() and QCoreApplication::applicationName()
    QSettings        settings;
    const QByteArray geometry = settings.value(MainWindow::geometryKey, QByteArray()).toByteArray();
    if (geometry.isEmpty())
    {
        const QRect availableGeometry = screen()->availableGeometry();
        resize(availableGeometry.width() * 2 / 3, availableGeometry.height() * 2 / 3);
    }
    else
    {
        restoreGeometry(geometry);
    }
}

void MainWindow::writeSettings()
{
    // default settings use QCoreApplication::organizationName() and QCoreApplication::applicationName()
    QSettings settings;
    settings.setValue(MainWindow::geometryKey, saveGeometry());
}

void MainWindow::setSize()
{
#if !defined(NDEBUG)
    qDebug() << "MainWindow size: "     << this->size();
    qDebug() << "MainWindow geometry: " << this->geometry();
    qDebug() << "mainWidget size: "     << view->mainWidget()->size();
    qDebug() << "mainWidget geometry: " << view->mainWidget()->geometry();
#endif

    DisplaySizeDialog dlg(view->mainWidget()->size(), this);
    if (dlg.exec() == QDialog::Accepted)
    {
        int   borderWidth  = size().width() - view->mainWidget()->size().width();
        int   borderHeight = size().height() - view->mainWidget()->size().height();
        int   newWndWidth  = dlg.size().width() + borderWidth;
        int   newWndHeight = dlg.size().height() + borderHeight;
        resize(newWndWidth, newWndHeight);
    }
}

void MainWindow::onViewFromPx()
{
    view->onViewFromPx();
}

void MainWindow::onViewFromNx()
{
    view->onViewFromNx();
}

void MainWindow::onViewFromPy()
{
    view->onViewFromPy();
}

void MainWindow::onViewFromNy()
{
    view->onViewFromNy();
}

void MainWindow::onViewFromPz()
{
    view->onViewFromPz();
}

void MainWindow::onViewFromNz()
{
    view->onViewFromNz();
}

void MainWindow::onViewFromNextDirection()
{
    view->onViewFromNextDirection();
}

void MainWindow::onShowNone()
{
    doc->onShowNone();

    // update Show menu
    this->showIsosurfacesAction->setChecked(false);
    this->showNoneAction->setChecked(true);
    this->showSolidAction->setChecked(false);
}

void MainWindow::onShowSolid()
{
    doc->onShowSolid();

    // update Show menu
    this->showIsosurfacesAction->setChecked(false);
    this->showNoneAction->setChecked(false);
    this->showSolidAction->setChecked(true);
}

void MainWindow::onShowIsosurfaces()
{
    doc->onShowIsosurfaces();

    // update Show menu
    this->showIsosurfacesAction->setChecked(true);
    this->showNoneAction->setChecked(false);
    this->showSolidAction->setChecked(false);
}

void MainWindow::onShowVectors()
{
    doc->onShowVectors(this);  // @todo see if this(CWidget*) is necessary
}

void MainWindow::onShowPathlines()
{
    doc->onShowPathlines();     // @todo check this see MvDoc::onShowPathlines
}

void MainWindow::onModelFeatures()
{
    doc->onModelFeatures();
}

void MainWindow::onShowAxes()
{
    doc->onShowAxes();
}

void MainWindow::onShowGridLines()
{
    doc->onShowGridLines();
}

void MainWindow::onShowGridShell()
{
    doc->onShowGridShell();
}

void MainWindow::onShowBoundingBox()
{
    doc->onShowBoundingBox();
}

void MainWindow::onShowOverlay()
{
    doc->onShowOverlay();
}

void MainWindow::onShowTime()
{
    doc->onShowTime();
}

void MainWindow::onShowColorBar()
{
    doc->onShowColorBar();
}

void MainWindow::onToolboxGeometry()
{
    doc->onToolboxGeometry();
}

//void MainWindow::setModifiedFlag(bool modified)
//{
//    this->_modifiedFlag = modified;
//}

void MainWindow::updateAllViews()
{
    this->view->mainWidget()->renderWindow()->Render();
}

//// bool MainWindow::saveFile(const char *lpszPathName)
//bool MainWindow::saveFile(const QString &fileName)
//{
//    /*
//    * BOOL CMvDoc::OnSaveDocument(LPCTSTR lpszPathName)
//    *
//    // Note: The base class method CDocument::OnSaveDocument is not called
//    // because serialization is done by the visualization pipeline doc->_manager.
//    // Thus, the method CMvDoc::Serialize is not used in this program.
//
//    // Copy the gui settings from the doc
//    mvGUISettings *gui = new mvGUISettings;
//    m_CropDlg->UpdateData(TRUE);
//    gui->cropBoundsXDelta = m_CropDlg->m_ControlsPage->m_XDelta;
//    gui->cropBoundsYDelta = m_CropDlg->m_ControlsPage->m_YDelta;
//    gui->cropBoundsZDelta = m_CropDlg->m_ControlsPage->m_ZDelta;
//    m_AnimationDlg->m_OptionsPage->UpdateData(TRUE);
//    gui->animationRotate  = m_AnimationDlg->m_OptionsPage->m_Rotate;
//    gui->animationElevate = m_AnimationDlg->m_OptionsPage->m_Elevate;
//    gui->animationDelay   = m_AnimationDlg->m_OptionsPage->m_Delay;
//    m_LightingDlg->m_LightsPage->UpdateData(TRUE);
//    gui->headlightOn             = m_LightingDlg->m_LightsPage->m_HeadlightOn;
//    gui->auxiliaryLightOn        = m_LightingDlg->m_LightsPage->m_AuxiliaryLightOn;
//    gui->headlightIntensity      = m_LightingDlg->m_LightsPage->m_HeadlightIntensity * 0.01;
//    gui->auxiliaryLightIntensity = m_LightingDlg->m_LightsPage->m_AuxiliaryLightIntensity * 0.01;
//    m_LightingDlg->m_BackgroundPage->UpdateData(TRUE);
//    gui->customBackground = m_LightingDlg->m_BackgroundPage->m_Background;
//
//    // Copy the gui settings from the view
//    POSITION pos          = GetFirstViewPosition();
//    CMvView *pView        = (CMvView *)GetNextView(pos);
//    pView->GetViewSettings(gui);
//
//    // The visualization pipeline doc->_manager will serialize everything along
//    // with the gui settings
//    char *errorMsg = m_Manager->Serialize(lpszPathName, gui);
//    delete gui;
//    if (errorMsg != 0)
//    {
//        AfxMessageBox(errorMsg);
//        return FALSE;
//    }
//
//    // Mark this document as saved
//    SetModifiedFlag(FALSE);
//    return TRUE;
//    */
//
//    assert(doc->_gui);
//    mvGUISettings settings(*doc->_gui);
//
//    // @todo replace this with m_CropDlg
//    // @todo replace this with m_AnimationDlg->m_OptionsPage
//    // @todo replace this with m_LightingDlg->m_LightsPage
//    // @todo replace this with m_LightingDlg->m_BackgroundPage
//
//    this->view->getViewSettings(&settings);
//
//    // The visualization pipeline doc->_manager will serialize everything along
//    // with the gui settings
//    ///char *errorMsg = this->doc->_manager->Serialize(lpszPathName, &settings);
//    char *errorMsg = this->doc->_manager->Serialize(fileName.toLocal8Bit().data(), &settings);
//    if (errorMsg != 0)
//    {
//        QMessageBox::information(this, tr(""), tr(errorMsg));
//        return false;
//    }
//    setCurrentFile(fileName);
//    setModifiedFlag(false);
//    return true;
//}

void MainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
    {
        doc->loadFile(action->data().toString());
    }
}

void MainWindow::updateRecentFileActions()
{
    // default settings use QCoreApplication::organizationName() and QCoreApplication::applicationName()
    QSettings   settings;
    QStringList files          = settings.value(MainWindow::recentFilesKey).toStringList();

    int         numRecentFiles = qMin(files.size(), (int) MainWindow::MaxRecentFiles);

    for (int i = 0; i < numRecentFiles; ++i)
    {
        QString text = tr("&%1 %2").arg(i + 1).arg(strippedName(files[i]));
        recentFileActions[i]->setText(text);
        recentFileActions[i]->setData(files[i]);
        recentFileActions[i]->setVisible(true);
    }
    for (int j = numRecentFiles; j < MainWindow::MaxRecentFiles; ++j)
    {
        recentFileActions[j]->setVisible(false);
    }

    recentFileSeparatorAction->setVisible(numRecentFiles > 0);
}
