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
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -g -fPIC" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11  -g -fPIC" )
		
#---------------------------------------------------------------
# Defines
#---------------------------------------------------------------

# include
set(INCLUDE_DIR
    ${PROJECT_SOURCE_DIR}
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

target_link_libraries(
  ${LIBRARY_NAME}
  ${EXTERNAL_LIBS}  
  )
    
 


