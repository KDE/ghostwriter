cmake_minimum_required(VERSION 3.16)

remove_definitions(-DQT_USE_QSTRINGBUILDER)

include(FeatureSummary)

option(QTAWESOME_USE_FREE "Use Font Awesome Free" OFF)
option(QTAWESOME_USE_PRO "Use Font Awesome Pro" OFF)

if(QTAWESOME_USE_FREE AND NOT QTAWESOME_USE_PRO)
  message(STATUS "Using Font Awesome Free")
  set(_config Free)
elseif(QTAWESOME_USE_PRO AND NOT QTAWESOME_USE_FREE)
  message(STATUS "Using Font Awesome Pro")
  set(_config Pro)
else()
  message(FATAL_ERROR
    "Define one and only one of QTAWESOME_USE_FREE or QTAWESOME_USE_PRO"
  )
endif()

set(SOURCES
  QtAwesomeAnim.cpp
  QtAwesomeAnim.h
  QtAwesome.cpp
  QtAwesome.h
)

find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} REQUIRED COMPONENTS Gui Widgets)

add_library(QtAwesome STATIC
    ${SOURCES}
    QtAwesome${_config}.qrc
)
target_include_directories(QtAwesome INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
set_target_properties(QtAwesome PROPERTIES
  AUTOMOC TRUE
  AUTORCC TRUE
)
target_link_libraries(QtAwesome PRIVATE
    Qt::Gui
    Qt::Widgets
)

if(_config STREQUAL "Pro")
  target_compile_definitions(QtAwesome PRIVATE
    FONT_AWESOME_PRO=1
  )
endif()

unset(_config)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

