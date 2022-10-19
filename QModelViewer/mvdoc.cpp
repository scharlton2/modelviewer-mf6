#include "mvdoc.h"

#include <algorithm>

#if defined(Q_OS_WIN) && !defined(NDEBUG)
#include <windows.h>        // for testing RGB macro
#endif

#include <QAction>
#include <QApplication>
#include <QDir>
#include <QFileDialog>
#include <QMainWindow>
#include <QMessageBox>
#include <QSettings>

#include <vtkPropCollection.h>

#include "mainwindow.h"
#include "datafilesdialog.h"

#include "Modflow6DataSource.h"

#include "mvview.h"

#include "mvGUISettings.h"
#include "mvManager.h"

#include "colorbardialog.h"
#include "datadialog.h"
#include "geometrydialog.h"
#include "griddialog.h"
#include "lightingdialog.h"

#include "preferencesdialog.h"


MvDoc::MvDoc(QMainWindow* parent)
    : QObject{parent}
    , _gui{nullptr}
    , _isAnimating{false}
    , _modified{false}
    , _interactorStyle{MouseMode::mmTrackball}
    , numberOfModels{1}
{
    // Initialize variables
    startup        = true;
    projectionMode = ProjectionType::ptPerspective;
    readyToClose   = false;
    animationType  = AnimationType::atTime;
    animationSteps = 10;

    defaultXOrigin = 0.;
    defaultYOrigin = 0.;
    defaultAngRot  = 0.;

    QSettings settings;
    _interactorStyle = static_cast<enum MouseMode>(settings.value("interactorStyle", (int)MouseMode::mmTrackball).toInt());

#if TODO
    // Get the models supported by this application
    m_NumberOfModels = mvModelList::GetNumberOfModels();
    m_ModelNames     = new char*[m_NumberOfModels];
    for (int i = 0; i < m_NumberOfModels; i++)
    {
        m_ModelNames[i] = new char[30];
    }
    mvModelList::GetModelNames(m_ModelNames);

    // Load presistent app settings from Windows registry
    LoadPreviousAppSettings();

#endif

    // Create the visualization pipeline manager
    _manager = new mvManager;

#if TODO
    // Set modeless dialog boxes to null. These cannot be created
    // until after the main frame window is created.
    m_GeometryDlg      = NULL;
    m_DataDlg          = NULL;
    m_SolidDlg         = NULL;
    m_IsosurfaceDlg    = NULL;
    m_VectorDlg        = NULL;
    m_PathlinesDlg     = NULL;
    m_ModelFeaturesDlg = NULL;
    m_CropDlg          = NULL;
    m_AnimationDlg     = NULL;
    m_OverlayDlg       = NULL;
#endif

    dataDialog     = new DataDialog(parent, this);
    ///dataDialog->reinitialize();

    colorBarDialog = new ColorBarDialog(parent, this);
    ///colorBarDialog->reinitialize();

    lightingDialog = new LightingDialog(parent, this);

    gridDialog     = new GridDialog(parent, this);

    geometryDialog = new GeometryDialog(parent, this);
    ///geometryDialog->reinitialize();

    reinitializeToolDialogs();

    // Note: views have not been created yet they depend on MvDoc
}

MvDoc::~MvDoc()
{
    // Save persistent app settings to Windows registry
    saveCurrentAppSettings();

    // Free memory
    delete _manager;
    delete _gui;

    /***
    for (int i = 0; i < m_NumberOfModels; i++)
    {
        delete[] m_ModelNames[i];
    }
    delete[] m_ModelNames;
***/
    // Note that modeless dialog boxes delete themselves.
}

/////////////////////////////////////////////////////////////////////////////
// Load and save application settings

void MvDoc::loadPreviousAppSettings()
{
#if TODO
    CString directory = AfxGetApp()->GetProfileString("Settings", "Directory");
    m_InteractorStyle = AfxGetApp()->GetProfileInt("Settings", "Interactor Style", 0);
    m_DefaultModel    = AfxGetApp()->GetProfileString("Settings", "Default Model");
    _chdir(directory.GetBuffer(10));
#endif
}

void MvDoc::saveCurrentAppSettings()
{
#if TODO
    if (m_strPathName.GetLength() > 0)
    {
        int count = m_strPathName.ReverseFind('\\');
        if (m_strPathName.GetAt(count - 1) == ':')
        {
            count++;
        }
        CString directory = m_strPathName.Left(count);

        AfxGetApp()->WriteProfileString("Settings", "Directory",
                                        (LPCTSTR)directory);
    }
    AfxGetApp()->WriteProfileInt("Settings", "Interactor Style", m_InteractorStyle);
    AfxGetApp()->WriteProfileString("Settings", "Default Model", (LPCTSTR)m_DefaultModel);
#endif
}

//const QString& MvDoc::pathName() const
//{
//    return _pathName;
//}

MouseMode MvDoc::interactorStyle() const
{
    return _interactorStyle;
}

QString MvDoc::modelName() const
{
    return _manager->GetModelName();
}

QString MvDoc::activeScalarName() const
{
    return _manager->GetActiveScalarDataName();
}

vtkSmartPointer<vtkPropCollection> MvDoc::propCollection()
{
    return _manager->GetPropCollection();
}

bool MvDoc::isAnimating() const
{
    return _isAnimating;
}

QStringList MvDoc::timePointLabels()
{
    QStringList          slist;
    int                  n      = _manager->GetNumberOfTimePoints();
    char**               labels = _manager->GetTimePointLabels();
    for (int i = 0; i < n; ++i)
    {
        slist.append(labels[i]);
    }
    return slist;
}

QStringList MvDoc::dataTypeLabels()
{
    QStringList slist;
    char**      labels = _manager->GetDataTypeLabels();
    int         n      = _manager->GetNumberOfScalarDataTypes();
    for (int i = 0; i < n; ++i)
    {
        slist.append(labels[i]);
    }
    return slist;
}

/////////////////////////////////////////////////////////////////////////////
// File->

void MvDoc::onFileClose()
{
    if (!maybeSave())
    {
        return;
    }

    delete _manager;
    delete _gui;

    QApplication::setOverrideCursor(Qt::WaitCursor);

    _manager = new mvManager();
    _gui     = new mvGUISettings();

    for (auto view : _views)
    {
        //view->renderer->GetViewProps()->RemoveAllItems();
        view->removeAllViewProps();
        vtkSmartPointer<vtkPropCollection> props = _manager->GetPropCollection();
        props->InitTraversal();
        for (int i = 0; i < props->GetNumberOfItems(); i++)
        {
            view->addViewProp(props->GetNextProp());
        }
        view->discardSavedViewpoint();
        view->applyViewSettings(_gui);
    }

    reinitializeToolDialogs();
    setCurrentFile("");

    QApplication::restoreOverrideCursor();
}

void MvDoc::reinitializeToolDialogs()
{
    dataDialog->reinitialize();
    colorBarDialog->reinitialize();
    //m_LightingDlg->Reinitialize();
    gridDialog->reinitialize();
    geometryDialog->reinitialize();
    //m_SolidDlg->Reinitialize();
    //m_IsosurfaceDlg->Reinitialize();
    //m_VectorDlg->Reinitialize();
    //m_VectorDlg->ShowWindow(SW_HIDE);
    //m_PathlinesDlg->Reinitialize();
    //m_PathlinesDlg->ShowWindow(SW_HIDE);
    //m_ModelFeaturesDlg->Reinitialize();
    //m_ModelFeaturesDlg->ShowWindow(SW_HIDE);
    //m_OverlayDlg->Reinitialize();
    //m_CropDlg->Reinitialize();
    //m_AnimationDlg->Reinitialize();
}

