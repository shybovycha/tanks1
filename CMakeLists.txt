cmake_minimum_required(VERSION 3.0)
project(tanks)

INCLUDE(FindPkgConfig)

pkg_check_modules(required OGRE OIS)

message("OGRE home: ${OGRE_HOME}")

set(CMAKE_MODULE_PATH "${OGRE_HOME}/cmake/;${CMAKE_MODULE_PATH}")

find_package(OGRE REQUIRED)
find_package(OIS REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    src/ExampleApplication.h
    src/ExampleFrameListener.h
    src/ExampleLoadingBar.h
    src/InputContext.h
    src/ObjectTextDisplay.cpp
    src/ObjectTextDisplay.h
    src/SdkCameraMan.h
    src/SdkTrays.h
    src/SkeletonDebug.cpp
    src/SkeletonDebug.h
    src/WorkingWithOgre.cpp
    src/WorkingWithOgre.h)

include_directories(${OGRE_INCLUDE_DIRS}
    ${OGRE_INCLUDE_DIRS}/Overlay
    ${OIS_INCLUDE_DIRS})

link_directories(${OGRE_LIBRARY_DIRS}
    ${OIS_LIBRARY_DIRS})

add_executable(tanks ${SOURCE_FILES})

target_link_libraries(tanks ${OGRE_LIBRARIES} ${OIS_LIBRARIES})
target_link_libraries(tanks OgreOverlay)