cmake_minimum_required(VERSION 3.20)

project (
  QMvProject${VTK_SUFFIX}
  VERSION 1.1.0
  LANGUAGES CXX C
)

# set standard directory locations
include(GNUInstallDirs)

if(APPLE)
  set(CMAKE_INSTALL_BINDIR .)
endif()

# check for vtk
find_package(VTK REQUIRED)

find_package(VTK
  COMPONENTS
    CommonColor
    CommonComputationalGeometry
    CommonCore
    CommonDataModel
    CommonExecutionModel
    CommonMath
    CommonMisc
    CommonSystem
    CommonTransforms
    DICOMParser
    doubleconversion
    expat
    FiltersCore
    FiltersExtraction
    FiltersGeneral
    FiltersGeometry
    FiltersHybrid
    FiltersModeling
    FiltersSources
    FiltersStatistics
    fmt
    freetype
    glew
    # GUISupportMFC
    GUISupportQt
    ImagingCore
    ImagingSources
    InteractionStyle
    IOCore
    IOImage
    IOLegacy
    IOXML
    IOXMLParser
    jpeg
    kissfft
    loguru
    lz4
    lzma
    metaio
    ParallelCore
    ParallelDIY
    png
    pugixml
    RenderingCore
    RenderingFreeType
    RenderingLOD
    RenderingOpenGL2
    RenderingUI
    vtksys
    tiff
    zlib
  REQUIRED
)

if (NOT VTK_DIR)
  message(FATAL_ERROR "Please set VTK_DIR.")
endif()

if (NOT VTK_FOUND)
  message("${VTK_NOT_FOUND_MESSAGE}")
  return()
endif()

option(MV_DEBUG_MEMORY_LEAKS "Build for memory debugging" OFF)
mark_as_advanced(MV_DEBUG_MEMORY_LEAKS)
if (MV_DEBUG_MEMORY_LEAKS)
  set(BUILD_SHARED_LIBS OFF)
else()
  set(BUILD_SHARED_LIBS ON)
endif()

if(NOT APPLE)
  add_subdirectory(doc)
  add_subdirectory(examples)
endif()
add_subdirectory(mv)
add_subdirectory(QModelViewer)

# set default startup project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT QModelViewer)

# based on code from CMake's QtDialog/CMakeLists.txt
macro(install_qt5_plugin _qt_plugin_name _qt_plugins_var)
  get_target_property(_qt_plugin_path "${_qt_plugin_name}" LOCATION)
  if(EXISTS "${_qt_plugin_path}")
    get_filename_component(_qt_plugin_file "${_qt_plugin_path}" NAME)
    get_filename_component(_qt_plugin_type "${_qt_plugin_path}" PATH)
    get_filename_component(_qt_plugin_type "${_qt_plugin_type}" NAME)
    if(APPLE)
      set(_qt_plugin_dest "QModelViewer.app/Contents/PlugIns/${_qt_plugin_type}")
    elseif(WIN32)
      set(_qt_plugin_dest "bin/${_qt_plugin_type}")
    endif()
    install(FILES "${_qt_plugin_path}"
        DESTINATION "${_qt_plugin_dest}")
    set(${_qt_plugins_var}
        "${${_qt_plugins_var}};\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${_qt_plugin_dest}/${_qt_plugin_file}")
  else()
    message(FATAL_ERROR "QT plugin ${_qt_plugin_name} not found")
  endif()
endmacro()

if(APPLE)
  install_qt5_plugin("Qt5::QCocoaIntegrationPlugin" QT_PLUGINS)
endif()

if(WIN32)
  install_qt5_plugin("Qt5::QWindowsIntegrationPlugin" QT_PLUGINS)
endif()

if(APPLE)
  set(APP "\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/QModelViewer.app")
endif()
if(WIN32)
  set(APP "\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/bin/QModelViewer.exe")
endif()

# add qt5 dir to DIRS
set(DIRS "${Qt5Widgets_DIR}")
if(NOT DIRS MATCHES "/$")
  string(APPEND DIRS "/")
endif()
if(APPLE)
  string(APPEND DIRS "../..")
endif()
if(WIN32)
  string(APPEND DIRS "../../../bin")
endif()

# add qt5-vtk to DIRS
if(APPLE)
  list(APPEND DIRS "${VTK_PREFIX_PATH}/lib")
endif()
if(WIN32)
  list(APPEND DIRS "${VTK_PREFIX_PATH}/bin")
endif()

message(STATUS "CMAKE_BUNDLE_LOCATION: ${CMAKE_BUNDLE_LOCATION}")
message(STATUS "APP: ${APP}")
message(STATUS "QT_PLUGINS: ${QT_PLUGINS}")
message(STATUS "DIRS: ${DIRS}")
message(STATUS "VTK_LIBRARIES: ${VTK_LIBRARIES}")

install(CODE "include(BundleUtilities)
    fixup_bundle(\"${APP}\" \"${QT_PLUGINS}\" \"${DIRS}\")")

set(CPACK_GENERATOR "DragNDrop")
set(CPACK_PACKAGE_NAME "QModelViewer")
include(CPack)

# cmake variable debugging
if (OFF)
  get_cmake_property(_variableNames VARIABLES)
  list (SORT _variableNames)
  foreach(_variableName ${_variableNames})
    message(STATUS "${_variableName}=${${_variableName}}")
  endforeach()
endif()
