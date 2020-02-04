set(SIMPLETEXTURE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
include_directories(${SIMPLETEXTURE_DIR})

pkg_search_module(GLFW REQUIRED glfw3)
set(GLFW_LIBRARIES ${GLFW_STATIC_LIBRARIES})

set(
  SIMPLETEXTURE_SOURCES
  ${SIMPLETEXTURE_DIR}/main.cpp
  ${SIMPLETEXTURE_DIR}/texturewidget.cpp
  )

add_executable(
  simpleTexture
  ${SIMPLETEXTURE_SOURCES}
  ${TUCANO_SOURCES}
  )

set_property(
  TARGET simpleTexture
  PROPERTY COMPILE_FLAGS
  "-DTUCANOSHADERDIR=${TUCANO_EFFECTS_DIR}/shaders/ -Wall -g -std=c++11"
  )

target_link_libraries(
  simpleTexture
  ${GLEW_LIBRARIES}
  ${GLFW_LIBRARIES}
  ${OPENGL_LIBRARIES}
  )
