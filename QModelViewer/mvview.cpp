#include "mvview.h"

#include "mvdoc.h"
#include "mvGUISettings.h"

#include <QtWidgets>

#include <QVTKOpenGLNativeWidget.h>
#include <vtkGenericOpenGLRenderWindow.h>

#include <vtkCamera.h>
#include <vtkLight.h>
#include <vtkLightCollection.h>
#include <vtkRenderer.h>
#include <vtkProp.h>
#include <vtkRenderLargeImage.h>
#include <vtkBMPWriter.h>

#include "bitmapresolutiondialog.h"


MvView::MvView(QObject *parent)
    : QAbstractView(parent)
{
    renderer = vtkRenderer::New();
    renderer->SetBackground(1, 1, 1);

#ifdef NDEBUG
    renderer->GlobalWarningDisplayOff();
#endif

    widget    = new QVTKOpenGLNativeWidget();

    headlight = vtkLight::New();
    headlight->SetLightTypeToHeadlight();

    auxiliaryLight = vtkLight::New();
    auxiliaryLight->SetIntensity(0);

    renderer->AddLight(headlight);
    renderer->AddLight(auxiliaryLight);

    widget->renderWindow()->AddRenderer(renderer);

    // bitmap exporting
    bitmapResolutionOption = ResolutionType::rtScreen;
    bitmapSideOption       = SideType::stWidth;
    bitmapWidthInInches    = 0;
    bitmapHeightInInches   = 0;

    // export image sequance
    resetExportImageParameters();
}

MvView::~MvView()
{
    renderer->SetRenderWindow(nullptr);
    renderer->RemoveLight(headlight);
    renderer->RemoveLight(auxiliaryLight);

    renderer->Delete();
    headlight->Delete();
    auxiliaryLight->Delete();

    delete widget;
}

/////////////////////////////////////////////////////////////////////////////
// MvView diagnostics

#ifdef _DEBUG
//void MvView::AssertValid() const
//{
//    CView::AssertValid();
//}
//
//void MvView::Dump(CDumpContext &dc) const
//{
//    CView::Dump(dc);
//}

MvDoc* MvView::GetDocument() // non-debug version is inline
{
    MvDoc* doc = dynamic_cast<MvDoc*>(parent());
    assert(doc);
    return doc;
}
#endif //_DEBUG

QVTKOpenGLNativeWidget* MvView::mainWidget()
{
    return widget;
}

void MvView::addActor(vtkProp* p)
{
    this->renderer->AddActor(p);
}

void MvView::addViewProp(vtkProp* p)
{
    this->renderer->AddViewProp(p);
}

void MvView::applyViewSettings(mvGUISettings* gui)
{
    /*
    vtkCamera* camera = m_Renderer->GetActiveCamera();
    camera->SetPosition(gui->cameraPosition);
    camera->SetFocalPoint(gui->focalPoint);
    camera->SetViewUp(gui->viewUp);
    camera->OrthogonalizeViewUp();
    camera->SetParallelProjection(gui->parallelProjection);
    camera->SetParallelScale(gui->parallelScale);
    PlaceHeadlightWithCamera();
    SetHeadlightIntensity(gui->headlightIntensity);
    // m_Headlight->SetIntensity(gui->headlightIntensity);
    // m_AuxiliaryLight->SetIntensity(gui->auxiliaryLightIntensity);
    SetAuxiliaryLightIntensity(gui->auxiliaryLightIntensity);
    m_AuxiliaryLight->SetPosition(gui->auxiliaryLightDirection);
    SwitchOnHeadlight(gui->headlightOn);
    SwitchOnAuxiliaryLight(gui->auxiliaryLightOn);
    m_Renderer->ResetCameraClippingRange();
    if (gui->customBackground)
    {
        m_Renderer->SetBackground(gui->background);
    }
    else
    {
        m_Renderer->SetBackground(1, 1, 1);
    }
    */

    //{{
    vtkCamera* camera = this->renderer->GetActiveCamera();
    camera->SetPosition(gui->cameraPosition);
    camera->SetFocalPoint(gui->focalPoint);
    camera->SetViewUp(gui->viewUp);
    camera->OrthogonalizeViewUp();
    camera->SetParallelProjection(gui->parallelProjection);
    camera->SetParallelScale(gui->parallelScale);
    PlaceHeadlightWithCamera();
    //SetHeadlightIntensity(gui->headlightIntensity);
    //// m_Headlight->SetIntensity(gui->headlightIntensity);
    //// m_AuxiliaryLight->SetIntensity(gui->auxiliaryLightIntensity);
    //SetAuxiliaryLightIntensity(gui->auxiliaryLightIntensity);
    //m_AuxiliaryLight->SetPosition(gui->auxiliaryLightDirection);
    //SwitchOnHeadlight(gui->headlightOn);
    //SwitchOnAuxiliaryLight(gui->auxiliaryLightOn);
    this->renderer->ResetCameraClippingRange();
    if (gui->customBackground)
    {
        this->renderer->SetBackground(gui->background);
    }
    else
    {
        this->renderer->SetBackground(1, 1, 1);
    }
    //}}

    ////this->renderer->ResetCamera();
    this->mainWidget()->renderWindow()->Render();
}

