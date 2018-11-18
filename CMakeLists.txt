cmake_minimum_required(VERSION 3.0)

project(chandlingsvr)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/lib/cmake)


add_definitions(-DSAMPGDK_AMALGAMATION -DSUBHOOK_STATIC)

add_subdirectory(src)
