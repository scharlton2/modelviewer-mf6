#ifndef MVDOC_H
#define MVDOC_H

#include <vector>
#include <string>

#include <QObject>
#include <QString>

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
    explicit MvDoc(QObject *parent = nullptr);
    ~MvDoc() override;


    const QString& pathName() const;
    //void setPathName(const QString* pathName);


    QString dataName() const;
    QString modelName() const;


signals:

private:

    void                     loadPreviousAppSettings();
    void                     saveCurrentAppSettings();

    QString                  _pathName;              // CDocument::m_strPathName


    AnimationType            animationType;         // m_AnimationType
    size_t                   animationSteps;        // m_AnimationSteps
    MouseMode                interactorStyle;       // m_InteractorStyle
    ProjectionType           projectionMode;        // m_ProjectionMode
    size_t                   numberOfModels;        // m_NumberOfModels
    bool                     readyToClose;          // m_ReadyToClose
    std::vector<std::string> modelNames;            // m_ModelNames
    std::string              defaultModel;          // m_DefaultModel
    bool                     startup;               // m_Startup
    bool                     isAnimating;           // m_IsAnimating
/***
    mvGUISettings*     m_GUI;
***/

    double                   defaultXOrigin;        // m_default_xorigin
    double                   defaultYOrigin;        // m_default_yorigin
    double                   defaultAngRot;         // m_default_angrot

/***

    // The visualization pipeline manager
    mvManager*         m_Manager;

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