void MvDoc::onFileNew()
{
    MainWindow* mainWindow = dynamic_cast<MainWindow*>(parent());
    assert(mainWindow);

    delete _manager;
    _manager = new mvManager;

    //dataDialog->reinitialize();
    //colorBarDialog->reinitialize();
    //geometryDialog->reinitialize();
    reinitializeToolDialogs();

    char selectedModel[20];
    // This version of Model View is customized to display only Modflow 6 results.
    // In other words, the variable "selectedModel" is set to "Modflow 6"
    strcpy(selectedModel, Modflow6DataSource::GetNameStatic());

    // Display dialog box for user to specify data files for the
    // selected model. Note that memory for dataFileList is allocated
    // by the dataFileList method
    char* dataFileList = DataFilesDialog::GetDataFileList(selectedModel, mainWindow);
    if (dataFileList == nullptr)
    {
        return; // User clicked the Cancel button
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    // Load data
    const char* errorMsg = _manager->LoadData(selectedModel, dataFileList);
    delete[] dataFileList;

    // Check for error in reading data files
    if (errorMsg != 0)
    {
        QApplication::restoreOverrideCursor();
        QMessageBox::critical(mainWindow, "", errorMsg);
        return;
    }

    // Display dialog box for user to select time and type of data to view
    // @todo
    _manager->SetTimePointTo(0);
    _manager->SetScalarDataTypeTo(0);

    // Apply default settings and then turn on bounding box
    _manager->ApplyDefaultSettings();

    // If there are more than one time points, turn on the
    // time label
    if (_manager->GetNumberOfTimePoints() > 1)
    {
        _manager->ShowTimeLabel();
    }

    // Create a gui settings object with initial settings. These
    // are used to update the tool dialog boxes and view.
    mvGUISettings* gui = new mvGUISettings;

    // Update the tool (modeless) dialog boxes
    updateToolDialogs(gui);

    for (auto view : _views)
    {
        view->removeAllViewProps();
        vtkSmartPointer<vtkPropCollection> props = _manager->GetPropCollection();
        props->InitTraversal();
        for (int i = 0; i < props->GetNumberOfItems(); i++)
        {
            view->addViewProp(props->GetNextProp());
        }
        view->setProjectionToPerspective();
        view->discardSavedViewpoint();
        view->applyViewSettings(gui);
        view->resetViewPoint();
        view->onUpdate(nullptr, nullptr);
    }
    //delete gui;
    _gui = gui;

    // Mark this document as modified
    setModified(true);

    // Redraw the view;
    //updateAllViews(nullptr);

    QApplication::restoreOverrideCursor();
}


void MvDoc::onFileOpen()
{
    MainWindow* mainWindow = dynamic_cast<MainWindow*>(parent());
    assert(mainWindow);

    //if (!mainWindow->maybeSave()) return;
    if (!maybeSave()) return;

    QString fileName = QFileDialog::getOpenFileName(mainWindow, tr("Open"), QString(), tr("MvMf6 Files (*.mvmf6)"));

    if (fileName.isEmpty())
    {
        return;
    }

#if 0
    //{{ application MainWindow::loadFile(QString fileName) -> MvDoc::openDocument(QString fileName, QWidget* parent = nullptr)
    delete _manager;
    delete _gui;

    QApplication::setOverrideCursor(Qt::WaitCursor);

    std::string errorMsg;
    _gui     = new mvGUISettings();
    _manager = new mvManager();
    _manager->Deserialize(QDir::toNativeSeparators(fileName).toLocal8Bit().data(), _gui, errorMsg);
    if (errorMsg.size())
    {
        QMessageBox::information(mainWindow, "Error", errorMsg.c_str());
        return;
    }

    for (auto view : _views)
    {
        view->removeAllViewProps();
        vtkSmartPointer<vtkPropCollection> props = _manager->GetPropCollection();
        props->InitTraversal();
        for (int i = 0; i < props->GetNumberOfItems(); i++)
        {
            view->addViewProp(props->GetNextProp());
        }
        view->applyViewSettings(_gui);
        view->onUpdate(nullptr, nullptr);
    }

    //{{
    updateToolDialogs(_gui);
    //}}
    //mainWindow->updateActions();
    mainWindow->updateStatusBar();
    setCurrentFile(fileName);

    QApplication::restoreOverrideCursor();

#else
    loadFile(fileName);
#endif
}

void MvDoc::loadFile(const QString& fileName)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    MainWindow* mainWindow = dynamic_cast<MainWindow*>(parent());
    assert(mainWindow);

    delete _manager;
    delete _gui;

    std::string errorMsg;
    _gui     = new mvGUISettings();
    _manager = new mvManager();
    _manager->Deserialize(QDir::toNativeSeparators(fileName).toLocal8Bit().data(), _gui, errorMsg);
    if (errorMsg.size())
    {
        QMessageBox::information(mainWindow, "Error", errorMsg.c_str());
        return;
    }

    for (auto view : _views)
    {
        view->removeAllViewProps();
        vtkSmartPointer<vtkPropCollection> props = _manager->GetPropCollection();
        props->InitTraversal();
        for (int i = 0; i < props->GetNumberOfItems(); i++)
        {
            view->addViewProp(props->GetNextProp());
        }
        view->discardSavedViewpoint();
        view->applyViewSettings(_gui);
        view->onUpdate(nullptr, nullptr);
    }

    updateToolDialogs(_gui);
    // mainWindow->updateActions();
    mainWindow->updateStatusBar();
    setCurrentFile(fileName);

    QApplication::restoreOverrideCursor();
}

// BOOL CMvDoc::OnSaveDocument(LPCTSTR lpszPathName)
bool MvDoc::saveFile(const QString& fileName)
{
    assert(_gui);
    mvGUISettings settings(*_gui);

    // @todo replace this with m_CropDlg
    // @todo replace this with m_AnimationDlg->m_OptionsPage
    lightingDialog->updateDataLights(true);
    settings.headlightOn             = lightingDialog->headlightOn ? 1 : 0;
    settings.auxiliaryLightOn        = lightingDialog->auxiliaryLightOn ? 1 : 0;
    settings.headlightIntensity      = lightingDialog->headlightIntensity * 0.01;
    settings.auxiliaryLightIntensity = lightingDialog->auxiliaryLightIntensity * 0.01;
    lightingDialog->updateDataBackground(true);
    settings.customBackground = lightingDialog->customBackground ? 1 : 0;

    _views.front()->getViewSettings(&settings);

    // The visualization pipeline doc->_manager will serialize everything along
    // with the gui settings
    ///char *errorMsg = this->doc->_manager->Serialize(lpszPathName, &settings);
    char *errorMsg = _manager->Serialize(fileName.toLocal8Bit().data(), &settings);
    if (errorMsg != 0)
    {
        QWidget* widget = dynamic_cast<QWidget*>(parent());
        assert(widget);
        QMessageBox::information(widget, tr(""), tr(errorMsg));
        return false;
    }
    setCurrentFile(fileName);
    setModified(false);
    return true;
}