void MvView::onUpdate(QAbstractView* sender, QObject* hint)
{
    //m_Renderer->GetViewProps()->RemoveAllItems();
    //vtkSmartPointer<vtkPropCollection> props = GetDocument()->GetPropCollection();
    //props->InitTraversal();
    //for (int i = 0; i < props->GetNumberOfItems(); i++)
    //{
    //    m_Renderer->AddViewProp(props->GetNextProp());
    //}
    //if (!GetDocument()->IsAnimating())
    //{
    //    BeginWaitCursor();
    //}
    //m_Renderer->ResetCameraClippingRange();
    //if (!GetDocument()->IsAnimating())
    //{
    //    EndWaitCursor();
    //}
    //CView::OnUpdate(pSender, lHint, pHint);

    //this->renderer->GetViewProps()->RemoveAllItems();
    //vtkSmartPointer<vtkPropCollection> props = GetDocument()->propCollection();
    //props->InitTraversal();
    //for (int i = 0; i < props->GetNumberOfItems(); i++)
    //{
    //    this->renderer->AddViewProp(props->GetNextProp());
    //}
    //if (!this->GetDocument()->isAnimating())
    //{
    //    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
    //}
    //this->renderer->ResetCameraClippingRange();
    //if (!GetDocument()->isAnimating())
    //{
    //    QGuiApplication::restoreOverrideCursor();
    //}

    //{{
    renderer->ResetCameraClippingRange();
    //}}

    widget->renderWindow()->Render();

    QAbstractView::onUpdate(sender, hint);
}

void MvView::resetExportImageParameters()
{
    m_StartIndex      = 0;
    m_EndIndex        = 0;
    rotate            = 0;
    elevate           = 0;
    m_NumberOfSteps   = 10;
    m_AnimationType   = AnimationType::atTime;
    outputFolder      = "";
    filePrefix        = "";
    fileStartNumber   = "001";
}


void MvView::getViewSettings(mvGUISettings* gui)
{
    /* void CMvView::GetViewSettings(mvGUISettings *gui)
    * 
    m_Renderer->GetBackground(gui->background);
    m_AuxiliaryLight->GetPosition(gui->auxiliaryLightDirection);
    vtkCamera* camera = m_Renderer->GetActiveCamera();
    camera->GetPosition(gui->cameraPosition);
    camera->GetFocalPoint(gui->focalPoint);
    camera->GetViewUp(gui->viewUp);
    gui->parallelProjection = camera->GetParallelProjection();
    gui->parallelScale      = camera->GetParallelScale();
    */
    this->renderer->GetBackground(gui->background);
    this->auxiliaryLight->GetPosition(gui->auxiliaryLightDirection);
    vtkCamera* camera = this->renderer->GetActiveCamera();
    camera->GetPosition(gui->cameraPosition);
    camera->GetFocalPoint(gui->focalPoint);
    camera->GetViewUp(gui->viewUp);
    gui->parallelProjection = camera->GetParallelProjection();
    gui->parallelScale      = camera->GetParallelScale();
}

/////////////////////////////////////////////////////////////////////////////
// Protected methods

void MvView::PlaceHeadlightWithCamera()
{
    /*
    vtkCamera*          camera = m_Renderer->GetActiveCamera();
    vtkLightCollection* lights = m_Renderer->GetLights();
    lights->InitTraversal();
    vtkLight* light = lights->GetNextItem();
    light->SetPosition(camera->GetPosition());
    light->SetFocalPoint(camera->GetFocalPoint());
    */

    vtkCamera*          camera = this->renderer->GetActiveCamera();
    vtkLightCollection* lights = this->renderer->GetLights();
    lights->InitTraversal();
    vtkLight* light = lights->GetNextItem();
    assert(light == this->headlight);
    light->SetPosition(camera->GetPosition());
    light->SetFocalPoint(camera->GetFocalPoint());
}

