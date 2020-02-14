cmake_minimum_required(VERSION 3.10.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set the project name
project(SketchCanvas)

# Project's options
option(SKETCH_CANVAS_DEPLOY_QT
    "Automatically copy Qt resources to build dir (Windows only)" ON)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#ICONS-

# Find the Qt5 library

#set (CMAKE_PREFIX_PATH "C:\\Qt\\5.12.3\\msvc2017_64")
#set (CMAKE_PREFIX_PATH "C:\\Qt\\5.12.5\\msvc2017_64")
#set(Qt5_DIR "C:\\Qt\\5.12.5\\mingw73_64\\lib\\cmake\\Qt5" CACHE PATH "Initial cache" FORCE)
find_package(Qt5 COMPONENTS Core Gui Widgets CONFIG REQUIRED)

# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)

# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)

#ICONS
set(CMAKE_AUTORCC ON)

# Include the Tucano wrapper (tucanow)
add_subdirectory(${CMAKE_SOURCE_DIR}/libs/tucanow)

# The following is only necessary if building
# tucanow as a dll in windows

set_target_properties(tucanow PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

set_target_properties(tucanow PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Populate a cmake variable with the project's sources
set(SOURCES
    src/main.cpp
    src/main_window.cpp
    src/canvas.cpp
    src/scene.cpp
    src/canvasmediator.cpp
    src/input_sketch.cpp
    src/halfedge.cpp
    src/openglcanvas.cpp
#    src/canvas2.cpp
#    src/scene2.cpp
#    src/input_sketch2.cpp
    src/layers.cpp
    src/sketch_utilities.cpp
    src/sketch_library.cpp
    src/openglmediator.cpp

    )

# Populate a cmake variable with the project's headers
set(HEADERS
    include/main_window.h
    include/canvas.h
    include/scene.h
    include/canvasmediator.h
    include/input_sketch.h
    include/halfedge.h
    include/openglcanvas.h
#    include/canvas2.h
#    include/scene2.h
#    include/input_sketch2.h
    include/layers.h
    include/sketch_library.h
    include/openglmediator.h

)

# Populate a cmake variable with the project's headers dir
set(HEADERS_DIR ${CMAKE_SOURCE_DIR}/include)
# Populate a cmake variable with the external libs' headers dir
set(EXTERNAL_LIBS_DIR ${CMAKE_SOURCE_DIR}/libs)


# Tell cmake to create the canvas executable
add_executable(Canvas WIN32 ${SOURCES} resources.qrc)

# Make the project's header dirs visible to building tools
target_include_directories(Canvas PUBLIC ${HEADERS_DIR})
target_include_directories(Canvas SYSTEM PRIVATE ${EXTERNAL_LIBS_DIR})

# Use Core, Gui and Widgets modules from Qt5
target_link_libraries(Canvas Qt5::Core Qt5::Gui Qt5::Widgets)

# Use tucanow wrapper
target_link_libraries(Canvas tucanow)

###############################################
# Copy shaders to binary dir when done
###############################################

add_custom_command(TARGET Canvas POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/shaders
        ${CMAKE_CURRENT_BINARY_DIR}/shaders
  )

###############################################
# Use windeployqt.exe to copy Qt5 resources

# https://blog.quickmediasolutions.com/2017/11/24/using-windeployqt-with-cpack.html
###############################################

if(WIN32 AND SKETCH_CANVAS_DEPLOY_QT)

    # Find windeployqt.exe
    get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")

    # Run windeployqt
    add_custom_command(TARGET Canvas POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E
        env PATH="${_qt_bin_dir}" "${WINDEPLOYQT_EXECUTABLE}"
        # --verbose 0
        --no-compiler-runtime
        \"$<TARGET_FILE:Canvas>\"
        COMMENT "Deploying Qt..."
        )

endif()
