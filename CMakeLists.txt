project(chandlingsvr)

cmake_minimum_required(VERSION 2.8)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(AMXConfig)
include(AddSAMPPlugin)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/amx
  ${CMAKE_CURRENT_SOURCE_DIR}/subhook
)

add_definitions(-DSAMPGDK_AMALGAMATION -DSUBHOOK_STATIC)

add_samp_plugin(chandlingsvr
  amxplugin.cpp
  BitStream.h
  BitStream.cpp
  subhook/subhook.c
  subhook/subhook.h
  Hooks.cpp
  Hooks.h
  utils.h
  raknet.h
  PacketEnum.h
  chandlingsvr.cpp
  chandlingsvr.def
  sampgdk.c
  sampgdk.h
)