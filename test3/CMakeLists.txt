cmake_minimum_required(VERSION 3.8...3.12 FATAL_ERROR)
project(SimpleView)

# set standard directory locations
include(GNUInstallDirs)

find_package(VTK REQUIRED
  COMPONENTS
    CommonCore
    GUISupportQt
    InfovisCore
    RenderingFreeType
    ViewsQt)
if (NOT VTK_FOUND)
  message("Skipping example: ${VTK_NOT_FOUND_MESSAGE}")
  return ()
endif ()

find_package(Qt5 REQUIRED COMPONENTS Widgets Gui)
if (NOT TARGET Qt5::Widgets OR NOT TARGET Qt5::Gui)
  message("Skipping example: ${Qt5_NOT_FOUND_MESSAGE}")
  return ()
endif ()

# Set your files and resources here
set(Srcs
  SimpleView.cxx
  main.cxx)

set(Hdrs
  SimpleView.h)

set(UIs
  SimpleView.ui)

set(QRCs
  Icons/icons.qrc)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

source_group("Resources" FILES
  ${UIs}
  ${QRCs})

add_executable(SimpleView WIN32 MACOSX_BUNDLE
  ${Srcs} ${Hdrs} ${UIs} ${QRCs})
target_link_libraries(SimpleView
  PRIVATE
    ${VTK_LIBRARIES}
    Qt5::Gui
    Qt5::Widgets)

# install
install(TARGETS SimpleView
  BUNDLE DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_BINDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# install vtk dlls
foreach(_lib ${VTK_LIBRARIES})
  install(FILES $<TARGET_FILE:${_lib}>
    DESTINATION ${CMAKE_INSTALL_BINDIR}
  )
endforeach()

# install runtimes
include (InstallRequiredSystemLibraries)
