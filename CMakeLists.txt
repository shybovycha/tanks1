cmake_minimum_required(VERSION 3.0)
project(tanks)

INCLUDE(FindPkgConfig)

#find_package(OGRE)
#find_package(OIS)

#message("OGRE include dir: `${OGRE_DIR}`")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    src/ObjectTextDisplay.cpp
    src/SkeletonDebug.cpp
    src/WorkingWithOgre.cpp)

add_executable(tanks ${SOURCE_FILES})

pkg_search_module(OGRE REQUIRED OGRE)
pkg_search_module(OIS REQUIRED OIS)
# pkg_search_module(BOOST COMPONENTS system REQUIRED boost)

include_directories(${OGRE_INCLUDE_DIRS}
    ${OGRE_INCLUDE_DIRS}/Overlay
    ${OIS_INCLUDE_DIRS})

link_directories(${OGRE_LIBRARY_DIRS}
    ${OIS_LIBRARY_DIRS})

target_link_libraries(tanks ${OGRE_LIBRARIES} ${OIS_LIBRARIES})
target_link_libraries(tanks OgreOverlay boost_system)