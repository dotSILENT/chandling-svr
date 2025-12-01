include(AMXConfig)
include(AddSAMPPlugin)

include_directories("${PROJECT_SOURCE_DIR}/src")

add_samp_plugin(${PROJECT_NAME}
  CPlayer.cpp
  CPlayer.h
  Natives.hpp
  Hooks.hpp
  Actions.cpp
  Actions.h
  HandlingEnum.cpp
  HandlingEnum.h
  HandlingManager.cpp
  HandlingManager.h
  HandlingDefault.cpp
  HandlingDefault.h
  HandlingStruct.h
  PacketEnum.h
  chandlingsvr.cpp
  chandlingsvr.h
)

if(NOT WIN32)
	# undefined symbol clock_gettime
	target_link_libraries(${PROJECT_NAME} rt)
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC OMP-SDK)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/lib)
