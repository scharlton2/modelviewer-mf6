#include "mvdoc.h"


MvDoc::MvDoc(QObject *parent)
    : QObject{parent}
{
    // Initialize variables
    startup           = true;
    projectionMode    = ProjectionType::ptPerspective;
    isAnimating       = false;
#if TODO
    m_GUI = NULL;
#endif
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

    // Create the visualization pipeline manager
    m_Manager          = new mvManager;

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
}

MvDoc::~MvDoc()
{
    // Save persistent app settings to Windows registry
    saveCurrentAppSettings();

/***
    // Free memory
    delete m_Manager;
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

const QString& MvDoc::pathName() const
{
    return _pathName;
}

QString MvDoc::modelName() const
{
#if TODO
    return m_Manager->GetModelName();
#else
    return tr("");
#endif
}

QString MvDoc::dataName() const
{
#if TODO
    return m_Manager->GetActiveScalarDataName();
#else
    return tr("");
#endif
}
