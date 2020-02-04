###############################################
# Preamble  
###############################################

cmake_minimum_required(VERSION 3.10)

project(TucanoWrapper)

set(VERSION_MAJOR 0 CACHE STRING "Tucanow major version number.")
set(VERSION_MINOR 1 CACHE STRING "Tucanow minor version number.")

# For vim's YouCompleteMe plugin
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


###############################################
# Set build type (debug, release, etc.)
###############################################

# If empty will use the system's default build type

set(TUCANOW_BUILD_TYPE "" CACHE STRING
    "Set build type to: Debug, Release, RelWithDebInfo, MinSizeRel, or leave empty")

if(TUCANOW_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "${TUCANOW_BUILD_TYPE}" CACHE INTERNAL "" FORCE)
endif()


###############################################
# Project options 
###############################################

option(TUCANOW_BUILD_SHARED_LIBRARY "Build tucanow as a shared library"  ON)
option(TUCANOW_BUILD_DOCS           "Build documentation with Doxygen"   ON)

if(TUCANOW_BUILD_SHARED_LIBRARY)
    set(TUCANOW_LIBRARY_TYPE "SHARED")

    # Never have problems using dlls again!
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
else()
    set(TUCANOW_LIBRARY_TYPE "STATIC")
endif()

if (TUCANOW_BUILD_DOCS)
    include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/BuildDocumentation.cmake")
endif(TUCANOW_BUILD_DOCS)



###############################################
# External libraries
###############################################

find_package(OpenGL REQUIRED)

if(WIN32)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/win32-libs")

else()
    # GLEW is required by Tucano
    find_package(GLEW REQUIRED)

    # Eigen3 is required by Tucano
    find_package(Eigen3 REQUIRED)
endif()



###############################################
# Create library
###############################################


set(TUCANOW_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/scene.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gui.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/misc.cpp
    )

add_library(tucanow ${TUCANOW_LIBRARY_TYPE} ${TUCANOW_SOURCES})

target_include_directories(tucanow
    PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

target_include_directories(tucanow SYSTEM
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/tucano
    )

target_link_libraries(tucanow 
    PUBLIC
    OpenGL::GL Eigen3::Eigen
    PRIVATE
    GLEW::GLEW
    )

target_compile_features(tucanow PUBLIC cxx_std_11)

# MSVC (as recently as version 19.15.26729) throws hundreds of warnings from
# it's own implementation of the STL -- CMake's SYSTEM directive seems
# ineffective to prevent this behaviour.  Thus, using -Wall in MSVC is,
# unfortunately, not possible.  

if(NOT MSVC) 
    target_compile_options(tucanow PRIVATE -Wall) 
endif()

# Constant M_PI is predefined on posix systems (which Windows allegedly is compatible with)
if(MSVC)
    target_compile_options(tucanow PRIVATE -DM_PI=3.14159265358979323846264338327950288)
endif()

# add_executable(mesh_viewer ${MESH_VIEWER_SOURCES} ${MESH_VIEWER_DIR}/src/main_load_ply.cpp)
# add_executable(mesh_viewer_spheres ${MESH_VIEWER_SOURCES} ${MESH_VIEWER_DIR}/src/main_spheres.cpp)


###############################################
# Copy shaders and assets to binary dir when done
###############################################

add_custom_command(TARGET tucanow POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/src/tucano/tucano/effects/shaders 
    ${CMAKE_BINARY_DIR}/shaders
    )

add_custom_command(TARGET tucanow POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/src/tucano/samples/assets
    ${CMAKE_BINARY_DIR}/samples/assets
    )


###############################################
# Set install and export CMake targets
###############################################

# # Must use GNUInstallDirs to install libraries into correct
# # locations on all platforms.

# include(GNUInstallDirs)

# # Modify installation path in cmake generation step by setting var:
# # CMAKE_INSTALL_PREFIX

# install(TARGETS tucanow
#     EXPORT tucanow-export
#     LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
#     COMPONENT tucanow
#     )

# install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include"
#     DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
#     )

# install(EXPORT tucanow-export
#     FILE tucanow-targets.cmake
#     NAMESPACE tucanow::
#     DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/tucanow
#     )
add_library(tucanow::tucanow ALIAS tucanow)

# # export(TARGETS tucanow FILE tucanow.cmake)
