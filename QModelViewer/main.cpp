#include <QApplication>
#include <QGuiApplication>
#include <QSurfaceFormat>

#include <QVTKOpenGLNativeWidget.h>

#include "mainwindow.h"

int main(int argc, char** argv)
{
#if defined(_DEBUG)
  int* leak1968 = (int*)malloc(1968);
#endif

  // needed to ensure appropriate OpenGL context is created for VTK rendering.
  QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());

  QApplication app(argc, argv);
  QGuiApplication::setApplicationDisplayName(QObject::tr("Model Viewer for Modflow 6"));
  QGuiApplication::setWindowIcon(QIcon(":/images/ModelViewer-16x16.png"));

  MainWindow window;
  window.show();

  int ret = app.exec();

#if defined(_DEBUG)
  int* leak2022 = (int*)malloc(2022);
#endif

  return ret;
}