#include "mvview.h"

#include <QtWidgets>

#include <QVTKOpenGLNativeWidget.h>
#include <vtkGenericOpenGLRenderWindow.h>

#include <vtkLight.h>
#include <vtkRenderer.h>
#include <vtkProp.h>

MvView::MvView(QObject *parent)
    : QObject{parent}
    //, mainWidget{nullptr}
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
    renderer->AddLight(headlight);

    widget->renderWindow()->AddRenderer(renderer);
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

QVTKOpenGLNativeWidget* MvView::mainWidget()
{
    return widget;
}

void MvView::addActor(vtkProp* p)
{
    renderer->AddActor(p);
}

void MvView::addViewProp(vtkProp* p)
{
    renderer->AddViewProp(p);
}
