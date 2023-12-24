cmake_minimum_required( VERSION 3.21 ) # for vcpkg integration

set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/install" CACHE STRING "Install dir")

set(CMAKE_CXX_STANDARD 20)

project(touch_gestures)

if(MSVC)
    set(CMAKE_SYSTEM_VERSION "10.0 (latest installed version)")
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    add_compile_definitions(_CRT_SECURE_NO_DEPRECATE)
    add_compile_definitions(_CRT_NONSTDC_NO_DEPRECATE)
    add_compile_definitions(WIN32_LEAN_AND_MEAN)
    add_compile_definitions(NOMINMAX)
    add_compile_options(
        $<$<COMPILE_LANGUAGE:CXX>:-MP>
        $<$<COMPILE_LANGUAGE:CXX>:-arch:AVX2>
        $<$<COMPILE_LANGUAGE:CXX>:-fp:fast>
    )
endif()

#######################################################################

set(PROJ_SOURCES
    gestures.hpp
    touch_gestures.hpp
    touch_gestures.cpp
    )
add_executable(${PROJECT_NAME} ${PROJ_SOURCES})