bool MvDoc::maybeSave()
{
    if (!this->isModified())
        return true;

    QString shownName = this->_currentFile;
    if (shownName.isEmpty())
    {
        shownName = "Untitled";
    }
    else
    {
        QFileInfo info(shownName);
        shownName = info.fileName();
    }

    //QWidget*                          widget = dynamic_cast<QWidget*>(parent());
    //assert(widget);
    MainWindow* mainWindow = dynamic_cast<MainWindow*>(parent());
    assert(mainWindow);
    const QMessageBox::StandardButton ret = QMessageBox::question(mainWindow,
                                                                  tr("Model Viewer for Modflow 6"),
                                                                  tr("Save changes to %1?").arg(shownName),
                                                                  QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    switch (ret)
    {
    case QMessageBox::Yes:
        return mainWindow->onFileSave();
    case QMessageBox::Cancel:
        return false;
    default:
        break;
    }
    return true;
}

void MvDoc::onPreferences()
{
    MainWindow* mainWindow = dynamic_cast<MainWindow*>(parent());
    assert(mainWindow);

    PreferencesDialog dlg(_interactorStyle, mainWindow);
    if (dlg.exec() != QDialog::Accepted)
    {
        return;
    }

    _interactorStyle = dlg.interactorStyle;
    for (auto view : _views)
    {
        view->setInteractorStyle(_interactorStyle);
    }

    QSettings settings;
    settings.setValue("interactorStyle", (int)_interactorStyle);
}

/////////////////////////////////////////////////////////////////////////////
// Time Label

void MvDoc::SetTimeLabelFontSize(int size, bool update /* = true */)
{
    _manager->SetTimeLabelFontSize(size);
    if (update)
    {
        updateAllViews(nullptr);
        setModified(true);
    }
}

int MvDoc::GetTimeLabelFontSize() const
{
    return _manager->GetTimeLabelFontSize();
}

void MvDoc::SetTimeLabelPosition(double x, double y, bool update /* = true */)
{
    _manager->SetTimeLabelPosition(x, y);
    if (update)
    {
        updateAllViews(nullptr);
        setModified(true);
    }
}

const double* MvDoc::GetTimeLabelPosition() const
{
    return _manager->GetTimeLabelPosition();
}


/////////////////////////////////////////////////////////////////////////////
// Color Bar

void MvDoc::setColorBarEndPoints(double valueBlue, double valueRed)
{
    _manager->SetColorBarEndPoints(valueBlue, valueRed);
    updateAllViews(nullptr);
    setModified(true);
}

void MvDoc::UseLinearColorBar()
{
    _manager->UseLinearColorBar();
    updateAllViews(nullptr);
    setModified(true);
}

void MvDoc::UseLogColorBar()
{
    _manager->UseLogColorBar();
    updateAllViews(nullptr);
    setModified(true);
}

void MvDoc::SetColorBarSize(int width, int height, int offset, bool update /*= true*/)
{
    _manager->SetColorBarWidth(width);
    _manager->SetColorBarHeight(height);
    _manager->SetColorBarOffset(offset);
    if (update)
    {
        updateAllViews(nullptr);
        setModified(true);
    }
}

void MvDoc::SetColorBarFontSize(int fontSize, bool update /*= true*/)
{
    _manager->SetColorBarFontSize(fontSize);
    if (update)
    {
        updateAllViews(nullptr);
        setModified(true);
    }
}

void MvDoc::SetColorBarNumberOfLabels(int numLabels, bool update /*= true*/)
{
    _manager->SetColorBarNumberOfLabels(numLabels);
    if (update)
    {
        updateAllViews(nullptr);
        setModified(true);
    }
}

void MvDoc::SetColorBarLabelPrecision(int precision, bool update /*= true*/)
{
    _manager->SetColorBarLabelPrecision(precision);
    if (update)
    {
        updateAllViews(nullptr);
        setModified(true);
    }
}

void MvDoc::SetColorBarTextColor(double red, double green, double blue, bool update /*= true*/)
{
    _manager->SetColorBarTextColor(red, green, blue);
    if (update)
    {
        updateAllViews(nullptr);
        setModified(true);
    }
}

void MvDoc::SetColorBarColorScheme(int Value)
{
    _manager->SetColorBarColorScheme(Value);
    updateAllViews(nullptr);
    setModified(true);
}

int MvDoc::GetColorBarWidth()
{
    return _manager->GetColorBarWidth();
}

int MvDoc::GetColorBarHeight()
{
    return _manager->GetColorBarHeight();
}

int MvDoc::GetColorBarOffset()
{
    return _manager->GetColorBarOffset();
}

int MvDoc::GetColorBarFontSize()
{
    return _manager->GetColorBarFontSize();
}

int MvDoc::GetColorBarColorScheme()
{
    return _manager->GetColorBarColorScheme();
}

std::uint32_t MvDoc::GetColorBarFirstCustomColor()
{
    return _manager->GetColorBarFirstCustomColor();
}

std::uint32_t MvDoc::GetColorBarLastCustomColor()
{
    return _manager->GetColorBarLastCustomColor();
}

void MvDoc::SetColorBarFirstCustomColor(std::uint32_t value)
{
    _manager->SetColorBarFirstCustomColor(value);
    updateAllViews(nullptr);
    setModified(true);
}

void MvDoc::SetColorBarFirstCustomColor(QColor color)
{
#if defined(Q_OS_WIN) && !defined(NDEBUG)
    assert(RGB(color.red(), color.green(), color.blue()) == qRgba(color.red(), color.green(), color.blue(), 0));
#endif
    SetColorBarFirstCustomColor(qRgba(color.red(), color.green(), color.blue(), 0));    
}

void MvDoc::SetColorBarLastCustomColor(std::uint32_t value)
{
    _manager->SetColorBarLastCustomColor(value);
    updateAllViews(nullptr);
    setModified(true);
}

void MvDoc::SetColorBarLastCustomColor(QColor color)
{
#if defined(Q_OS_WIN) && !defined(NDEBUG)
    assert(RGB(color.red(), color.green(), color.blue()) == qRgba(color.red(), color.green(), color.blue(), 0));
#endif
    SetColorBarLastCustomColor(qRgba(color.red(), color.green(), color.blue(), 0));
}

int MvDoc::getColorBarSource()
{
    return _manager->GetColorBarSource();
}

void MvDoc::setColorBarSource(int value)
{
    _manager->SetColorBarSource(value);
}

double MvDoc::getColorBarValueBlue() const
{
    return _manager->GetColorBarValueBlue();
}

double MvDoc::getColorBarValueRed() const
{
    return _manager->GetColorBarValueRed();
}

bool MvDoc::modified() const
{
    return _modified;
}

bool MvDoc::isModified() const
{
    return _modified;
}

void MvDoc::setModified(bool modified)
{
    _modified = modified;
}

void MvDoc::addView(QAbstractView* view)
{
    assert(view);
    auto it = std::find(_views.begin(), _views.end(), view);
    assert(it == _views.end());
    _views.push_back(view);
}

void MvDoc::removeView(QAbstractView* view)
{
    assert(view);
    auto it = std::find(_views.begin(), _views.end(), view);
    assert(it != _views.end());
    _views.remove(view);
}

void MvDoc::updateAllViews(QAbstractView* sender, QObject* hint)
{
    for (auto& view : _views)
    {
        if (view != sender) view->onUpdate(sender, hint);
    }
}

/////////////////////////////////////////////////////////////////////////////
// Show menu

void MvDoc::onShowNone()
{
    // @todo
    /*
    m_Manager->HideScalarData();
    m_IsosurfaceDlg->Activate(FALSE);
    m_SolidDlg->Activate(FALSE);
    m_CropDlg->Activate(FALSE);
    UpdateAllViews(NULL);
    setModified(TRUE);
    */

    _manager->HideScalarData();
    //m_IsosurfaceDlg->Activate(FALSE);
    //m_SolidDlg->Activate(FALSE);
    //m_CropDlg->Activate(FALSE);
    updateAllViews(nullptr);
    setModified(true);
}

void MvDoc::onShowSolid()
{
    // @todo
    /*
    m_Manager->ShowScalarDataAsSolid();
    m_IsosurfaceDlg->Activate(FALSE);
    m_SolidDlg->Activate(TRUE);
    m_CropDlg->Activate(TRUE);
    UpdateAllViews(NULL);
    setModified(TRUE);
    */

    _manager->ShowScalarDataAsSolid();
    //m_IsosurfaceDlg->Activate(FALSE);
    //m_SolidDlg->Activate(TRUE);
    //m_CropDlg->Activate(TRUE);
    updateAllViews(nullptr);
    setModified(true);
}

void MvDoc::onShowIsosurfaces()
{
    // @todo
    /*
    m_Manager->ShowScalarDataAsIsosurfaces();
    m_IsosurfaceDlg->Activate(TRUE);
    if (m_Manager->UsingRegularIsosurfaces())
    {
        m_IsosurfaceDlg->m_PropertySheet->SetActivePage(0);
    }
    else
    {
        m_IsosurfaceDlg->m_PropertySheet->SetActivePage(1);
    }
    m_SolidDlg->Activate(FALSE);
    m_CropDlg->Activate(TRUE);
    UpdateAllViews(NULL);
    setModified(TRUE);
    */

    _manager->ShowScalarDataAsIsosurfaces();
    //m_IsosurfaceDlg->Activate(TRUE);
    if (_manager->UsingRegularIsosurfaces())
    {
        //m_IsosurfaceDlg->m_PropertySheet->SetActivePage(0);
    }
    else
    {
        //m_IsosurfaceDlg->m_PropertySheet->SetActivePage(1);
    }
    //m_SolidDlg->Activate(FALSE);
    //m_CropDlg->Activate(TRUE);
    updateAllViews(nullptr);
    setModified(true);
}

void MvDoc::onShowVectors(QWidget* parent)
{
    /*
    if (m_Manager->AreVectorsVisible())
    {
        m_Manager->HideVectors();
        m_VectorDlg->Activate(FALSE);
    }
    else
    {
        if (m_Manager->GetVectorScaleFactor() == 0)
        {
            AfxMessageBox("Warning--Vector scale factor is currently set to zero!");
        }
        m_Manager->ShowVectors();
        m_VectorDlg->Activate(TRUE);
    }
    UpdateAllViews(NULL);
    setModified(TRUE);
    */

    if (_manager->AreVectorsVisible())
    {
        _manager->HideVectors();
        //m_VectorDlg->Activate(FALSE);
    }
    else
    {
        if (_manager->GetVectorScaleFactor() == 0)
        {
            QMessageBox::warning(parent, tr("Warning"), tr("Vector scale factor is currently set to zero!"));
        }
        _manager->ShowVectors();
        //m_VectorDlg->Activate(TRUE);
    }
    updateAllViews(nullptr);
    setModified(true);
}

void MvDoc::onShowPathlines()
{
    /*
    if (m_Manager->ArePathlinesVisible())
    {
        m_Manager->HidePathlines();
        m_PathlinesDlg->Activate(FALSE);
    }
    else
    {
        m_Manager->ShowPathlines();
        m_PathlinesDlg->Activate(TRUE);
    }
    UpdateAllViews(NULL);
    setModified(TRUE);
    */

    if (_manager->ArePathlinesVisible())
    {
        _manager->HidePathlines();
        //m_PathlinesDlg->Activate(FALSE);
    }
    else
    {
        _manager->ShowPathlines();
        //m_PathlinesDlg->Activate(TRUE);
    }
    updateAllViews(nullptr);
    setModified(true);
}

void MvDoc::onModelFeatures()
{
    if (this->_manager->AreModelFeaturesVisible())
    {
        this->_manager->HideModelFeatures();
        ///m_ModelFeaturesDlg->Activate(FALSE);
    }
    else
    {
        this->_manager->ShowModelFeatures();
        //m_ModelFeaturesDlg->Activate(TRUE);
        //if ((!m_ModelFeaturesDlg->IsWindowVisible()) &&
        //    (m_ModelFeaturesDlg->m_ShowListBox.GetCount() == 0))
        //{
        //    m_ModelFeaturesDlg->ShowWindow(SW_SHOW);
        //}
    }
    updateAllViews(nullptr);
    setModified(true);
}

void MvDoc::setCurrentFile(const QString& fileName)
{
    _currentFile = QDir::toNativeSeparators(fileName);
    setModified(false);

    MainWindow* mainWindow = dynamic_cast<MainWindow*>(parent());
    assert(mainWindow);
    if (mainWindow)
    {
        if (!fileName.isEmpty())
        {
            // default settings use QCoreApplication::organizationName() and QCoreApplication::applicationName()
            QSettings   settings;
            QStringList files = settings.value(MainWindow::recentFilesKey).toStringList();
            files.removeAll(fileName);
            files.prepend(fileName);
            while (files.size() > MainWindow::MaxRecentFiles)
                files.removeLast();

            settings.setValue(MainWindow::recentFilesKey, files);
            mainWindow->updateRecentFileActions();
        }

        mainWindow->setWindowModified(false);

        QString shownName = _currentFile;
        if (shownName.isEmpty())
            shownName = tr("Untitled");
        mainWindow->setWindowFilePath(shownName);
    }
    updateAllViews(nullptr);
}

QString MvDoc::currentFile()
{
    return _currentFile;
}

QDir MvDoc::defaultDir() const
{
    if (!_currentFile.isEmpty())
    {
        QFileInfo fileInfo(_currentFile);
        return fileInfo.absoluteDir();
    }
    return QDir();
}

void MvDoc::onShowAxes()
{
    if (_manager->AreAxesVisible())
    {
        _manager->HideAxes();
        geometryDialog->activateAxes(false);
    }
    else
    {
        _manager->ShowAxes();
        geometryDialog->activateAxes(true);
    }
    updateAllViews(nullptr);
    setModified(true);
}

void MvDoc::onShowGridLines()
{
    if (_manager->GetGridType() == GridType::MV_STRUCTURED_GRID)
    {
        if (_manager->AreActivatedGridLinesVisible())
        {
            _manager->HideGridLines();
            //m_GridDlg->m_ApplyButton.EnableWindow(FALSE);
            gridDialog->activateLines(false);
        }
        else
        {
            _manager->ShowActivatedGridLines();
            //m_GridDlg->m_ApplyButton.EnableWindow(
            //m_GridDlg->m_PropertySheet->GetActiveIndex() == 0);
            gridDialog->activateLines(true);
        }
    }
    else if (_manager->GetGridType() == GridType::MV_LAYERED_GRID)
    {
        if (_manager->IsGridLayerVisible())
        {
            _manager->HideGridLayer();
            //m_GridDlg->m_ApplyButton.EnableWindow(FALSE);
            gridDialog->activateLines(false);
        }
        else
        {
            _manager->ShowGridLayer();
            //m_GridDlg->m_ApplyButton.EnableWindow(TRUE);
            gridDialog->activateLines(true);
        }
    }
    updateAllViews(nullptr);
    setModified(true);
}

void MvDoc::onShowGridShell()
{
    if (_manager->IsGridShellVisible())
    {
        _manager->HideGridShell();
        gridDialog->activateShell(false);
    }
    else
    {
        _manager->ShowGridShell();
        gridDialog->activateShell(true);
    }
    updateAllViews(nullptr);
    setModified(true);
}

void MvDoc::onShowBoundingBox()
{
    if (_manager->IsBoundingBoxVisible())
    {
        _manager->HideBoundingBox();
        geometryDialog->activateBoundingBox(false);
    }
    else
    {
        _manager->ShowBoundingBox();
        geometryDialog->activateBoundingBox(true);
    }
    updateAllViews(nullptr);
    setModified(true);
}

void MvDoc::onShowOverlay()
{
    assert(_manager->HasOverlay());
    if (_manager->IsOverlayVisible())
    {
        _manager->HideOverlay();
    }
    else
    {
        _manager->ShowOverlay();
    }
    updateAllViews(nullptr);
    setModified(true);
}

void MvDoc::onShowTime()
{
    if (_manager->IsTimeLabelVisible())
    {
        _manager->HideTimeLabel();
    }
    else
    {
        _manager->ShowTimeLabel();
    }
    updateAllViews(nullptr);
    setModified(true);
}

void MvDoc::onShowColorBar()
{
    if (_manager->IsColorBarVisible())
    {
        _manager->HideColorBar();
    }
    else
    {
        _manager->ShowColorBar();
    }
    updateAllViews(nullptr);
    setModified(true);
}

/////////////////////////////////////////////////////////////////////////////
// Command handlers -- Action Menu

void MvDoc::onPerspectiveProjection()
{
    projectionMode   = ProjectionType::ptPerspective;
    for (auto view : _views)
    {
        view->setProjectionToPerspective();
    }
    updateAllViews(nullptr, nullptr);
}

void MvDoc::onUpdatePerspectiveProjection(QAction* action)
{
    action->setChecked(projectionMode == ProjectionType::ptPerspective);
    action->setEnabled(_manager->GetDataFileList() && !_isAnimating);
}

void MvDoc::onParallelProjection()
{
    projectionMode   = ProjectionType::ptParallel;
    //POSITION pos     = GetFirstViewPosition();
    //CMvView* pView   = (CMvView*)GetNextView(pos);
    //pView->SetProjectionToParallel();
    //UpdateAllViews(NULL);

    for (auto view : _views)
    {
        view->setProjectionToParallel();
    }
    updateAllViews(nullptr, nullptr);
}

void MvDoc::onUpdateParallelProjection(QAction* action)
{
    action->setChecked(projectionMode == ProjectionType::ptParallel);
    action->setEnabled(_manager->GetDataFileList() && !_isAnimating);
}

/////////////////////////////////////////////////////////////////////////////
// Command handlers -- Tools Menu


void MvDoc::setTimePointTo(int timePointIndex)
{
    //QApplication::setOverrideCursor(Qt::WaitCursor);
    _manager->SetTimePointTo(timePointIndex);
    updateAnimation();
}

void MvDoc::updateAnimation()
{
    ///POSITION pos = GetFirstViewPosition();
    ///GetNextView(pos)->SendMessage(WM_PAINT);
    
    for (auto view : _views)
    {
        view->onUpdate(nullptr, nullptr);

        //{{
        // added so that File->Export Animation->Preview would update the view
        // this seems to do what 'GetNextView(pos)->SendMessage(WM_PAINT)' did in the MFC version
        qApp->processEvents();
        //}}
    }

    double range[2];
    _manager->GetScalarDataRange(range);
    //m_DataDlg->m_ScalarPage->SetRange(range);
    _manager->GetVectorMagnitudeRange(range);
    //m_DataDlg->m_VectorPage->SetRange(range);
    //m_AnimationDlg->m_ControlsPage->SetAndDisplayCurrentTime(_manager->GetCurrentTimePointIndex());

    if (!_isAnimating)
    {
        bool          b;
        switch (animationType)
        {
        case AnimationType::atTime:
            b = _manager->GetCurrentTimePointIndex() < _manager->GetNumberOfTimePoints() - 1;
            break;
        case AnimationType::atSpace:
            b = true;
            break;
        }

        //m_AnimationDlg->m_ControlsPage->GetDlgItem(IDC_RUN_ANIMATION)->EnableWindow(b);
        //m_AnimationDlg->m_ControlsPage->GetDlgItem(IDC_ADVANCE_ANIMATION)->EnableWindow(b);
        ///EndWaitCursor();
    }
}

void MvDoc::advanceOneTimePoint()
{
    if (!_isAnimating)
    {
        ///BeginWaitCursor();
    }
    _manager->AdvanceOneTimePoint();
    updateAnimationPosition();
}

void MvDoc::updateAnimationPosition()
{
    //POSITION pos   = GetFirstViewPosition();
    //CMvView* pView = (CMvView*)GetNextView(pos);
    //pView->RotateCamera(m_AnimationDlg->m_OptionsPage->m_Rotate);
    //pView->ElevateCamera(m_AnimationDlg->m_OptionsPage->m_Elevate);

    for(auto view : this->_views)
    {
        //pView->RotateCamera(m_AnimationDlg->m_OptionsPage->m_Rotate);
        //pView->ElevateCamera(m_AnimationDlg->m_OptionsPage->m_Elevate);

        //view->rotateCamera();
        //view->elevateCamera();
    }
    updateAnimation();
}

//void MvDoc::updateAnimation()
//{
//    //POSITION pos = GetFirstViewPosition();
//    //GetNextView(pos)->SendMessage(WM_PAINT);
//
//    double range[2];
//    _manager->GetScalarDataRange(range);
//    //m_DataDlg->m_ScalarPage->SetRange(range);
//    _manager->GetVectorMagnitudeRange(range);
//    //m_DataDlg->m_VectorPage->SetRange(range);
//    //m_AnimationDlg->m_ControlsPage->SetAndDisplayCurrentTime(_manager->GetCurrentTimePointIndex());
//
//    if (!_isAnimating)
//    {
//        bool          b;
//        AnimationType at = animationType;
//        switch (at)
//        {
//        case AnimationType::atTime:
//            b = _manager->GetCurrentTimePointIndex() < _manager->GetNumberOfTimePoints() - 1;
//            break;
//        case AnimationType::atSpace:
//            b = true;
//            break;
//        }
//
//        //m_AnimationDlg->m_ControlsPage->GetDlgItem(IDC_RUN_ANIMATION)->EnableWindow(b);
//        //m_AnimationDlg->m_ControlsPage->GetDlgItem(IDC_ADVANCE_ANIMATION)->EnableWindow(b);
//        //EndWaitCursor();
//    }
//}

void MvDoc::updateToolDialogs(mvGUISettings* gui)
{
    updateColorBarDialog();
    updateSolidDialog();
    updateIsosurfaceDialog();
    updateLightingDialog(gui);
    updateGeometryDialog();
    updateGridDialog();
    updateAnimationDialog(gui);
    updateVectorDialog();
    updateCropDialog(gui);
    updateDataDialog();
    updatePathlinesDialog();
    updateModelFeaturesDialog();
    updateOverlayDialog();
}


/////////////////////////////////////////////////////////////////////////////
// Scale

void MvDoc::setScale(double xScale, double yScale, double zScale)
{
    _manager->SetScale(xScale, yScale, zScale);
    //POSITION pos   = GetFirstViewPosition();
    //CMvView* pView = (CMvView*)GetNextView(pos);
    //pView->ResetCameraClippingRange();
    //UpdateAllViews(NULL);
    //SetModifiedFlag(TRUE);

    for (auto view : this->_views)
    {
        view->resetCameraClippingRange();
    }
    updateAllViews(nullptr);
    setModified(true);
}

/////////////////////////////////////////////////////////////////////////////
// Axes

void MvDoc::setAxesRepresentationToLine()
{
    _manager->SetAxesRepresentationToLine();
    updateAllViews(nullptr);
    setModified(true);
}

void MvDoc::setAxesRepresentationToTube()
{
    _manager->SetAxesRepresentationToTube();
    updateAllViews(nullptr);
    setModified(true);
}

void MvDoc::setAxesProperties(double xPos, double yPos, double zPos,
                               double axesSize, double tubeDiameter)
{
    _manager->SetAxesNormalizedSize(axesSize);
    _manager->SetAxesNormalizedPosition(xPos, yPos, zPos);
    _manager->SetAxesNormalizedTubeDiameter(tubeDiameter);
    updateAllViews(nullptr);
    setModified(true);
}


/////////////////////////////////////////////////////////////////////////////
// Bounding Box
void MvDoc::setBoundingBoxColor(double red, double green, double blue)
{
    _manager->SetBoundingBoxColor(red, green, blue);
    updateAllViews(nullptr);
    setModified(true);
}

void MvDoc::setBoundingBoxColor(vtkColor3d color3d)
{
    _manager->SetBoundingBoxColor(color3d.GetRed(), color3d.GetGreen(), color3d.GetBlue());
    updateAllViews(nullptr);
    setModified(true);
}

/////////////////////////////////////////////////////////////////////////////
// Toolbox->Geometry

void MvDoc::onToolboxGeometry()
{
    assert(geometryDialog);
    if (geometryDialog->isVisible())
    {
        geometryDialog->hide();
        //geometryDialog->setVisible(false);
    }
    else
    {
        geometryDialog->show();
        //geometryDialog->setVisible(true);
    }
}

void MvDoc::onUpdateToolboxGeometry(QAction* action)
{
    assert(geometryDialog);
    action->setChecked(geometryDialog->isVisible());
}

void MvDoc::updateGeometryDialog()
{
    // scale
    const double* sc       = _manager->GetScale();
    geometryDialog->xScale = sc[0];
    geometryDialog->yScale = sc[1];
    geometryDialog->zScale = sc[2];
    geometryDialog->updateDataScale(false);
    geometryDialog->activateScale(true);

    // axes
    const double* p                = _manager->GetAxesNormalizedPosition();
    geometryDialog->representation = _manager->GetAxesRepresentation();
    geometryDialog->axesSize       = _manager->GetAxesNormalizedSize();
    geometryDialog->tubeDiameter   = _manager->GetAxesNormalizedTubeDiameter();
    geometryDialog->xPos           = p[0];
    geometryDialog->yPos           = p[1];
    geometryDialog->zPos           = p[2];
    geometryDialog->updateDataAxes(false);
    geometryDialog->activateAxes(_manager->AreAxesVisible());

    // bounding box
    const double* rgb = _manager->GetBoundingBoxColor();
    geometryDialog->boundingBoxColor = (int)(rgb[0] * 2 + 0.1);                     // black = 0, gray = 1, white = 2
    geometryDialog->updateDataBoundingBox(false);
    geometryDialog->activateBoundingBox(_manager->IsBoundingBoxVisible());

    geometryDialog->enableApplyButton(true);
    geometryDialog->setCurrentIndex(0);
}

/////////////////////////////////////////////////////////////////////////////
// Toolbox->Data

void MvDoc::onToolboxData()
{
    assert(dataDialog);
    if (dataDialog->isVisible())
    {
        dataDialog->hide();
    }
    else
    {
        dataDialog->show();
    }
}

void MvDoc::onUpdateToolboxData(QAction* action)
{
    assert(dataDialog);
    action->setChecked(dataDialog->isVisible());
}

void MvDoc::updateDataDialog()
{
    double range[2];
    _manager->GetScalarDataRange(range);
    dataDialog->setScalarDataRange(range);
    if (_manager->HasVectorData())
    {
        _manager->GetVectorMagnitudeRange(range);
        dataDialog->setVectorMagnitudeRange(range);
    }
    else
    {
        dataDialog->setVectorMagnitudeRange(nullptr);
    }
    if (_manager->HasPathlineData())
    {
        _manager->GetPathlineTimeRange(range);
        dataDialog->setPathlineTimeRange(range);
    }
    else
    {
        dataDialog->setPathlineTimeRange(nullptr);
    }
    assert(_manager->GetActiveScalarDataType() >= 0);
    assert(_manager->GetNumberOfScalarDataTypes() > 0);
    assert(_manager->GetNumberOfScalarDataTypes() > _manager->GetActiveScalarDataType());
    dataDialog->setDataTypeLabels(dataTypeLabels(), _manager->GetActiveScalarDataType());
    dataDialog->activate(true);
}

void MvDoc::setScalarDataTypeTo(int index)
{
    assert(index >= 0);
    int n = _manager->GetNumberOfScalarDataTypes();
    assert(index < _manager->GetNumberOfScalarDataTypes());

    _manager->SetScalarDataTypeTo(index);

    //updateColorBarDialog();          // @todo
    //updateSolidDialog();             // @todo
    //updateIsosurfaceDialog();        // @todo
    double range[2];
    _manager->GetScalarDataRange(range);
    dataDialog->setScalarDataRange(range);

    updateAllViews(nullptr);
    setModified(true);
}

/////////////////////////////////////////////////////////////////////////////
// utilities
bool MvDoc::hasPathlineData() const
{
    return (_manager->HasPathlineData() != 0);
}


/////////////////////////////////////////////////////////////////////////////
// Toolbox->Color Bar

// afx_msg void OnUpdateColorBarTool(CCmdUI* pCmdUI);
void MvDoc::onUpdateToolboxColorBar(QAction* action)
{
    assert(colorBarDialog);
    action->setChecked(colorBarDialog->isVisible());
}

// afx_msg void OnColorBarTool();
void MvDoc::onToolboxColorBar()
{
    assert(colorBarDialog);
    if (colorBarDialog->isVisible())
    {
        colorBarDialog->hide();
    }
    else
    {
        colorBarDialog->show();
    }
}

// void UpdateColorBarDlg()
void MvDoc::updateColorBarDialog()
{ 
    assert(colorBarDialog);

    //CColorBarDataSource* data_source = m_ColorBarDlg->m_DataSource;
    //data_source->m_DataSourceIndex   = m_Manager->GetColorBarSource();
    //data_source->CustomUpdateData(FALSE);

    // Source
    colorBarDialog->dataSourceIndex = _manager->GetColorBarSource();
    colorBarDialog->updateDataSource(false);

    //CColorBarLimitsPage *lim = m_ColorBarDlg->m_LimitsPage;
    //lim->m_ValueBlue = m_Manager->GetColorBarValueBlue();
    //lim->m_ValueRed  = m_Manager->GetColorBarValueRed();
    //lim->m_LogScaleCheckBox.SetCheck(!m_Manager->IsColorBarLinear());
    //lim->CustomUpdateData(FALSE);

    // Limits
    colorBarDialog->valueBlue        = _manager->GetColorBarValueBlue();
    colorBarDialog->valueRed         = _manager->GetColorBarValueRed();
    colorBarDialog->isColorBarLinear = _manager->IsColorBarLinear();
    colorBarDialog->updateDataLimits(false);


    //CColorBarSizePage* size = m_ColorBarDlg->m_SizePage;
    //size->m_Width           = m_Manager->GetColorBarWidth();
    //size->m_Height          = m_Manager->GetColorBarHeight();
    //size->m_Offset          = m_Manager->GetColorBarOffset();
    //size->CustomUpdateData(FALSE);

    // Size
    colorBarDialog->width  = _manager->GetColorBarWidth();
    colorBarDialog->height = _manager->GetColorBarHeight();
    colorBarDialog->offset = _manager->GetColorBarOffset();
    colorBarDialog->updateDataSize(false);


    //CColorBarTextPage* text = m_ColorBarDlg->m_TextPage;
    //text->m_FontSize        = m_Manager->GetColorBarFontSize();
    //text->m_NumLabels       = m_Manager->GetColorBarNumberOfLabels();
    //text->m_Precision       = m_Manager->GetColorBarLabelPrecision();
    //const double* rgb       = m_Manager->GetColorBarTextColor();
    //text->m_ColorOption     = (int)(rgb[0] * 2 + 0.1);
    //text->CustomUpdateData(FALSE);

    // Labels (text)
    colorBarDialog->fontSize    = _manager->GetColorBarFontSize();
    colorBarDialog->numLabels   = _manager->GetColorBarNumberOfLabels();
    colorBarDialog->precision   = _manager->GetColorBarLabelPrecision();
    const double* rgb           = _manager->GetColorBarTextColor();
    colorBarDialog->colorOption = (int)(rgb[0] * 2 + 0.1);                          // black = 0, gray = 1, white = 2
    colorBarDialog->updateDataLabels(false);


    //CColorBarColorsPage* scheme = m_ColorBarDlg->m_ColorsPage;
    //scheme->InitializeDialog();

    // Colors
    colorBarDialog->updateDataColors(false);


    //m_ColorBarDlg->m_PropertySheet->SetActivePage(0);
    //m_ColorBarDlg->Activate(TRUE);
    colorBarDialog->setCurrentIndex(0);
    colorBarDialog->activate(true);
}

/////////////////////////////////////////////////////////////////////////////
// Toolbox->Lighting

void MvDoc::onUpdateToolboxLighting(QAction* action)
{
    assert(lightingDialog);
    action->setChecked(lightingDialog->isVisible());
}

void MvDoc::onToolboxLighting()
{
    assert(lightingDialog);
    if (lightingDialog->isVisible())
    {
        lightingDialog->hide();
    }
    else
    {
        lightingDialog->show();
    }
}

void MvDoc::updateLightingDialog(mvGUISettings* gui)
{
    assert(lightingDialog);

    // Lights
    lightingDialog->headlightOn             = (gui->headlightOn != 0);
    lightingDialog->auxiliaryLightOn        = (gui->auxiliaryLightOn != 0);
    lightingDialog->headlightIntensity      = (int)(gui->headlightIntensity * 100 + 0.5);
    lightingDialog->auxiliaryLightIntensity = (int)(gui->auxiliaryLightIntensity * 100 + 0.5);
    lightingDialog->dirX                    = (int)(gui->auxiliaryLightDirection[0] * 10 + 10.5);
    lightingDialog->dirY                    = (int)(gui->auxiliaryLightDirection[1] * 10 + 10.5);
    lightingDialog->dirZ                    = (int)(gui->auxiliaryLightDirection[2] * 10 + 10.5);
    lightingDialog->updateDataLights(false);
    lightingDialog->updateLabelsLights();
    lightingDialog->activateLights(true);

    // Surface
    lightingDialog->diffuse       = (int)(_manager->GetDiffuseLighting() * 100 + 0.5);
    lightingDialog->ambient       = (int)(_manager->GetAmbientLighting() * 100 + 0.5);
    lightingDialog->specular      = (int)(_manager->GetSpecularLighting() * 100 + 0.5);
    lightingDialog->specularPower = (int)(_manager->GetSpecularPower() + 0.5);
    lightingDialog->updateDataSurface(false);
    lightingDialog->updateLabelsSurface();
    lightingDialog->activateSurface(true);

    // Background
    lightingDialog->customBackground = (gui->customBackground != 0);
    lightingDialog->red              = (int)(gui->background[0] * 100 + 0.5);
    lightingDialog->green            = (int)(gui->background[1] * 100 + 0.5);
    lightingDialog->blue             = (int)(gui->background[2] * 100 + 0.5);
    lightingDialog->updateDataBackground(false);
    lightingDialog->updateLabelsBackground();
    lightingDialog->activateBackground(true);

    lightingDialog->setCurrentIndex(0);
    lightingDialog->setDefaultEnabled(true);
}

void MvDoc::setDiffuseLighting(double diffuse)
{
    _manager->SetDiffuseLighting(diffuse);
    updateAllViews(nullptr);
    setModified(true);
}

void MvDoc::setAmbientLighting(double ambient)
{
    _manager->SetAmbientLighting(ambient);
    updateAllViews(nullptr);
    setModified(true);
}

void MvDoc::setSpecularLighting(double specular)
{
    _manager->SetSpecularLighting(specular);
    updateAllViews(nullptr);
    setModified(true);
}

void MvDoc::setSpecularPower(double specularPower)
{
    _manager->SetSpecularPower(specularPower);
    updateAllViews(nullptr);
    setModified(true);
}

void MvDoc::switchOnHeadlight(bool switchOn)
{
    for (auto view : _views)
    {
        view->switchOnHeadlight(switchOn);
    }
    updateAllViews(nullptr);
    setModified(true);
}

void MvDoc::setHeadlightIntensity(double intensity)
{
    for (auto view : _views)
    {
        view->setHeadlightIntensity(intensity);
    }
    updateAllViews(nullptr);
    setModified(true);
}


void MvDoc::switchOnAuxiliaryLight(bool switchOn)
{
    for (auto view : _views)
    {
        view->switchOnAuxiliaryLight(switchOn);
    }
    updateAllViews(nullptr);
    setModified(true);
}

void MvDoc::setAuxiliaryLightIntensity(double intensity)
{
    for (auto view : _views)
    {
        view->setAuxiliaryLightIntensity(intensity);
    }
    updateAllViews(nullptr);
    setModified(true);
}

void MvDoc::setAuxiliaryLightPosition(double x, double y, double z)
{
    for (auto view : _views)
    {
        view->setAuxiliaryLightPosition(x, y, z);
    }
    updateAllViews(nullptr);
    setModified(true);
}

void MvDoc::setBackgroundColor(double red, double green, double blue)
{
    for (auto view : _views)
    {
        view->setBackgroundColor(red, green, blue);
    }
    updateAllViews(nullptr);
    setModified(true);
}


/////////////////////////////////////////////////////////////////////////////
// Toolbox->Grid

void MvDoc::onUpdateToolboxGrid(QAction* action)
{
    assert(gridDialog);
    action->setChecked(gridDialog->isVisible());
}

void MvDoc::onToolboxGrid()
{
    assert(gridDialog);
    if (gridDialog->isVisible())
    {
        gridDialog->hide();
    }
    else
    {
        gridDialog->show();
    }
}

void MvDoc::updateGridDialog()
{
    assert(gridDialog);

    // Using row-col-lay convention of Modflow
    const int* sdim = _manager->GetScalarGridDimensions(); // recall these are dimensions of vtk points

    // Lines
    if (_manager->IsScalarSubgridOn())
    {
        // if subgrid is on, then limit the extent of the grid gridDialog
        const int* voi = _manager->GetScalarSubgridExtent();
        gridDialog->XMin  = voi[0];
        gridDialog->XMax  = voi[1];
        gridDialog->YMin  = voi[2];
        gridDialog->YMax  = voi[3];
        gridDialog->ZMin  = voi[4];
        gridDialog->ZMax  = voi[5];
    }
    else
    {
        if (_manager->GetGridType() == GridType::MV_STRUCTURED_GRID)
        {
            gridDialog->XMin = 0;
            gridDialog->XMax = sdim[0] - 1;
            gridDialog->YMin = 0;
            gridDialog->YMax = sdim[1] - 1;
            gridDialog->ZMin = 0;
            gridDialog->ZMax = sdim[2] - 1;
        }
        else
        {
            gridDialog->ZMin = 0;
            gridDialog->ZMax = _manager->GetNumberOfLayersInUnstructuredGrid();
        }
    }
    if (_manager->GetGridType() == GridType::MV_STRUCTURED_GRID)
    {
        int p[3];
        _manager->GetGridLinePositions(p);
        gridDialog->positionX = p[0];
        gridDialog->positionY = sdim[1] - p[1] - 1;
        gridDialog->positionZ = sdim[2] - p[2] - 1;

        gridDialog->isActiveX   = _manager->AreGridLinesActive(0);
        gridDialog->isActiveY   = _manager->AreGridLinesActive(1);
        gridDialog->isActiveZ   = _manager->AreGridLinesActive(2);

        const double* rgb    = _manager->GetGridLineColor();
        gridDialog->colorOption = (int)(rgb[0] * 2 + 0.1);                              // black = 0, gray = 1, white = 2
        gridDialog->updateDataLines(false);
        bool activate = _manager->AreActivatedGridLinesVisible();

        gridDialog->activateLines(activate);
    }
    else
    {
        gridDialog->positionZ = _manager->GetGridLayerPosition();
        gridDialog->isActiveX = false;
        gridDialog->isActiveY = false;
        gridDialog->isActiveZ = true;
        // this asserts on ex-gwf-disvmesh
        //assert(gridDialog->isActiveX == (_manager->AreGridLinesActive(0) != 0));
        //assert(gridDialog->isActiveY == (_manager->AreGridLinesActive(1) != 0));
        //assert(gridDialog->isActiveZ == (_manager->AreGridLinesActive(2) != 0));
        const double* rgb       = _manager->GetGridLineColor();
        gridDialog->colorOption = (int)(rgb[0] * 2 + 0.1);                              // black = 0, gray = 1, white = 2
        gridDialog->updateDataLines(false);
        gridDialog->activateLines(_manager->IsGridLayerVisible());
    }

    // Shell
    const double* rgb   = _manager->GetGridShellColor();
    gridDialog->red     = (int)(rgb[0] * 100 + 0.5);
    gridDialog->green   = (int)(rgb[1] * 100 + 0.5);
    gridDialog->blue    = (int)(rgb[2] * 100 + 0.5);
    gridDialog->opacity = (int)(_manager->GetGridShellOpacity() * 100 + 0.5);
    gridDialog->updateDataShell(false);
    gridDialog->updateLabelsShell();
    gridDialog->activateShell(_manager->IsGridShellVisible());


    // Subgrid
    // Subgrid page
    const int* voi = _manager->GetScalarSubgridExtent();
    if (_manager->GetGridType() == GridType::MV_STRUCTURED_GRID)
    {
        // structured grid
        gridDialog->col_min            = voi[0] + 1;
        gridDialog->col_max            = voi[1];
        gridDialog->row_min            = sdim[1] - voi[3];
        gridDialog->row_max            = sdim[1] - voi[2] - 1;
        gridDialog->lay_min            = sdim[2] - voi[5];
        gridDialog->lay_max            = sdim[2] - voi[4] - 1;
        gridDialog->col_upper_limit    = sdim[0] - 1;
        gridDialog->row_upper_limit    = sdim[1] - 1;
        gridDialog->lay_upper_limit    = sdim[2] - 1;
        gridDialog->isSubgridActivated = (_manager->IsScalarSubgridOn() != 0);
    }
    else if (_manager->GetGridType() == GridType::MV_LAYERED_GRID)
    {
        gridDialog->col_min            = 0;
        gridDialog->col_max            = 0;
        gridDialog->row_min            = 0;
        gridDialog->row_max            = 0;
        // TO DO: get layer limits from m_Manager
        gridDialog->lay_min            = 1;
        gridDialog->lay_max            = _manager->GetNumberOfLayersInUnstructuredGrid();
        gridDialog->col_upper_limit    = 0;
        gridDialog->row_upper_limit    = 0;
        gridDialog->lay_upper_limit    = _manager->GetNumberOfLayersInUnstructuredGrid();
        gridDialog->isSubgridActivated = false;
    }
    else if (_manager->GetGridType() == GridType::MV_UNSTRUCTURED_GRID)
    {
        gridDialog->col_min            = 0;
        gridDialog->col_max            = 0;
        gridDialog->row_min            = 0;
        gridDialog->row_max            = 0;
        gridDialog->lay_min            = 0;
        gridDialog->lay_max            = 0;
        gridDialog->col_upper_limit    = 0;
        gridDialog->row_upper_limit    = 0;
        gridDialog->lay_upper_limit    = 0;
        gridDialog->isSubgridActivated = false;
    }
    gridDialog->updateDataSubgrid(false);
    gridDialog->activateSubgrid(true);

}

void MvDoc::setGridLineColor(double red, double green, double blue)
{
    _manager->SetGridLineColor(red, green, blue);
    updateAllViews(nullptr);
    setModified(true);
}

void MvDoc::setGridLineColor(vtkColor3d color3d)
{
    setGridLineColor(color3d.GetRed(), color3d.GetGreen(), color3d.GetBlue());
}

void MvDoc::activateGridLines(int slice, bool b)
{
    if (b)
    {
        _manager->ActivateGridLines(slice);
    }
    else
    {
        _manager->DeactivateGridLines(slice);
    }
    updateAllViews(nullptr);
    setModified(true);
}

void MvDoc::setGridLinePositions(int col, int row, int lay)
{
    const int* sdim = _manager->GetScalarGridDimensions(); // returns point dimensions
    int        posX = col;
    int        posY = (sdim[1] - 1) - row;
    int        posZ = (sdim[2] - 1) - lay;
    _manager->SetGridLinePositions(posX, posY, posZ);
    updateAllViews(nullptr);
    setModified(true);
}

void MvDoc::setGridLayerPosition(int layerNumber)
{
    _manager->SetGridLayerPosition(layerNumber);
    updateAllViews(nullptr);
    setModified(true);
}

void MvDoc::setGridShellColor(double red, double green, double blue)
{
    _manager->SetGridShellColor(red, green, blue);
    updateAllViews(nullptr);
    setModified(true);
}

void MvDoc::setGridShellOpacity(double opacity)
{
    _manager->SetGridShellOpacity(opacity);
    updateAllViews(nullptr);
    setModified(true);
}

void MvDoc::applySubgrid(int col_min, int col_max, int row_min, int row_max, int lay_min, int lay_max)
{
    int imin, imax, jmin, jmax, kmin, kmax;
    // convert to vtk indexing and compute extents for points
    if (_manager->GetGridType() == GridType::MV_STRUCTURED_GRID)
    {
        const int* sdim = _manager->GetScalarGridDimensions();
        // note that imin, imax, jmin, etc refer to point dimensions
        imin            = col_min - 1;
        imax            = col_max;
        jmin            = sdim[1] - row_max - 1;
        jmax            = sdim[1] - row_min;
        kmin            = sdim[2] - lay_max - 1;
        kmax            = sdim[2] - lay_min;
    }
    else if (_manager->GetGridType() == GridType::MV_LAYERED_GRID)
    {
        imin = 0;
        imax = 0;
        jmin = 0;
        jmax = 0;
        // note that kmin and kmax here refer to cell dimension
        // kmin = m_Manager->GetNumberOfLayersInUnstructuredGrid() - lay_max;
        // kmax = m_Manager->GetNumberOfLayersInUnstructuredGrid() - lay_min-1;
        kmin = lay_min - 1; // top layer in subgrid
        kmax = lay_max - 1; // bottom layer in subgrid
    }

    _manager->SetScalarSubgridExtent(imin, imax, jmin, jmax, kmin, kmax);
    _manager->ScalarSubgridOn();
    /*
        // update the grid lines dlg box
        int p[3];
        m_Manager->GetGridLinePositions(p);
        m_GridDlg->m_GridLinesPage->m_XMin = imin;
        m_GridDlg->m_GridLinesPage->m_XMax = imax;
        m_GridDlg->m_GridLinesPage->m_YMin = sdim[1]-jmax-1;
        m_GridDlg->m_GridLinesPage->m_YMax = sdim[1]-jmin-1;
        m_GridDlg->m_GridLinesPage->m_ZMin = sdim[2]-kmax-1;
        m_GridDlg->m_GridLinesPage->m_ZMax = sdim[2]-kmin-1;
        m_GridDlg->m_GridLinesPage->m_PositionX = p[0];
        m_GridDlg->m_GridLinesPage->m_PositionY = sdim[1] - p[1] - 1;
        m_GridDlg->m_GridLinesPage->m_PositionZ = sdim[2] - p[2] - 1;
        m_GridDlg->m_GridLinesPage->CustomUpdateData(FALSE);

        // Update the vector dialog box
        if (m_Manager->HasVectorData())
        {
            if(m_VectorDlg->m_ControlsPage->m_col_min < col_min)
             m_VectorDlg->m_ControlsPage->m_col_min = col_min;
            if(m_VectorDlg->m_ControlsPage->m_col_max > col_max)
               m_VectorDlg->m_ControlsPage->m_col_max = col_max;
            if(m_VectorDlg->m_ControlsPage->m_row_min < row_min)
               m_VectorDlg->m_ControlsPage->m_row_min = row_min;
            if(m_VectorDlg->m_ControlsPage->m_row_max > row_max)
               m_VectorDlg->m_ControlsPage->m_row_max = row_max;
            if(m_VectorDlg->m_ControlsPage->m_lay_min < lay_min)
               m_VectorDlg->m_ControlsPage->m_lay_min = lay_min;
            if(m_VectorDlg->m_ControlsPage->m_lay_max > lay_max)
               m_VectorDlg->m_ControlsPage->m_lay_max = lay_max;

          m_VectorDlg->m_ControlsPage->m_col_lower_limit = col_min;
            m_VectorDlg->m_ControlsPage->m_col_upper_limit = col_max;
            m_VectorDlg->m_ControlsPage->m_row_lower_limit = row_min;
            m_VectorDlg->m_ControlsPage->m_row_upper_limit = row_max;
            m_VectorDlg->m_ControlsPage->m_lay_lower_limit = lay_min;
            m_VectorDlg->m_ControlsPage->m_lay_upper_limit = lay_max;
            m_VectorDlg->m_ControlsPage->CustomUpdateData(FALSE);

          m_VectorDlg->m_ControlsPage->Apply();
        }
    */

    updateAllViews(nullptr);
    setModified(true);
}

void MvDoc::subgridOff()
{
    _manager->ScalarSubgridOff();

    // update the grid lines dlg box
    const int* sdim  = _manager->GetScalarGridDimensions(); // keep in mind these are dimensions of points
    gridDialog->XMin = 0;
    gridDialog->XMax = sdim[0] - 1;
    gridDialog->YMin = 0;
    gridDialog->YMax = sdim[1] - 1;
    gridDialog->ZMin = 0;
    gridDialog->ZMax = sdim[2] - 1;

    // Update the vector dlg box
    if (_manager->HasVectorData())
    {
        const int* vdim = _manager->GetVectorGridDimensions();
        /*
          const int *voi = m_Manager->GetVectorSubsampleExtents();
          m_VectorDlg->m_ControlsPage->m_col_min = voi[0] + 1;
          m_VectorDlg->m_ControlsPage->m_col_max = voi[1] + 1;
          m_VectorDlg->m_ControlsPage->m_row_min = voi[2] + 1;
          m_VectorDlg->m_ControlsPage->m_row_max = voi[3] + 1;
          m_VectorDlg->m_ControlsPage->m_lay_min = voi[4] + 1;
          m_VectorDlg->m_ControlsPage->m_lay_max = voi[5] + 1;
        */
        // @todo below
        //m_VectorDlg->m_ControlsPage->m_col_lower_limit = 1;
        //m_VectorDlg->m_ControlsPage->m_col_upper_limit = vdim[0];
        //m_VectorDlg->m_ControlsPage->m_row_lower_limit = 1;
        //m_VectorDlg->m_ControlsPage->m_row_upper_limit = vdim[1];
        //m_VectorDlg->m_ControlsPage->m_lay_lower_limit = 1;
        //m_VectorDlg->m_ControlsPage->m_lay_upper_limit = vdim[2];
        //m_VectorDlg->m_ControlsPage->CustomUpdateData(FALSE);
    }

    updateAllViews(nullptr);
    setModified(true);
}

/////////////////////////////////////////////////////////////////////////////
// Toolbox->???


void MvDoc::getPathlineTimeRange(double* range)
{
    _manager->GetPathlineTimeRange(range);
}

void MvDoc::getScalarDataRange(double* range)
{
    _manager->GetScalarDataRange(range);
}


void MvDoc::updateSolidDialog()
{
    // @todo
    assert(solidDialog);
}
void MvDoc::updateIsosurfaceDialog()
{
    // @todo
    assert(isosurfaceDialog);
}

void MvDoc::updateAnimationDialog(mvGUISettings* gui)
{
    // @todo
    assert(animationDialog);
}

void MvDoc::updateVectorDialog()
{
    // @todo
    assert(vectorDialog);
}

void MvDoc::updateCropDialog(mvGUISettings* gui)
{
    // @todo
    assert(cropDialog);
}

void MvDoc::updatePathlinesDialog()
{
    // @todo
    //assert(pathlinesDialog);
}

void MvDoc::updateModelFeaturesDialog()
{
    // @todo
    assert(modelFeaturesDialog);
}

void MvDoc::updateOverlayDialog()
{
    // @todo
    assert(overlayDialog);
}

GridType MvDoc::gridType()
{
    return _manager->GetGridType();
}
