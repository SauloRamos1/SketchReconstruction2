set(RENDERING_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
include_directories(${RENDERING_DIR})

pkg_search_module(GLFW REQUIRED glfw3)
set(GLFW_LIBRARIES ${GLFW_STATIC_LIBRARIES})

set(
  RENDERING_SOURCES
  ${RENDERING_DIR}/main.cpp
  ${RENDERING_DIR}/glwidget.cpp
  )

add_executable(
  rendering
  ${RENDERING_SOURCES}
  ${TUCANO_SOURCES}
  )

set_property(
  TARGET rendering
  PROPERTY COMPILE_FLAGS
  "-DTUCANOSHADERDIR=${TUCANO_EFFECTS_DIR}/shaders/ -Wall -g -std=c++11"
  )

target_link_libraries(
  rendering
  ${GLEW_LIBRARIES}
  ${GLFW_LIBRARIES}
  ${OPENGL_LIBRARIES}
  )
