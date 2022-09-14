#ifndef MVDOC_H
#define MVDOC_H

#include <list>
#include <vector>
#include <string>

#include <QDir>
#include <QObject>
#include <QString>

#include <vtkSmartPointer.h>

class mvGUISettings;
class mvManager;
//class mvView;
class QAbstractView;
class QObject;
class QWidget;
class QMainWindow;

class vtkPropCollection;

enum class ProjectionType {
    ptPerspective,
    ptProjection
};

enum class AnimationType {
    atTime,
    atSpace
};

enum class MouseMode {
    mmTrackball,
    mmJoystick
};

class MvDoc : public QObject
{
    Q_OBJECT
public:
    explicit MvDoc(QMainWindow* parent);
    ~MvDoc() override;


    const QString& pathName() const;
    //void setPathName(const QString* pathName);


    QString        activeScalarName() const;
    QString        modelName() const;

    //char**                             GetTimePointLabels();
    //int                                GetNumberOfTimePoints();
    std::vector<QString>               timePointLabels();

    void                               setTimePointTo(int timePointIndex);
    void                               updateAnimation();
    void                               advanceOneTimePoint();
    void                               updateAnimationPosition();



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
    void                               setModified(bool modifed);

    bool                               isAnimating() const;

	void           addView(QAbstractView* pView);
    void           removeView(QAbstractView* pView);
    void           updateAllViews(QAbstractView* pSender, QObject* hint = nullptr);

    vtkSmartPointer<vtkPropCollection> propCollection();

    // 
    QString                            currentFile();

    QDir                               defaultDir() const;


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
    MouseMode                interactorStyle;       // m_InteractorStyle
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
    CGeometryDlg*      m_GeometryDlg;
    CDataDlg*          m_DataDlg;
    CSolidDlg*         m_SolidDlg;
    CIsosurfaceDlg*    m_IsosurfaceDlg;
    CCropDlg*          m_CropDlg;
    CAnimationDlg*     m_AnimationDlg;
    CVectorDlg*        m_VectorDlg;
    CPathlinesDlg*     m_PathlinesDlg;
    CModelFeaturesDlg* m_ModelFeaturesDlg;
    COverlayDlg*       m_OverlayDlg;
***/

};

#endif // MVDOC_H
