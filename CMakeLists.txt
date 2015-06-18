cmake_minimum_required(VERSION 3.0)
project(tanks)

INCLUDE(FindPkgConfig)

set(CHAISCRIPT_INCLUDE_DIRS /home/shybovycha/projects/forks/ChaiScript/include)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    src/ObjectTextDisplay.cpp
    src/SkeletonDebug.cpp
    src/TanksApplication.cpp)

add_executable(tanks ${SOURCE_FILES})

pkg_search_module(OGRE REQUIRED OGRE)
pkg_search_module(OIS REQUIRED OIS)
# pkg_search_module(BOOST COMPONENTS system REQUIRED boost)

include_directories(${OGRE_INCLUDE_DIRS}
    ${OGRE_INCLUDE_DIRS}/Overlay
    ${OIS_INCLUDE_DIRS}
    ${CHAISCRIPT_INCLUDE_DIRS})

link_directories(${OGRE_LIBRARY_DIRS}
    ${OIS_LIBRARY_DIRS})

target_link_libraries(tanks ${OGRE_LIBRARIES} ${OIS_LIBRARIES})
target_link_libraries(tanks OgreOverlay boost_system dl)