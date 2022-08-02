#include "mainwindow.h"
#include "ui_mainwindow.h"

//#include <QCloseEvent>
//#include <QDebug.h>
//#include <QFileDialog>
//#include <QMessageBox>
//#include <QSaveFile>
//#include <QScreen>
//#include <QSettings>

//
// vtkMFCWindow*    m_MFCWindow                        ->   QVTKOpenGLNativeWidget*   mainWidget
//                  m_MFCWindow->GetRenderWindow()                                    mainWidget->renderWindow()
//                  m_MFCWindow->GetInteractor()                                      mainWidget->interactor()
//
// CMainFrame                                          ->   MainWindow
//

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

#include "mvManager.h"
#include "mvGUISettings.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    vtkNew<vtkNamedColors> colors;

    doc  = new MvDoc();

    //{{
    //mvManager *    m_Manager = new mvManager;
    //mvGUISettings *gui       = new mvGUISettings;
    manager = new mvManager;
    gui     = new mvGUISettings;

    std::string errorMsg;
    manager->Deserialize("C:\\Users\\charlton\\source\\repos\\modflow\\modelviewer-mf6-latest\\examples\\ex-gwf-csub-p04\\ex-gwf-csub-p04.mvmf6", gui, errorMsg);
    //manager->Deserialize("C:\\Users\\charlton\\source\\repos\\modflow\\modelviewer-mf6-latest\\examples\\ex-gwf-disvmesh\\ex-gwf-disvmesh.mvmf6", gui, errorMsg);
    if (errorMsg.size())
    {
        QMessageBox::information(this, "Error", errorMsg.c_str());
    }
    //delete gui;
    //delete m_Manager;
    //}}

    view = new MvView();
    setCentralWidget(view->mainWidget());

    // 1920x1029
    const QRect availableGeometry = screen()->availableGeometry();
    //resize(availableGeometry.width() / 2, availableGeometry.height() / 2);
    //resize(1920, 1029);
    //resize(1920, 1080);
    //resize(1920, 1123);  // client size (1920x1080)

    ///resize(availableGeometry.width() * 2 / 3, availableGeometry.height() * 2 / 3);

    //resize(1200, 900);
    //setGeometry(100, 100, 1200, 900);
    //view->mainWidget()->resize(1200, 900);
    qDebug() << view->mainWidget()->size();
    qDebug() << "hint " << view->mainWidget()->sizeHint();

    //// Cone
    //vtkNew<vtkConeSource> coneSource;
    //coneSource->Update();
    //vtkNew<vtkPolyDataMapper> coneMapper;
    //coneMapper->SetInputConnection(coneSource->GetOutputPort());

    //vtkNew<vtkActor> coneActor;
    //coneActor->SetMapper(coneMapper);
    //coneActor->GetProperty()->SetColor(colors->GetColor4d("Tomato").GetData());

    //view->addActor(coneActor);

    //{{
    vtkSmartPointer<vtkPropCollection> props = manager->GetPropCollection();
    props->InitTraversal();
    for (int i = 0; i < props->GetNumberOfItems(); i++)
    {
        //m_Renderer->AddViewProp(props->GetNextProp());
        view->addViewProp(props->GetNextProp());
    }
    //}}

    createActions();
    createStatusBar();

    readSettings();

    setCurrentFile(QString());

    //{{
    setDataName(tr("head"));
    setModelName(tr("Modflow 6"));
    //}}
}

MainWindow::~MainWindow()
{
    delete view;
    delete doc;

    delete gui;
    delete manager;
}

