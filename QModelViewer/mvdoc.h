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


// modeless dialogs
class DataDialog;
class ColorBarDialog;
class LightingDialog;
class GridDialog;
class GeometryDialog;
class OverlayDialog;
class SolidDialog;
class IsosurfaceDialog;
class VectorDialog;
class PathlinesDialog;
class ModelFeaturesDialog;
class CropDialog;
class AnimationDialog;


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


    QString                            activeScalarName() const;
    QString                            modelName() const;

    // Get methods
    //char**                             GetTimePointLabels();
    //int                                GetNumberOfTimePoints();
    QStringList                        timePointLabels();
    void                               getScalarDataRange(double* range);
    //void                               GetVectorMagnitudeRange(double* range);
    //char*                              GetModelName();
    //char*                              GetDataName();
    //GridType                           GetGridType();


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
    void                               SetTimeLabelFontSize(int size, bool update = true);
    int                                GetTimeLabelFontSize() const;
    void                               SetTimeLabelPosition(double x, double y, bool update = true);
    const double*                      GetTimeLabelPosition() const;

    // Color Bar
    void                               setColorBarEndPoints(double valueBlue, double valueRed);
    void                               UseLinearColorBar();
    void                               UseLogColorBar();
    void                               SetColorBarSize(int width, int height, int offset, bool update = true);
    void                               SetColorBarFontSize(int fontSize, bool update = true);
    void                               SetColorBarNumberOfLabels(int numLabels, bool update = true);
    void                               SetColorBarLabelPrecision(int precision, bool update = true);
    void                               SetColorBarTextColor(double red, double green, double blue, bool update = true);
    void                               SetColorBarColorScheme(int Value);
    int                                GetColorBarWidth();
    int                                GetColorBarHeight();
    int                                GetColorBarOffset();
    int                                GetColorBarFontSize();
    int                                GetColorBarColorScheme();
    unsigned long                      GetColorBarFirstCustomColor();
    unsigned long                      GetColorBarLastCustomColor();
    void                               SetColorBarFirstCustomColor(unsigned long value);
    void                               SetColorBarFirstCustomColor(QColor color);
    void                               SetColorBarLastCustomColor(unsigned long value);
    void                               SetColorBarLastCustomColor(QColor color);

    int                                getColorBarSource();
    void                               setColorBarSource(int value);
    double                             getColorBarValueBlue() const;
    double                             getColorBarValueRed() const;

    
    bool                               modified() const;
    bool                               isModified() const;
    void                               setModified(bool modifed);

    bool                               isAnimating() const;

	void                               addView(QAbstractView* pView);
    void                               removeView(QAbstractView* pView);
    void                               updateAllViews(QAbstractView* pSender, QObject* hint = nullptr);

    vtkSmartPointer<vtkPropCollection> propCollection();

    // 
    QString                            currentFile();

    bool                               maybeSave();

    QDir                               defaultDir() const;

    // Toolbox->Data
    void                               onToolboxData();
    void                               onUpdateToolboxData(QAction* action);
    void                               setScalarDataTypeTo(int index);

    // Toolbox->Color Bar
    void                               onToolboxColorBar();
    void                               onUpdateToolboxColorBar(QAction* action);
    //void                               setScalarDataTypeTo(int index);


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


    // Pathlines
    //void                               SetPathlineRepresentationToLine();
    //void                               SetPathlineRepresentationToTube();
    //void                               SetPathlineTubeDiameter(double diameter);
    //void                               SetPathlineColorBarEndPoints(double valueBlue, double valueRed);
    //void                               SetPathlineTimeClippingMode(int mode);
    //void                               SetPathlineTimeClippingRange(double minTime, double maxTime);
    void                               getPathlineTimeRange(double* range);
    //void                               SetPathlineLogTransform(int Value);
    bool                               hasPathlineData() const;




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

public:

    //int           GetAnimationSteps();
    //AnimationType GetAnimationType();
    //void          CropVectors(double xmin, double xmax,
    //                          double ymin, double ymax, double zmin, double zmax, int cropangle);

signals:

private:

    friend class MainWindow;

    void                      setCurrentFile(const QString& fileName);


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

    // Modeless dialog boxes (menu order)
    DataDialog*               dataDialog;           // ID_DATA_TOOL
    // SEPARATOR
    ColorBarDialog*           colorBarDialog;       // ID_COLOR_BAR_TOOL
    LightingDialog*           lightingDialog;       // ID_LIGHTING_TOOL
    // SEPARATOR
    GridDialog*               gridDialog;           // ID_GRID_TOOL
    GeometryDialog*           geometryDialog;       // ID_GEOMETRY_TOOL
    OverlayDialog*            overlayDialog;        // ID_OVERLAY_TOOL
    // SEPARATOR
    SolidDialog*              solidDialog;          // ID_SOLID_TOOL
    IsosurfaceDialog*         isosurfaceDialog;     // ID_ISOSURFACE_TOOL
    VectorDialog*             vectorDialog;         // ID_VECTOR_TOOL
    PathlinesDialog*          pathlinesDialog;      // ID_PATHLINES_TOOL
    ModelFeaturesDialog*      modelFeaturesDialog;  // ID_MODEL_FEATURES_TOOL
    // SEPARATOR
    CropDialog*               cropDialog;           // ID_CROP_TOOL
    AnimationDialog*          animationDialog;      // ID_ANIMATION_TOOL


    // Protected methods;
    void                      loadPreviousAppSettings();
    void                      saveCurrentAppSettings();
    
    void                      reinitializeToolDialogs();

    ///////////////////////////////////////////////////////////////////////////
    // updateToolDialogs

    void                      updateToolDialogs(mvGUISettings* gui);

    friend class ColorBarDialog;

    void                      updateSolidDialog();
    void                      updateIsosurfaceDialog();
    void                      updateColorBarDialog();
    void                      updateLightingDialog(mvGUISettings* gui);
    void                      updateGeometryDialog();
    void                      updateGridDialog();
    void                      updateAnimationDialog(mvGUISettings* gui);
    void                      updateVectorDialog();
    void                      updateCropDialog(mvGUISettings* gui);
    void                      updateDataDialog();
    void                      updatePathlinesDialog();
    void                      updateModelFeaturesDialog();
    void                      updateOverlayDialog();


    void                      onParallelProjection();
    void                      onUpdateParallelProjection(QAction* action);
    void                      onPerspectiveProjection();
    void                      onUpdatePerspectiveProjection(QAction* action);

    void                      onPreferences();
};


#endif // MVDOC_H