void MvView::WriteBmp(const char* filename, bool useScreenResolution)
{
    this->widget->renderWindow()->SetUseOffScreenBuffers(true);

    int size[2];
    size[0]             = this->widget->renderWindow()->GetSize()[0];
    size[1]             = this->widget->renderWindow()->GetSize()[1];

    // Parameters for writing bitmap at screen resolution
    int   width         = size[0];
    int   height        = size[1];
    float magnification = 1;

    if (!useScreenResolution)
    {
        if (bitmapResolutionOption == ResolutionType::rt150ppi)
        {
            // 150 ppi
            if (bitmapSideOption == SideType::stWidth)
            {
                width         = bitmapWidthInInches * 150;
                height        = (((long)width) * size[1]) / size[0];
                magnification = ((float)width) / size[0];
            }
            else
            {
                height        = bitmapHeightInInches * 150;
                width         = (((long)height) * size[0]) / size[1];
                magnification = ((float)height) / size[1];
            }
        }
        else if (bitmapResolutionOption == ResolutionType::rt300ppi)
        {
            // 300 ppi
            if (bitmapSideOption == SideType::stWidth)
            {
                width         = bitmapWidthInInches * 300;
                height        = (((long)width) * size[1]) / size[0];
                magnification = ((float)width) / size[0];
            }
            else
            {
                height        = bitmapHeightInInches * 300;
                width         = (((long)height) * size[0]) / size[1];
                magnification = ((float)height) / size[1];
            }
        }
    }
    int dataWidth = ((width * 3 + 3) / 4) * 4;



    // temporarily change color bar and text size
    MvDoc*        doc               = GetDocument();
    int           colorBarWidth     = doc->GetColorBarWidth();
    int           colorBarHeight    = doc->GetColorBarHeight();
    int           colorBarOffset    = doc->GetColorBarOffset();
    int           colorBarFontSize  = doc->GetColorBarFontSize();
    int           timeLabelFontSize = doc->GetTimeLabelFontSize();
    const double* pos               = doc->GetTimeLabelPosition();
    double        timeLabelPos[2];
    timeLabelPos[0] = pos[0];
    timeLabelPos[1] = pos[1];
    if (magnification != 1)
    {
        doc->SetColorBarSize((int)(colorBarWidth * magnification),
                             (int)(colorBarHeight * magnification),
                             (int)(colorBarOffset * magnification), false);
        doc->SetColorBarFontSize((int)(colorBarFontSize * magnification), false);
        doc->SetTimeLabelFontSize((int)(timeLabelFontSize * magnification), false);
        doc->SetTimeLabelPosition(timeLabelPos[0] * magnification,
                                  timeLabelPos[1] * magnification, false);
    }

    // render to memory and write bitmap data
    this->widget->renderWindow()->Render();
    {
        // vtkRenderLargeImage
        this->widget->renderWindow()->SetSize(width, height);

        vtkNew<vtkRenderLargeImage> renderLarge;
        renderLarge->SetInput(renderer);
        renderLarge->SetMagnification(1);

        vtkNew<vtkBMPWriter> bmpWriter;
        bmpWriter->SetFileName(filename);
        bmpWriter->SetInputConnection(renderLarge->GetOutputPort());
        bmpWriter->Write();
    }
    if (magnification != 1)
    {
        doc->SetColorBarSize(colorBarWidth, colorBarHeight, colorBarOffset, false);
        doc->SetColorBarFontSize(colorBarFontSize, false);
        doc->SetTimeLabelFontSize(timeLabelFontSize, false);
        doc->SetTimeLabelPosition(timeLabelPos[0], timeLabelPos[1], false);
    }
    if (magnification != 1)
    {
        this->widget->renderWindow()->SetSize(size);
    }
    this->widget->renderWindow()->SetUseOffScreenBuffers(false);
}

void MvView::onFileExportAsBmp(QWidget* parent)
{
    BitmapResolutionDialog dlg(parent);
    dlg.resolutionOption = bitmapResolutionOption;
    dlg.sideOption       = bitmapSideOption;
    dlg.widthInInches    = bitmapWidthInInches;
    dlg.heightInInches   = bitmapHeightInInches;
    if (dlg.exec() != QDialog::Accepted)
    {
        return;
    }
    bitmapResolutionOption = dlg.resolutionOption;
    bitmapSideOption       = dlg.sideOption;
    bitmapWidthInInches    = dlg.widthInInches;
    bitmapHeightInInches   = dlg.heightInInches;

    QString fileName = QFileDialog::getSaveFileName(parent,
                                                tr("Save As"),
                                                tr(""),
                                                tr("Bitmap Files (*.bmp);;All Files (*.*)"));

    PlaceHeadlightWithCamera();

    vtkLightCollection* lights =  this->renderer->GetLights();
    lights->InitTraversal();
    
    // get headlight
    assert(lights->IsItemPresent(this->headlight));
    vtkLight* light = lights->GetNextItem();
    assert(light == this->headlight);

    // get auxilliary light
    assert(lights->IsItemPresent(this->auxiliaryLight));
    light = lights->GetNextItem();
    assert(light == this->auxiliaryLight);

    vtkCamera* camera = this->renderer->GetActiveCamera();

    camera->Azimuth(this->rotate);
    camera->Elevation(this->elevate);

    if (this->elevate != 0)
    {
        camera->OrthogonalizeViewUp();
    }

    light->SetPosition(camera->GetPosition());
    light->SetFocalPoint(camera->GetFocalPoint());

    QApplication::setOverrideCursor(Qt::WaitCursor);
    WriteBmp(fileName.toStdString().c_str(), this->bitmapResolutionOption == ResolutionType::rtScreen);
    QApplication::restoreOverrideCursor();
}

void MvView::onFileExportAnimation(QWidget* parent)
{
    QMessageBox::information(parent, "TODO", "Not implemented");
}
