cmake_minimum_required( VERSION 3.21 ) # for vcpkg integration

set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/install" CACHE STRING "Install dir")

set(CMAKE_CXX_STANDARD 17)

project(touch_gestures)

if(MSVC)
    set(CMAKE_SYSTEM_VERSION "10.0 (latest installed version)")
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
    add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
    add_definitions(-DNOMINMAX)
    add_compile_definitions(/MP /arch:AVX2 /fp:fast)
endif()

#######################################################################

set(PROJ_SOURCES
    gestures.hpp
    touch_gestures.hpp
    touch_gestures.cpp
    )
add_executable(${PROJECT_NAME} ${PROJ_SOURCES})
