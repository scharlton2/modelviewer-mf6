#ifndef MVVIEW_H
#define MVVIEW_H

#include <QObject>
#include <QPointer>

#include <vtkNew.h>

class vtkLight;
class vtkRenderer;
class QVTKOpenGLNativeWidget; //    vtkMFCWindow;
class vtkGenericOpenGLRenderWindow;
class vtkProp;
    // class mvGUISettings;

class MvView : public QObject
{
    Q_OBJECT

public:
    explicit MvView(QObject *parent = nullptr);
    ~MvView() override;

    QVTKOpenGLNativeWidget* mainWidget();
    void                    addActor(vtkProp* p);
    void                    addViewProp(vtkProp* p);


signals:

private:
    QPointer<QVTKOpenGLNativeWidget>     widget;
    vtkRenderer*                         renderer;
    vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;


    vtkLight*                            headlight;
    vtkLight*                            auxiliaryLight;
};

#endif // MVVIEW_H