void MainWindow::createActions()
{
    QMenu *     fileMenu    = menuBar()->addMenu(tr("&File"));
    
    // New...
    //const QIcon newIcon     = QIcon::fromTheme("document-new", QIcon(":/images/new.png"));
    QAction *   newAct      = new QAction(/* newIcon, */ tr("&New..."), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new mvmv6 file"));
    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);
    fileMenu->addAction(newAct);

    // Close
    QAction *closeAct = new QAction(tr("&Close"), this);
#if !defined(WIN32)
    closeAct->setShortcuts(QKeySequence::Close);
#endif
    closeAct->setStatusTip(tr("Close the current mvmv6 file"));
    ////connect(closeAct, &QAction::triggered, this, &MainWindow::newFile);
    ////connect(closeAct, &QAction::triggered, doc, &MvDoc::closeFile);
    fileMenu->addAction(closeAct);
    closeAct->setEnabled(false);


    // 
    fileMenu->addSeparator();

    // Exit
    //const QIcon exitIcon     = QIcon::fromTheme("application-exit");
    QAction *exitAct = new QAction(/* exitIcon, */ tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Quit the application"));
    connect(exitAct, &QAction::triggered, qApp, &QApplication::closeAllWindows);
    fileMenu->addAction(exitAct);


    QMenu *actionMenu = menuBar()->addMenu(tr("&Action"));

    // Set Size of Display Area...
    QAction *setSizeAct    = new QAction(tr("Set Si&ze of Display Area..."), this);
    setSizeAct->setStatusTip(tr("Set the size of the display area"));
    connect(setSizeAct, &QAction::triggered, this, &MainWindow::setSize);
    actionMenu->addAction(setSizeAct);    
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
    if (maybeSave())
    {
        writeSettings();
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void MainWindow::newFile()
{
    if (maybeSave())
    {
        //textEdit->clear();
        setCurrentFile(QString());
    }
}

bool MainWindow::isModified() const
{
    return true;
}
 
bool MainWindow::maybeSave()
{
    if (!isModified())
        return true;

    QString shownName = curFile;
    if (curFile.isEmpty())
        shownName = "Untitled";

    const QMessageBox::StandardButton ret = QMessageBox::question(this, tr("Model Viewer for Modflow 6"),
                                                                  tr("Save changes to %1?").arg(shownName),
                                                                  QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    switch (ret)
    {
    case QMessageBox::Yes:
        return save();
    case QMessageBox::Cancel:
        return false;
    default:
        break;
    }
    return true;
}

bool MainWindow::save()
{
    if (curFile.isEmpty())
    {
        return saveAs();
    }
    else
    {
        return saveFile(curFile);
    }
}

bool MainWindow::saveAs()
{
    QString     selectedFilter;
    QString     fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save As"),
                                                    tr(""),
                                                    tr("MvMf6 Files (*.mvmf6);;All Files (*.*)"),
                                                    &selectedFilter);

    if (!fileName.isEmpty())
    {
        return saveFile(fileName);
    }
    return false;

    //QFileDialog dialog(this);
    //dialog.setWindowModality(Qt::WindowModal);
    //dialog.setAcceptMode(QFileDialog::AcceptSave);
    //if (dialog.exec() != QDialog::Accepted)
    //    return false;
    //return saveFile(dialog.selectedFiles().first());
}


bool MainWindow::saveFile(const QString &fileName)
{
#if TODO
    QString errorMessage;

    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
    QSaveFile file(fileName);
    if (file.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream out(&file);
        //out << textEdit->toPlainText();
        out << "#TODO" << "\n";
        if (!file.commit())
        {
            errorMessage = tr("Cannot write file %1:\n%2.")
                               .arg(QDir::toNativeSeparators(fileName), file.errorString());
        }
    }
    else
    {
        errorMessage = tr("Cannot open file %1 for writing:\n%2.")
                           .arg(QDir::toNativeSeparators(fileName), file.errorString());
    }
    QGuiApplication::restoreOverrideCursor();

    if (!errorMessage.isEmpty())
    {
        QMessageBox::warning(this, tr("Application"), errorMessage);
        return false;
    }

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
#else
    ////return doc->saveFile(fileName);
    QString errorMessage;

    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
    QSaveFile file(fileName);
    if (file.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream out(&file);
        //out << textEdit->toPlainText();
        out << "#TODO"
            << "\n";
        if (!file.commit())
        {
            errorMessage = tr("Cannot write file %1:\n%2.")
                               .arg(QDir::toNativeSeparators(fileName), file.errorString());
        }
    }
    else
    {
        errorMessage = tr("Cannot open file %1 for writing:\n%2.")
                           .arg(QDir::toNativeSeparators(fileName), file.errorString());
    }
    QGuiApplication::restoreOverrideCursor();

    if (!errorMessage.isEmpty())
    {
        QMessageBox::warning(this, tr("Application"), errorMessage);
        return false;
    }

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;

#endif

}

void MainWindow::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    ///textEdit->document()->setModified(false);
    setWindowModified(false);

    QString shownName = curFile;
    if (curFile.isEmpty())
        shownName = "Untitled";
    setWindowFilePath(shownName);
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void MainWindow::readSettings()
{
    ///QSettings        settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    QSettings        settings(tr("USGS"), tr("QModel Viewer for Modflow 6"));
    const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
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
    ///QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    QSettings settings(tr("USGS"), tr("QModel Viewer for Modflow 6"));
    settings.setValue("geometry", saveGeometry());
}

void MainWindow::setSize()
{
    qDebug() << "MainWindow size: "     << this->size();
    qDebug() << "MainWindow geometry: " << this->geometry();
    qDebug() << "mainWidget size: "     << view->mainWidget()->size();
    qDebug() << "mainWidget geometry: " << view->mainWidget()->geometry();

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
