cmake_minimum_required(VERSION 3.16)

project(QChartRealTime VERSION 0.1 LANGUAGES CXX)

if(NOT DEFINED INSTALL_EXAMPLESDIR)
    set(INSTALL_EXAMPLESDIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}/charts/zoomlinechart")
find_package(Qt6 REQUIRED COMPONENTS Charts Core Gui)
find_package(Qt6 REQUIRED COMPONENTS Core)
qt_standard_project_setup()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

qt_add_executable(QChartRealTime
  main.cpp
  qchartmain.cpp
  qchartmain.h
  QChartRealTime.h
  QChartRealTime.cpp
  qchartmain.ui
  MultiLineseries.h
  MultiLineseries.cpp
)


target_link_libraries(QChartRealTime PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt::Charts Qt::Core Qt::Gui)
target_link_libraries(QChartRealTime PRIVATE Qt6::Core)

set_target_properties(QChartRealTime PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS QChartRealTime
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(QChartRealTime)
endif()
