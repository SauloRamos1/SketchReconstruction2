###############################################
# Script options
###############################################


###############################################
# Download Eigen3 and extract it to EIGEN_DIR
###############################################

function(DownloadEigen3 LIBS_DIR EIGEN_DIR)

    set(EIGEN_URL https://github.com/eigenteam/eigen-git-mirror)
    set(EIGEN_PATH "${LIBS_DIR}/eigen3")

    if (NOT EXISTS "${EIGEN_PATH}")
        file(MAKE_DIRECTORY "${LIBS_DIR}")

        message(STATUS "Cloning eigen3 from <${EIGEN_URL}>...")
        execute_process(
            COMMAND git clone ${EIGEN_URL} ${LIBS_DIR}/eigen3
        )

    file(REMOVE "${EIGEN_ZIP_PATH}")
    endif()

    set(EIGEN_DIR ${EIGEN_PATH} PARENT_SCOPE)
endfunction(DownloadEigen3)


macro(GetEigen3)

    ###############################################
    # Set env vars and download Eigen3
    ###############################################

    if (NOT EIGEN_PARENT_DIR)
        set(EIGEN_PARENT_DIR "${CMAKE_CURRENT_LIST_DIR}")
    endif()

    DownloadEigen3(${EIGEN_PARENT_DIR} EIGEN_DIR)


    ###############################################
    # Find include dir
    ###############################################

    set(EIGEN3_INCLUDE_DIR "${EIGEN_DIR}")

    if (EIGEN3_INCLUDE_DIR)
        message(STATUS "Found Eigen3 include dir: ${EIGEN3_INCLUDE_DIR}")
    else()
        message(FATAL_ERROR "Could not find Eigen3 include dir -- abort")
    endif()


    ###############################################
    # Create library interface
    ###############################################

    if(NOT TARGET Eigen3)
        add_library(Eigen3 INTERFACE)
        target_include_directories(Eigen3 INTERFACE ${EIGEN3_INCLUDE_DIR})

        add_library(Eigen3::Eigen ALIAS Eigen3)

        set(Eigen3_FOUND TRUE)
    endif()

endmacro(GetEigen3)
