cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
cmake_policy(SET CMP0115 NEW)

project(chandlingsvr
        LANGUAGES CXX
)
enable_language(C)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/lib/cmake)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(${PROJECT_SOURCE_DIR}/lib/omp-sdk)
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/subhook)
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/pawn-natives)

file(GLOB AMX_SRC "${PROJECT_SOURCE_DIR}/lib/amx/*.c")
if(DEFINED UNIX)
    file(GLOB AMXL_SRC "${PROJECT_SOURCE_DIR}/lib/amx/linux/*.c")
endif()
file(GLOB AMX_HDR "${PROJECT_SOURCE_DIR}/lib/amx/*.h")
if(DEFINED UNIX)
    file(GLOB AMXL_HDR "${PROJECT_SOURCE_DIR}/lib/amx/linux/*.h")
endif()
add_library(AMX ${AMX_SRC} ${AMXL_SRC} ${AMX_HDR} ${AMXL_HDR})
set_target_properties(AMX PROPERTIES LINKER_LANGUAGE CXX)
if(DEFINED UNIX)
target_compile_options(AMX PRIVATE -m32)
target_link_options(AMX PRIVATE -m32)
endif()
target_include_directories(AMX
        INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/lib/amx>
        $<INSTALL_INTERFACE:amx>
)

file(GLOB RakNet_SRC "${PROJECT_SOURCE_DIR}/lib/RakNet/*.cpp")
file(GLOB RakNetE_SRC "${PROJECT_SOURCE_DIR}/lib/RakNet/Encoding/*.cpp")
file(GLOB RakNet_HDR "${PROJECT_SOURCE_DIR}/lib/RakNet/*.hpp")
file(GLOB RakNetE_HDR "${PROJECT_SOURCE_DIR}/lib/RakNet/Encoding/*.hpp")
add_library(RakNet ${RakNet_SRC} ${RakNetE_SRC} ${RakNet_HDR} ${RakNetE_HDR})
set_target_properties(RakNet PROPERTIES LINKER_LANGUAGE CXX)
if(DEFINED UNIX)
target_compile_options(RakNet PRIVATE -m32)
target_link_options(RakNet PRIVATE -m32)
endif()
target_include_directories(RakNet
        INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/lib/RakNet>
        $<INSTALL_INTERFACE:RakNet>
)

add_definitions(-DSUBHOOK_STATIC -DSUBHOOK_IMPLEMENTATION)
add_definitions(-DHAVE_STDINT_H=1 -DPAWN_CELL_SIZE=64)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

add_subdirectory(${PROJECT_SOURCE_DIR}/src)

set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY LINK_FLAGS "/DEF:\"${CMAKE_CURRENT_BINARY_DIR}/plugin.def\"")
