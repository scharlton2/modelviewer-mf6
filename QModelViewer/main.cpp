#include <QApplication>
#include <QGuiApplication>
#include <QSurfaceFormat>

#include <QVTKOpenGLNativeWidget.h>

#include "mainwindow.h"

/////#if defined(_MSC_VER)
//#include <shlwapi.h>
/////#endif

int main(int argc, char** argv)
{

//#if defined(_MSC_VER)
//    char buffer[300];
//    PathCanonicalizeA(buffer, ".");
//#endif

  // needed to ensure appropriate OpenGL context is created for VTK rendering.
  QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());

  QApplication app(argc, argv);
  ////QGuiApplication::setApplicationDisplayName(QObject::tr("Model Viewer for Modflow 6"));
  QGuiApplication::setWindowIcon(QIcon(":/images/ModelViewer-16x16.png"));

  MainWindow window;
  QObject::connect(&window, SIGNAL(exit), qApp, SLOT(closeAllWindows));
  window.show();

  return app.exec();
}
