cmake_minimum_required(VERSION 3.2)
project(Corium)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -pthread -O3")
# -Wl,-rpath='$ORIGIN/lib/',-z,origin

###############
# FIND OPENGL #
###############

find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})
if(NOT OPENGL_FOUND)
    message(ERROR " OPENGL not found!")
endif(NOT OPENGL_FOUND)

# Set Win32 settings
IF(WIN32)
SET(GUI_TYPE WIN32)
ENDIF(WIN32)
# Set MacOS settings
IF (APPLE)
SET(GUI_TYPE MACOSX_BUNDLE)
ENDIF (APPLE)

set(CMAKE_SKIP_BUILD_PATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH "${CMAKE_CURRENT_SOURCE_DIR}/lib/")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

################
# SOURCE FILES #
################

set(SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

file(GLOB_RECURSE SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(
Corium ${SOURCE_FILES} ${SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/src/resource.rc
)

########
# LINK #
########

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/include)

if (WIN32)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/win/include)

set_target_properties(Corium PROPERTIES LINK_FLAGS -Wl,-subsystem,windows) # hide console

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/win/lib)

target_link_libraries(Corium ${CMAKE_CURRENT_SOURCE_DIR}/glew32.dll)

target_link_libraries(Corium
${CMAKE_CURRENT_SOURCE_DIR}/lib/win/lib/libglew32.a
${CMAKE_CURRENT_SOURCE_DIR}/lib/win/lib/libglfw3.a
${CMAKE_CURRENT_SOURCE_DIR}/lib/win/lib/libopengl32.a
)
else ()
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/linux/include)

set_target_properties(Corium PROPERTIES LINK_FLAGS -Wl,-rpath=$ORIGIN/lib,-z,origin)

target_link_libraries(Corium GLEW glfw GL)
endif ()

target_link_libraries(Corium "-Wl,--allow-multiple-definition") # solve later
