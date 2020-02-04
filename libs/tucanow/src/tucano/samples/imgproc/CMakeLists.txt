set(IMGPROC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
include_directories(${IMGPROC_DIR})

pkg_search_module(GLFW REQUIRED glfw3)
set(GLFW_LIBRARIES ${GLFW_STATIC_LIBRARIES})

set(
  IMGPROC_SOURCES
  ${IMGPROC_DIR}/main.cpp
  ${IMGPROC_DIR}/texturewidget.cpp
  )

add_executable(
  imgproc
  ${IMGPROC_SOURCES}
  ${TUCANO_SOURCES}
  )

set_property(
  TARGET imgproc
  PROPERTY COMPILE_FLAGS
  "-DTUCANOSHADERDIR=${TUCANO_EFFECTS_DIR}/shaders/ -Wall -g -std=c++11"
  )

target_link_libraries(
  imgproc
  ${GLEW_LIBRARIES}
  ${GLFW_LIBRARIES}
  ${OPENGL_LIBRARIES}
  )
