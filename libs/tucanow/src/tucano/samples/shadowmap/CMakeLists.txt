set(SHADOWMAP_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
include_directories(${SHADOWMAP_DIR})

pkg_search_module(GLFW REQUIRED glfw3)
set(GLFW_LIBRARIES ${GLFW_STATIC_LIBRARIES})

set(
  SHADOWMAP_SOURCES
  ${SHADOWMAP_DIR}/main.cpp
  ${SHADOWMAP_DIR}/simplewidget.cpp
  )

add_executable(
  shadowmap
  ${SHADOWMAP_SOURCES}
  ${TUCANO_SOURCES}
  )

set_property(
  TARGET shadowmap
  PROPERTY COMPILE_FLAGS
  "-DTUCANOSHADERDIR=${TUCANO_EFFECTS_DIR}/shaders/ -Wall -g -std=c++11"
  )

target_link_libraries(
  shadowmap
  ${GLEW_LIBRARIES}
  ${GLFW_LIBRARIES}
  ${OPENGL_LIBRARIES}
  )
