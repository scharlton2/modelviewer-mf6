#ifndef MVDOC_H
#define MVDOC_H

#include <list>
#include <vector>
#include <string>

#include <QDir>
#include <QObject>
#include <QString>

#include <vtkColor.h>
#include <vtkSmartPointer.h>

class mvGUISettings;
class mvManager;
//class mvView;
class QAbstractView;
class QAction;
class QObject;
class QWidget;
class QMainWindow;
class DataDialog;
class GeometryDialog;

class vtkPropCollection;

// enum forward declarations
enum class MouseMode;

enum class ProjectionType {
    ptPerspective,
    ptParallel
};

enum class AnimationType {
    atTime,
    atSpace
};

class MvDoc : public QObject
{
    Q_OBJECT
public:
    explicit MvDoc(QMainWindow* parent);
    ~MvDoc() override;


    MouseMode                          interactorStyle() const;

    const QString& pathName() const;
    //void setPathName(const QString* pathName);


    QString        activeScalarName() const;
    QString        modelName() const;

    //char**                             GetTimePointLabels();
    //int                                GetNumberOfTimePoints();
    QStringList                        timePointLabels();

    QStringList                        dataTypeLabels();

    void                               setTimePointTo(int timePointIndex);
    void                               updateAnimation();
    void                               advanceOneTimePoint();
    void                               updateAnimationPosition();


    // File menu
    void                               onFileClose();
    void                               onFileNew();
    void                               onFileOpen();

    void                               loadFile(const QString& fileName);
    bool                               saveFile(const QString& fileName);


    // Time Label
    void           SetTimeLabelFontSize(int size, bool update = true);
    int            GetTimeLabelFontSize() const;
    void           SetTimeLabelPosition(double x, double y, bool update = true);
    const double*  GetTimeLabelPosition() const;

    // Color Bar
    void           SetColorBarEndPoints(double valueBlue, double valueRed);
    void           UseLinearColorBar();
    void           UseLogColorBar();
    void           SetColorBarSize(int width, int height, int offset, bool update = true);
    void           SetColorBarFontSize(int fontSize, bool update = true);
    void           SetColorBarNumberOfLabels(int numLabels, bool update = true);
    void           SetColorBarLabelPrecision(int precision, bool update = true);
    void           SetColorBarTextColor(double red, double green, double blue, bool update = true);
    void           SetColorBarColorScheme(int Value);
    int            GetColorBarWidth();
    int            GetColorBarHeight();
    int            GetColorBarOffset();
    int            GetColorBarFontSize();
    int            GetColorBarColorScheme();
    unsigned long  GetColorBarFirstCustomColor();
    unsigned long  GetColorBarLastCustomColor();
    void           SetColorBarFirstCustomColor(unsigned long value);
    void           SetColorBarLastCustomColor(unsigned long value);
    int            GetColorBarSource();
    void                               SetColorBarSource(int value);
    double                             GetColorBarValueBlue() const;
    double                             GetColorBarValueRed() const;

    
    bool                               modified() const;
    bool                               isModified() const;
    void                               setModified(bool modifed);

    bool                               isAnimating() const;

	void           addView(QAbstractView* pView);
    void           removeView(QAbstractView* pView);
    void           updateAllViews(QAbstractView* pSender, QObject* hint = nullptr);

    vtkSmartPointer<vtkPropCollection> propCollection();

    // 
    QString                            currentFile();

    bool                               maybeSave();

    QDir                               defaultDir() const;

    // Toolbox->Data
    void                               onToolboxData();
    void                               onUpdateToolboxData(QAction* action);
    void                               setScalarDataTypeTo(int index);


    // Toolbox->Geometry
    void                               onToolboxGeometry();
    void                               onUpdateToolboxGeometry(QAction* action);

