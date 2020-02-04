###############################################
# Script options
###############################################

option(GLEW_BUILD_x64 "Use glew x64 library" ON)


###############################################
# Download glew and extract it to GLEW_DIR
###############################################

function(DownloadGLEW2 LIBS_DIR GLEW_DIR)
    set(GLEW_URL "https://downloads.sourceforge.net/project/glew/glew/2.1.0/glew-2.1.0-win32.zip")
    set(GLEW_PATH "${LIBS_DIR}/glew-2.1.0")
    set(GLEW_ZIP_PATH "${LIBS_DIR}/glew-2.1.0.zip")

    if (NOT EXISTS "${GLEW_PATH}")
        file(MAKE_DIRECTORY "${LIBS_DIR}")

        message(STATUS "Downloading glew...")
        file(DOWNLOAD "${GLEW_URL}" "${GLEW_ZIP_PATH}")

        message(STATUS "Uncompressing glew...")
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E tar "xf" "${GLEW_ZIP_PATH}" --format=zip
            WORKING_DIRECTORY ${LIBS_DIR}
        )

        file(REMOVE "${GLEW_ZIP_PATH}")
    endif()

    set(GLEW_DIR ${GLEW_PATH} PARENT_SCOPE)

endfunction(DownloadGLEW2)

macro(GetGLEW2)

    ###############################################
    # Set env vars and download glew
    ###############################################

    if (NOT GLEW_PARENT_DIR)
        set(GLEW_PARENT_DIR "${CMAKE_CURRENT_LIST_DIR}")
    endif()

    DownloadGLEW2(${GLEW_PARENT_DIR} GLEW_DIR)


    ###############################################
    # Find include dir
    ###############################################

    find_path(GLEW_INCLUDE_DIRS GL/glew.h HINTS ${GLEW_DIR}/include)

    if (GLEW_INCLUDE_DIRS)
        set(GLEW_INCLUDE_DIR ${GLEW_INCLUDE_DIRS})
        message(STATUS "Found GLEW include dir: ${GLEW_INCLUDE_DIRS}")

    else()
        message(FATAL_ERROR "Could not find GLEW include dir -- abort")

    endif()


    ###############################################
    # Find library
    ###############################################

    if (GLEW_BUILD_x64)
        find_library(GLEW_LIBRARIES NAMES glew32 HINTS "${GLEW_DIR}/lib/Release/x64")
        find_program(GLEW_RUNTIME NAMES glew32.dll HINTS "${GLEW_DIR}/bin/Release/x64")
    else()
        find_library(GLEW_LIBRARIES NAMES glew32 HINTS "${GLEW_DIR}/lib/Release/Win32")
        find_library(GLEW_RUNTIME NAMES glew32.dll HINTS "${GLEW_DIR}/bin/Release/Win32")
    endif()

    if (GLEW_LIBRARIES)
        set(GLEW_LIBRARY ${GLEW_LIBRARIES})
        message(STATUS "Found GLEW library: ${GLEW_LIBRARIES}")
        message(STATUS "Found GLEW runtime: ${GLEW_RUNTIME}")
    else()
        message(FATAL_ERROR "Could not find GLEW library -- abort")
    endif()


    ###############################################
    # Create library interface
    ###############################################

    if (NOT TARGET GLEW) 
        add_library(GLEW INTERFACE)
        target_include_directories(GLEW INTERFACE ${GLEW_INCLUDE_DIRS})
        target_link_libraries(GLEW INTERFACE ${GLEW_LIBRARIES})

        add_library(GLEW::GLEW ALIAS GLEW)

        set(GLEW_FOUND TRUE)
    endif()


    ###############################################
    # Copy dll to build dir
    ###############################################

    execute_process(
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${GLEW_RUNTIME} ${CMAKE_BINARY_DIR}
    )


endmacro(GetGLEW2)
