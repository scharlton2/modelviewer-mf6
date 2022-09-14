#ifndef MVVIEW_H
#define MVVIEW_H

//#include <QObject>
#include "qabstractview.h"
#include <QPointer>

#include <vtkNew.h>

class vtkLight;
class vtkRenderer;
class QVTKOpenGLNativeWidget; //    vtkMFCWindow;
class vtkGenericOpenGLRenderWindow;
class vtkProp;

class mvGUISettings;
//class MvDoc;

enum class ResolutionType;
enum class SideType;


#include "mvdoc.h"

class MvView : public QAbstractView
{
    Q_OBJECT

public:
    explicit MvView(QObject *parent = nullptr);
    ~MvView() override;

    MvDoc*                  GetDocument();

    void                    resetExportImageParameters();
    //void                    RotateCamera(double angle);
    //void                    ElevateCamera(double angle);
    //void                    ResetViewpoint() { m_DoResetViewpoint = TRUE; }
    //void                    ResetCameraClippingRange();
    //void                    SetInteractorStyle(int interactorStyle);
    //void                    SetProjectionToPerspective();
    //void                    SetProjectionToParallel();
    //void                    SwitchOnHeadlight(BOOL switchOn);
    //void                    SetHeadlightIntensity(double intensity);
    //void                    SwitchOnAuxiliaryLight(BOOL switchOn);
    //void                    SetAuxiliaryLightIntensity(double intensity);
    //void                    SetAuxiliaryLightPosition(double x, double y, double z);
    //void                    SetBackgroundColor(double red, double green, double blue);
    //void                    GetViewSettings(mvGUISettings* gui);
    void                    applyViewSettings(mvGUISettings* gui);  // void ApplyViewSettings(mvGUISettings* gui);
    //void                    DiscardSavedViewpoint();

    QVTKOpenGLNativeWidget* mainWidget();
    void                    addActor(vtkProp* p);
    void                    addViewProp(vtkProp* p);

    void                    onUpdate(QAbstractView* sender, QObject* hint) override;   // CMvView::OnUpdate(CView *pSender, LPARAM lHint, CObject *pHint)

    void                    getViewSettings(mvGUISettings* gui);


    void                    onFileExportAsBmp(QWidget* parent);
    void                    onFileExportAnimation(QWidget* parent);


protected:

    void           PlaceHeadlightWithCamera();
    void           WriteBmp(const char* filename, bool useScreenResolution);


    // Parameters for exporting bitmap
    ResolutionType bitmapResolutionOption;        // m_BitmapResolutionOption
    SideType       bitmapSideOption;              // m_BitmapSideOption
    double         bitmapWidthInInches;           // m_BitmapWidthInInches
    double         bitmapHeightInInches;          // m_BitmapHeightInInches

    // parameters for exporting image sequence
    int            startIndex;
    int            endIndex;
    int            numberOfSteps;
    double         rotate;
    double         elevate;
    AnimationType  animationType;
    QString        outputFolder;
    QString        filePrefix;
    QString        fileStartNumber;


signals:

private:
    friend class MainWindow;

    QPointer<QVTKOpenGLNativeWidget>     widget;
    vtkRenderer*                         renderer;
    vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;


    vtkLight*                            headlight;
    vtkLight*                            auxiliaryLight;
};

#ifndef _DEBUG // debug version in MvView.cpp
inline MvDoc* MvView::GetDocument()
{
    return dynamic_cast<MvDoc*>(parent());
}
#endif

#endif // MVVIEW_H
