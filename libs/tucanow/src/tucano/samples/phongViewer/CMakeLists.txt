set(PHONGVIEWER_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
include_directories(${PHONGVIEWER_DIR})

pkg_search_module(GLFW REQUIRED glfw3)
set(GLFW_LIBRARIES ${GLFW_STATIC_LIBRARIES})

set(
  PHONGVIEWER_SOURCES
  ${PHONGVIEWER_DIR}/main.cpp
  ${PHONGVIEWER_DIR}/simplewidget.cpp
  )

add_executable(
  phongViewer
  ${PHONGVIEWER_SOURCES}
  ${TUCANO_SOURCES}
  )

set_property(
  TARGET phongViewer
  PROPERTY COMPILE_FLAGS
  "-DTUCANOSHADERDIR=${TUCANO_EFFECTS_DIR}/shaders/ -Wall -g -std=c++11"
  )

target_link_libraries(
  phongViewer
  ${GLEW_LIBRARIES}
  ${GLFW_LIBRARIES}
  ${OPENGL_LIBRARIES}
  )
