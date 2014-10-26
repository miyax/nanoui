cmake_minimum_required(VERSION 2.8.8)

#---------------------------------------------------------------
# Project
#---------------------------------------------------------------

PROJECT(nanoui)
set(LIBRARY_NAME nanoui)

SET(version_major 0)
SET(version_minor 1)
SET(version_patch 1)

configure_file (
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
)

#---------------------------------------------------------------
# Platform
#---------------------------------------------------------------
if (MSVC)

set( PLATFORM_INCLUDE_DIR
  ${PROJECT_SOURCE_DIR}/platforms/windows/include
)

add_definitions(-DNANOVG_GLEW )
add_definitions(-DNANOVG_GL3_IMPLEMENTATION )

elseif (ANDROID)

elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

set( PLATFORM_INCLUDE_DIR
  ${PROJECT_SOURCE_DIR}/platforms/linux/include
)

set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -g -fPIC" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11  -g -fPIC" )

add_definitions(-DNANOVG_GL3_IMPLEMENTATION )

endif ()

#---------------------------------------------------------------
# Defines
#---------------------------------------------------------------

# include
set(INCLUDE_DIR
    ${PROJECT_SOURCE_DIR}
    ${PLATFORM_INCLUDE_DIR}
)

set (SOURCES
    main.cpp
    nanoui.cpp
    nanovg.c
)

include_directories(${INCLUDE_DIR})

add_executable(
  ${LIBRARY_NAME}
  ${SOURCES}
)


#---------------------------------------------------------------
# Sub Modules
#---------------------------------------------------------------


#---------------------------------------------------------------
# Link
#---------------------------------------------------------------
if (MSVC)

set (EXTERNAL_LIBS
   ${PROJECT_SOURCE_DIR}/platforms/windows/lib/glfw3.lib
   ${PROJECT_SOURCE_DIR}/platforms/windows/lib/glew32s.lib
   opengl32
)

elseif (ANDROID)


elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

  set (EXTERNAL_LIBS
     GL
     GLU
     m
     GLEW
     glfw3
     Xxf86vm
     pthread
     X11
     Xrandr
     Xinerama
     Xi
     Xcursor
     rt
  )

endif (MSVC)

target_link_libraries(
  ${LIBRARY_NAME}
  ${EXTERNAL_LIBS}
  )
