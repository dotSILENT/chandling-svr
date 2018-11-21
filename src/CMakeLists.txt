include(AMXConfig)
include(AddSAMPPlugin)

set(SAMPSDK_DIR "${PROJECT_SOURCE_DIR}/lib/samp-plugin-sdk")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")
set(LIB_DIR "${PROJECT_SOURCE_DIR}/lib")

include_directories(
  ${SAMPSDK_DIR}
  ${LIB_DIR}
  ${SRC_DIR}
)

set(LIB_SAMPGDK
  "${LIB_DIR}/sampgdk/sampgdk.h"
  "${LIB_DIR}/sampgdk/sampgdk.c"
)

set(LIB_RAKNET
  "${LIB_DIR}/raknet/raknet.h"
  "${LIB_DIR}/raknet/BitStream.h"
  "${LIB_DIR}/raknet/BitStream.cpp"
)
 
set(LIB_SUBHOOK
  "${LIB_DIR}/subhook/subhook.h"
  "${LIB_DIR}/subhook/subhook.c"
)

SET(LIB_SAMPSDK
  "${SAMPSDK_DIR}/plugincommon.h"
  "${SAMPSDK_DIR}/amx/amx.h"
  "${SAMPSDK_DIR}/amxplugin.cpp"
)

add_samp_plugin(chandlingsvr
  ${PROJECT_SOURCE_DIR}/chandlingsvr.def
  ${LIB_SAMPSDK}
  ${LIB_SAMPGDK}
  ${LIB_RAKNET}
  ${LIB_SUBHOOK}
  CPlayer.cpp
  CPlayer.h
  Natives.cpp
  Natives.h
  Hooks.cpp
  Hooks.h
  Actions.cpp
  Actions.h
  HandlingEnum.cpp
  HandlingEnum.h
  HandlingManager.cpp
  HandlingManager.h
  HandlingDefault.cpp
  HandlingDefault.h
  HandlingStruct.h
  utils.h
  PacketEnum.h
  chandlingsvr.cpp
  chandlingsvr.h
)

source_group("lib\\sampgdk" FILES ${LIB_SAMPGDK})
source_group("lib\\subhook" FILES ${LIB_SUBHOOK})
source_group("lib\\raknet" FILES ${LIB_RAKNET})
source_group("lib\\samp-plugin-sdk" FILES ${LIB_SAMPSDK})