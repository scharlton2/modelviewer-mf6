#include "mvdoc.h"

#include <QAction>
#include <QApplication>
#include <QDir>
#include <QMainWindow>
#include <QMessageBox>
#include <QSettings>


//#include "qabstractview.h"

#include "mainwindow.h"
#include "datafilesdialog.h"

#include "Modflow6DataSource.h"

#include "mvview.h"

#include "mvGUISettings.h"
#include "mvManager.h"

#include "geometrydialog.h"


#include <algorithm>


MvDoc::MvDoc(QMainWindow* parent)
    : QObject{parent}
    , _gui{nullptr}
    , _isAnimating{false}
    , _modified{false}
{
    // Initialize variables
    startup           = true;
    projectionMode    = ProjectionType::ptPerspective;
    readyToClose      = false;
    animationType     = AnimationType::atTime;
    animationSteps    = 10;

    defaultXOrigin    = 0.;
    defaultYOrigin    = 0.;
    defaultAngRot     = 0.;

#if TODO
    // Get the models supported by this application
    m_NumberOfModels  = mvModelList::GetNumberOfModels();
    m_ModelNames      = new char *[m_NumberOfModels];
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
    m_ColorBarDlg      = NULL;
    m_LightingDlg      = NULL;
    m_GridDlg          = NULL;
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
    geometryDialog = new GeometryDialog(parent, this);
    geometryDialog->reinitialize();
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

std::vector<QString> MvDoc::timePointLabels()
{
    std::vector<QString> timePointLabels;
    int                  n      = _manager->GetNumberOfTimePoints();
    char**               labels = _manager->GetTimePointLabels();
    for (int i = 0; i < n; ++i)
    {
        timePointLabels.push_back(labels[i]);
    }
    return timePointLabels;
}

/////////////////////////////////////////////////////////////////////////////
// File->

#include <QFileDialog>
#include <QMessageBox>
#include <vtkPropCollection.h>

void MvDoc::onFileNew()
{
    //{{ OLD
    //char selectedModel[20];
    //// This version of Model View is customized to display only Modflow 6 results.
    //// In other words, the variable "selectedModel" is set to "Modflow 6"
    //strcpy(selectedModel, Modflow6DataSource::GetNameStatic());

    //// Display dialog box for user to specify data files for the
    //// selected model. Note that memory for dataFileList is allocated
    //// by the dataFileList method
    //char* dataFileList = DataFilesDialog::GetDataFileList(selectedModel);
    //if (dataFileList == NULL)
    //{
    //    return; // User clicked the Cancel button
    //}

    //// Load data
    //char* errorMsg = m_Manager->LoadData(selectedModel, dataFileList);
    //delete[] dataFileList;

    //// Check for error in reading data files
    //if (errorMsg != 0)
    //{
    //    AfxMessageBox(errorMsg);
    //    return;
    //}

    //// Display dialog box for user to select time and type of data to view
    //CDataSelectionDlg dlg2;
    //dlg2.m_NumberOfTimePoints      = m_Manager->GetNumberOfTimePoints();
    //dlg2.m_TimePointLabels         = m_Manager->GetTimePointLabels();
    //dlg2.m_NumberOfScalarDataTypes = m_Manager->GetNumberOfScalarDataTypes();
    //dlg2.m_DataTypeLabels          = m_Manager->GetDataTypeLabels();
    //dlg2.m_TimeLabelOption         = m_Manager->GetTimeLabelOption();
    //dlg2.m_InitialDisplayTimePoint = m_Manager->GetInitialDisplayTimePoint();
    //dlg2.DoModal(); // Cancel not allowed.
    //m_Manager->SetTimePointTo(dlg2.m_SelectedTimePoint);
    //m_Manager->SetScalarDataTypeTo(dlg2.m_SelectedDataType);

    //// Apply default settings and then turn on bounding box
    //m_Manager->ApplyDefaultSettings();

    //// If there are more than one time points, turn on the
    //// time label
    //if (m_Manager->GetNumberOfTimePoints() > 1)
    //{
    //    m_Manager->ShowTimeLabel();
    //}

    //// Create a gui settings object with initial settings. These
    //// are used to update the tool dialog boxes and view.
    //mvGUISettings* gui = new mvGUISettings;

    //// Update the tool (modeless) dialog boxes
    //UpdateToolDialogs(gui);

    //// Set the view parameters
    //m_ProjectionMode = MV_PERSPECTIVE_PROJECTION;
    //POSITION pos     = GetFirstViewPosition();
    //CMvView* pView   = (CMvView*)GetNextView(pos);
    //pView->SetProjectionToPerspective();
    //pView->ApplyViewSettings(gui);
    //pView->ResetViewpoint();
    //delete gui;

    //// Mark this document as modified
    //SetModifiedFlag(TRUE);

    //// Redraw the view;
    //UpdateAllViews(NULL);
    //}} OLD

    MainWindow* mainWindow = dynamic_cast<MainWindow*>(parent());
    assert(mainWindow);

    //{{NEW
    delete _manager;
    _manager = new mvManager;

    geometryDialog->reinitialize();
    //}}NEW


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
    this->_manager->SetTimePointTo(0);
    this->_manager->SetScalarDataTypeTo(0);

    // Apply default settings and then turn on bounding box
    this->_manager->ApplyDefaultSettings();

    // If there are more than one time points, turn on the
    // time label
    if (this->_manager->GetNumberOfTimePoints() > 1)
    {
        this->_manager->ShowTimeLabel();
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

    if (!mainWindow->maybeSave()) return;

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
    _manager->Deserialize(QDir::toNativeSeparators(fileName).toStdString().c_str(), _gui, errorMsg);
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
    _manager->Deserialize(QDir::toNativeSeparators(fileName).toStdString().c_str(), _gui, errorMsg);
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

    updateToolDialogs(_gui);
    // mainWindow->updateActions();
    mainWindow->updateStatusBar();
    setCurrentFile(fileName);

    QApplication::restoreOverrideCursor();
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

void MvDoc::SetColorBarEndPoints(double valueBlue, double valueRed)
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

unsigned long MvDoc::GetColorBarFirstCustomColor()
{
    return _manager->GetColorBarFirstCustomColor();
}

unsigned long MvDoc::GetColorBarLastCustomColor()
{
    return _manager->GetColorBarLastCustomColor();
}

void MvDoc::SetColorBarFirstCustomColor(unsigned long value)
{
    _manager->SetColorBarFirstCustomColor(value);
    updateAllViews(nullptr);
    setModified(true);
}

void MvDoc::SetColorBarLastCustomColor(unsigned long value)
{
    _manager->SetColorBarLastCustomColor(value);
    updateAllViews(nullptr);
    setModified(true);
}

int MvDoc::GetColorBarSource()
{
    return _manager->GetColorBarSource();
}

void MvDoc::SetColorBarSource(int value)
{
    _manager->SetColorBarSource(value);
}

double MvDoc::GetColorBarValueBlue() const
{
    return _manager->GetColorBarValueBlue();
}

double MvDoc::GetColorBarValueRed() const
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

void MvDoc::onUpdateToolboxGeometry(QAction *action)
{
    assert(geometryDialog);
    action->setChecked(geometryDialog->isVisible());
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
    /*
    if (m_Manager->AreAxesVisible())
    {
        m_Manager->HideAxes();
        m_GeometryDlg->m_AxesPage->Activate(FALSE);
    }
    else
    {
        m_Manager->ShowAxes();
        m_GeometryDlg->m_AxesPage->Activate(TRUE);
    }
    UpdateAllViews(NULL);
    setModified(TRUE);
    */

    if (_manager->AreAxesVisible())
    {
        _manager->HideAxes();
        //m_GeometryDlg->m_AxesPage->Activate(FALSE);
    }
    else
    {
        _manager->ShowAxes();
        //m_GeometryDlg->m_AxesPage->Activate(TRUE);
    }
    updateAllViews(nullptr);
    setModified(true);
}

void MvDoc::onShowGridLines()
{
    /*
    if (m_Manager->GetGridType() == GridType::MV_STRUCTURED_GRID)
    {
        if (m_Manager->AreActivatedGridLinesVisible())
        {
            m_Manager->HideGridLines();
            m_GridDlg->m_GridLinesPage->Activate(FALSE);
            m_GridDlg->m_ApplyButton.EnableWindow(FALSE);
        }
        else
        {
            m_Manager->ShowActivatedGridLines();
            m_GridDlg->m_GridLinesPage->Activate(TRUE);
            m_GridDlg->m_ApplyButton.EnableWindow(
                m_GridDlg->m_PropertySheet->GetActiveIndex() == 0);
        }
    }
    else if (m_Manager->GetGridType() == GridType::MV_LAYERED_GRID)
    {
        if (m_Manager->IsGridLayerVisible())
        {
            m_Manager->HideGridLayer();
            m_GridDlg->m_GridLinesPage->Activate(FALSE);
            m_GridDlg->m_ApplyButton.EnableWindow(FALSE);
        }
        else
        {
            m_Manager->ShowGridLayer();
            m_GridDlg->m_GridLinesPage->Activate(TRUE);
            m_GridDlg->m_ApplyButton.EnableWindow(TRUE);
        }
    }
    UpdateAllViews(NULL);
    setModified(TRUE);
    */

    if (_manager->GetGridType() == GridType::MV_STRUCTURED_GRID)
    {
        if (_manager->AreActivatedGridLinesVisible())
        {
            _manager->HideGridLines();
            //m_GridDlg->m_GridLinesPage->Activate(FALSE);
            //m_GridDlg->m_ApplyButton.EnableWindow(FALSE);
        }
        else
        {
            _manager->ShowActivatedGridLines();
            //m_GridDlg->m_GridLinesPage->Activate(TRUE);
            //m_GridDlg->m_ApplyButton.EnableWindow(
            //m_GridDlg->m_PropertySheet->GetActiveIndex() == 0);
        }
    }
    else if (_manager->GetGridType() == GridType::MV_LAYERED_GRID)
    {
        if (_manager->IsGridLayerVisible())
        {
            _manager->HideGridLayer();
            //m_GridDlg->m_GridLinesPage->Activate(FALSE);
            //m_GridDlg->m_ApplyButton.EnableWindow(FALSE);
        }
        else
        {
            _manager->ShowGridLayer();
            //m_GridDlg->m_GridLinesPage->Activate(TRUE);
            //m_GridDlg->m_ApplyButton.EnableWindow(TRUE);
        }
    }
    updateAllViews(nullptr);
    setModified(true);
}

void MvDoc::onShowGridShell()
{
    /*
    if (m_Manager->IsGridShellVisible())
    {
        m_Manager->HideGridShell();
        m_GridDlg->m_GridShellPage->Activate(FALSE);
    }
    else
    {
        m_Manager->ShowGridShell();
        m_GridDlg->m_GridShellPage->Activate(TRUE);
    }
    UpdateAllViews(NULL);
    setModified(TRUE);
    */

    if (_manager->IsGridShellVisible())
    {
        _manager->HideGridShell();
        //m_GridDlg->m_GridShellPage->Activate(FALSE);
    }
    else
    {
        _manager->ShowGridShell();
        //m_GridDlg->m_GridShellPage->Activate(TRUE);
    }
    updateAllViews(nullptr);
    setModified(true);
}

void MvDoc::onShowBoundingBox()
{
    /*
    if (m_Manager->IsBoundingBoxVisible())
    {
        m_Manager->HideBoundingBox();
        m_GeometryDlg->m_BoundingBoxPage->Activate(FALSE);
    }
    else
    {
        m_Manager->ShowBoundingBox();
        m_GeometryDlg->m_BoundingBoxPage->Activate(TRUE);
    }
    UpdateAllViews(NULL);
    setModified(TRUE);
    */

    if (_manager->IsBoundingBoxVisible())
    {
        _manager->HideBoundingBox();
        //m_GeometryDlg->m_BoundingBoxPage->Activate(FALSE);
    }
    else
    {
        _manager->ShowBoundingBox();
        //m_GeometryDlg->m_BoundingBoxPage->Activate(TRUE);
    }
    updateAllViews(nullptr);
    setModified(true);
}

void MvDoc::onShowOverlay()
{
    /*
    if (m_Manager->IsOverlayVisible())
    {
        m_Manager->HideOverlay();
    }
    else
    {
        m_Manager->ShowOverlay();
    }
    UpdateAllViews(NULL);
    setModified(TRUE);
    */

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
    /*
    if (m_Manager->IsTimeLabelVisible())
    {
        m_Manager->HideTimeLabel();
    }
    else
    {
        m_Manager->ShowTimeLabel();
    }
    UpdateAllViews(NULL);
    setModified(TRUE);
    */

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
    /*
    if (m_Manager->IsColorBarVisible())
    {
        m_Manager->HideColorBar();
    }
    else
    {
        m_Manager->ShowColorBar();
    }
    UpdateAllViews(NULL);
    setModified(TRUE);
    */

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
    
    //POSITION pos     = GetFirstViewPosition();
    //CMvView* pView   = (CMvView*)GetNextView(pos);
    //pView->SetProjectionToPerspective();
    //UpdateAllViews(NULL);

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
    //UpdateColorBarDlg();
    //UpdateSolidDlg();
    //UpdateIsosurfaceDlg();
    //UpdateLightingDlg(gui);
    updateGeometryDialog();
    //UpdateGridDlg();
    //UpdateAnimationDlg(gui);
    //UpdateVectorDlg();
    //UpdateCropDlg(gui);
    //UpdateDataDlg();
    //UpdatePathlinesDlg();
    //UpdateModelFeaturesDlg();
    //UpdateOverlayDlg();
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
    geometryDialog->boundingBoxColor = (int)(rgb[0] * 2 + 0.1);
    geometryDialog->updateDataBoundingBox(false);
    geometryDialog->activateBoundingBox(_manager->IsBoundingBoxVisible());

    geometryDialog->enableApplyButton(true);
    geometryDialog->setCurrentIndex(0);
}
