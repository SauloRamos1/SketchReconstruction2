cmake_minimum_required(VERSION 3.10)
project(GetWin32ExternalLibraries)

find_package(GLEW QUIET)
if(NOT GLEW_FOUND)
    include(${CMAKE_CURRENT_SOURCE_DIR}/get_glew-2.1.0_win32.cmake)
    GetGLEW2()
endif()

find_package(Eigen3 QUIET)
if(NOT EIGEN3_FOUND)
    include(${CMAKE_CURRENT_SOURCE_DIR}/get_eigen-3.3.7_win32.cmake)
    GetEigen3()
endif()