    // Geometry
    void                               setScale(double xScale, double yScale, double zScale);
    void                               setAxesRepresentationToLine();
    void                               setAxesRepresentationToTube();
    void                               setAxesProperties(double xPos, double yPos, double zPos,
                                                         double axesSize, double tubeDiameter);
    void                               setBoundingBoxColor(double red, double green, double blue);
    void                               setBoundingBoxColor(vtkColor3d color3d);
    //void                               ApplySubgrid(int col_min, int col_max, int row_min, int row_max, int lay_min, int lay_max);
    //void                               SubgridOff();


public slots:

    void onShowIsosurfaces();
    void onShowNone();
    void onShowSolid();

    void onShowVectors(QWidget* parent);
    void onShowPathlines();
    void onModelFeatures();

    void onShowAxes();
    void onShowGridLines();
    void onShowGridShell();
    void onShowBoundingBox();
    void onShowOverlay();

    void onShowTime();
    void onShowColorBar();

signals:

private:

    friend class MainWindow;

    void                      setCurrentFile(const QString& fileName);


    void                      loadPreviousAppSettings();
    void                      saveCurrentAppSettings();

    //QString                  _pathName;              // CDocument::m_strPathName
    QString                   _currentFile;

    bool                      _modified;            // CDocument::m_bModified


    AnimationType            animationType;         // m_AnimationType
    size_t                   animationSteps;        // m_AnimationSteps
    MouseMode                _interactorStyle;      // m_InteractorStyle
    ProjectionType           projectionMode;        // m_ProjectionMode
    size_t                   numberOfModels;        // m_NumberOfModels
    bool                     readyToClose;          // m_ReadyToClose
    std::vector<std::string> modelNames;            // m_ModelNames
    std::string              defaultModel;          // m_DefaultModel
    bool                     startup;               // m_Startup
    bool                     _isAnimating;          // m_IsAnimating

    mvGUISettings*           _gui;

    double                   defaultXOrigin;        // m_default_xorigin
    double                   defaultYOrigin;        // m_default_yorigin
    double                   defaultAngRot;         // m_default_angrot

    // The visualization pipeline manager
    mvManager*               _manager;

    std::list<QAbstractView*> _views;



/***
    // Modeless dialog boxes
    CGridDlg*          m_GridDlg;
    CColorBarDlg*      m_ColorBarDlg;
    CLightingDlg*      m_LightingDlg;
    CSolidDlg*         m_SolidDlg;
    CIsosurfaceDlg*    m_IsosurfaceDlg;
    CCropDlg*          m_CropDlg;
    CAnimationDlg*     m_AnimationDlg;
    CVectorDlg*        m_VectorDlg;
    CPathlinesDlg*     m_PathlinesDlg;
    CModelFeaturesDlg* m_ModelFeaturesDlg;
    COverlayDlg*       m_OverlayDlg;
***/

    DataDialog*               dataDialog;
    GeometryDialog*           geometryDialog;

    // Protected methods;
    //void                      LoadPreviousAppSettings();
    //void                      SaveCurrentAppSettings();
    void                      updateToolDialogs(mvGUISettings* gui);
    //void                      UpdateSolidDlg();
    //void                      UpdateIsosurfaceDlg();
    //void                      UpdateLightingDlg(mvGUISettings* gui);
    void                      updateGeometryDialog();
    //void                      UpdateGridDlg();
    //void                      UpdateVectorDlg();
    //void                      UpdateCropDlg(mvGUISettings* gui);
    void                      updateDataDialog();
    //void                      UpdateModelFeaturesDlg();
    //void                      UpdateAnimationDlg(mvGUISettings* gui);
    //void                      UpdateOverlayDlg();
    //void                      UpdateAnimation();
    //void                      UpdateAnimationPosition();


    void                      onParallelProjection();
    void                      onUpdateParallelProjection(QAction* action);
    void                      onPerspectiveProjection();
    void                      onUpdatePerspectiveProjection(QAction* action);

    void                      onPreferences();
};


#endif // MVDOC_H
