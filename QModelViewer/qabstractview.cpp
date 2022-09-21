#include "qabstractview.h"

#include "mvGUISettings.h"

#include <vtkCamera.h>
#include <vtkLight.h>
#include <vtkLightCollection.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>

#include <QVTKOpenGLNativeWidget.h>
#include <QAction>


#include <cassert>

QAbstractView::QAbstractView(QObject *parent)
    : QObject{parent}
{
    renderer = vtkRenderer::New();
    renderer->SetBackground(1, 1, 1);

#ifdef NDEBUG
    renderer->GlobalWarningDisplayOff();
#else
    //renderer->SetDebug(true);
#endif

    //{{
    widget    = new QVTKOpenGLNativeWidget();
    widget->renderWindow()->AddRenderer(renderer);
    //}}

    headlight = vtkLight::New();
    headlight->SetLightTypeToHeadlight();

    auxiliaryLight = vtkLight::New();
    auxiliaryLight->SetIntensity(0);

    renderer->AddLight(headlight);
    renderer->AddLight(auxiliaryLight);
}

QAbstractView::~QAbstractView()
{
    renderer->SetRenderWindow(nullptr);
    renderer->RemoveLight(headlight);
    renderer->RemoveLight(auxiliaryLight);

    renderer->Delete();
    headlight->Delete();
    auxiliaryLight->Delete();

    delete widget;
}


void QAbstractView::onUpdate(QAbstractView* sender, QObject* /*hint*/)
{
    assert(sender != this);
    widget->update();
}

void QAbstractView::getViewSettings(mvGUISettings* gui)
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

void QAbstractView::applyViewSettings(mvGUISettings* gui)
{
    vtkCamera* camera = this->renderer->GetActiveCamera();
    camera->SetPosition(gui->cameraPosition);
    camera->SetFocalPoint(gui->focalPoint);
    camera->SetViewUp(gui->viewUp);
    camera->OrthogonalizeViewUp();
    camera->SetParallelProjection(gui->parallelProjection);
    camera->SetParallelScale(gui->parallelScale);

    PlaceHeadlightWithCamera();

    setHeadlightIntensity(gui->headlightIntensity);
    setAuxiliaryLightIntensity(gui->auxiliaryLightIntensity);
    auxiliaryLight->SetPosition(gui->auxiliaryLightDirection);
    switchOnHeadlight(gui->headlightOn);
    switchOnAuxiliaryLight(gui->auxiliaryLightOn);


    this->renderer->ResetCameraClippingRange();
    if (gui->customBackground)
    {
        this->renderer->SetBackground(gui->background);
    }
    else
    {
        this->renderer->SetBackground(1, 1, 1);
    }
}

void QAbstractView::resetViewPoint()
{
    // if (!this->renderer->GetViewProps()->GetNumberOfItems()) return;
    this->renderer->ResetCamera();
    this->renderer->ResetCameraClippingRange();
}

void QAbstractView::resetCameraClippingRange()
{
    this->renderer->ResetCameraClippingRange();
}

/////////////////////////////////////////////////////////////////////////////
// Protected methods

void QAbstractView::PlaceHeadlightWithCamera()
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

void QAbstractView::addActor(vtkProp* p)
{
    renderer->AddActor(p);
}

void QAbstractView::addViewProp(vtkProp* p)
{
    renderer->AddViewProp(p);
}

void QAbstractView::removeAllViewProps()
{
    renderer->RemoveAllViewProps();
}

void QAbstractView::setProjectionToPerspective()
{
    renderer->GetActiveCamera()->ParallelProjectionOff();
}

void QAbstractView::setProjectionToParallel()
{
    renderer->GetActiveCamera()->ParallelProjectionOn();
}




void QAbstractView::switchOnHeadlight(bool switchOn)
{
    if (switchOn)
    {
        headlight->SetIntensity(headlightIntensity);
    }
    else
    {
        headlightIntensity = headlight->GetIntensity();
        headlight->SetIntensity(0);
    }
}

void QAbstractView::setHeadlightIntensity(double intensity)
{
    headlight->SetIntensity(intensity);
    headlightIntensity = intensity;
}

void QAbstractView::switchOnAuxiliaryLight(bool switchOn)
{
    if (switchOn)
    {
        auxiliaryLight->SetIntensity(auxiliaryLightIntensity);
    }
    else
    {
        auxiliaryLightIntensity = auxiliaryLight->GetIntensity();
        auxiliaryLight->SetIntensity(0);
    }
}

void QAbstractView::setAuxiliaryLightIntensity(double intensity)
{
    auxiliaryLight->SetIntensity(intensity);
    auxiliaryLightIntensity = intensity;
}

void QAbstractView::setAuxiliaryLightPosition(double x, double y, double z)
{
    auxiliaryLight->SetPosition(x, y, z);
}















