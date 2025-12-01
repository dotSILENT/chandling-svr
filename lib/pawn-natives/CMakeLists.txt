cmake_minimum_required(VERSION 3.1)

if (NOT MAIN_RUN)
		project(dependencies)

	    #if(WIN32)
   		#    set(STDOUT_DEVICE CON)
        #    set(STDERR_DEVICE CON)
        #else()
        #    set(STDOUT_DEVICE /dev/stdout)
        #    set(STDERR_DEVICE /dev/stderr)
        #endif()

		set(SAMPGDK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/sampgdk")
		set(SAMPGDK_BUILD_DIR "${SAMPGDK_DIR}/build")

		if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/lib")
			file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib")
        endif()

        if(NOT EXISTS "${SAMPGDK_DIR}")
    		message(STATUS "Cloning sampgdk repository...")
			execute_process(
        		COMMAND wsl.exe git clone --recursive --depth 1 -b master https://github.com/Just-Archives/sampgdk.git
       			WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib"
        		RESULT_VARIABLE GIT_CLONE_RESULT
    			#OUTPUT_FILE ${STDOUT_DEVICE}
    			#ERROR_FILE ${STDERR_DEVICE}
    		)
    		if(NOT GIT_CLONE_RESULT EQUAL 0)
       		message(FATAL_ERROR "Failed to clone sampgdk repository")
   			endif()
		endif()

		file(MAKE_DIRECTORY "${SAMPGDK_BUILD_DIR}")

        if(NOT TARGET sampgdk)
        	add_library(sampgdk
					"${CMAKE_CURRENT_SOURCE_DIR}/lib/sampgdk/include/sampgdk/a_objects.h"
					"${CMAKE_CURRENT_SOURCE_DIR}/lib/sampgdk/include/sampgdk/a_players.h"
					"${CMAKE_CURRENT_SOURCE_DIR}/lib/sampgdk/include/sampgdk/a_samp.h"
					"${CMAKE_CURRENT_SOURCE_DIR}/lib/sampgdk/include/sampgdk/a_vehicles.h"
					"${CMAKE_CURRENT_SOURCE_DIR}/lib/sampgdk/include/sampgdk/amx.h"
					"${CMAKE_CURRENT_SOURCE_DIR}/lib/sampgdk/include/sampgdk/plugin.h"
					"${CMAKE_CURRENT_SOURCE_DIR}/lib/sampgdk/include/sampgdk/plugincommon.h"
					"${CMAKE_CURRENT_SOURCE_DIR}/lib/sampgdk/include/sampgdk/config.h"
					"${CMAKE_CURRENT_SOURCE_DIR}/lib/sampgdk/include/sampgdk/core.h"
					"${CMAKE_CURRENT_SOURCE_DIR}/lib/sampgdk/include/sampgdk/export.h"
					"${CMAKE_CURRENT_SOURCE_DIR}/lib/sampgdk/include/sampgdk/version.h"
					"${CMAKE_CURRENT_SOURCE_DIR}/lib/sampgdk/src/a_objects.cpp"
					"${CMAKE_CURRENT_SOURCE_DIR}/lib/sampgdk/src/a_players.cpp"
					"${CMAKE_CURRENT_SOURCE_DIR}/lib/sampgdk/src/a_samp.cpp"
					"${CMAKE_CURRENT_SOURCE_DIR}/lib/sampgdk/src/a_vehicles.cpp"
					"${CMAKE_CURRENT_SOURCE_DIR}/lib/sampgdk/src/amxhooks.cpp"
					"${CMAKE_CURRENT_SOURCE_DIR}/lib/sampgdk/src/amxhooks.h"
					"${CMAKE_CURRENT_SOURCE_DIR}/lib/sampgdk/src/amxplugin.cpp"
					"${CMAKE_CURRENT_SOURCE_DIR}/lib/sampgdk/src/callbacks.cpp"
					"${CMAKE_CURRENT_SOURCE_DIR}/lib/sampgdk/src/callbacks.h"
					"${CMAKE_CURRENT_SOURCE_DIR}/lib/sampgdk/src/core.cpp"
					"${CMAKE_CURRENT_SOURCE_DIR}/lib/sampgdk/src/fakeamx.cpp"
					"${CMAKE_CURRENT_SOURCE_DIR}/lib/sampgdk/src/fakeamx.h"
					"${CMAKE_CURRENT_SOURCE_DIR}/lib/sampgdk/src/jump-x86.cpp"
					"${CMAKE_CURRENT_SOURCE_DIR}/lib/sampgdk/src/jump-x86.h"
					"${CMAKE_CURRENT_SOURCE_DIR}/lib/sampgdk/src/natives.cpp"
					"${CMAKE_CURRENT_SOURCE_DIR}/lib/sampgdk/src/natives.h"
					"${CMAKE_CURRENT_SOURCE_DIR}/lib/sampgdk/src/sampgdk.rc"
					"${CMAKE_CURRENT_SOURCE_DIR}/lib/sampgdk/src/timers.cpp"
					"${CMAKE_CURRENT_SOURCE_DIR}/lib/sampgdk/src/timers.h"
					"${CMAKE_CURRENT_SOURCE_DIR}/lib/sampgdk/src/version.cpp"
        	)
			target_include_directories(sampgdk PUBLIC
					"${CMAKE_CURRENT_SOURCE_DIR}/lib/sampgdk/src/sampgdk/include"
					"${CMAKE_CURRENT_SOURCE_DIR}/lib/sampgdk/src/sampgdk-build/include"
			)
		endif()

    	if(MSVC)
        	set(SAMPGDK_LINKER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/sampgdk/src/sampgdk.def")

        	if(EXISTS "${SAMPGDK_LINKER_DIR}")
			    message(STATUS "Applying linker tweaks for sampgdk...")
            	file(TO_NATIVE_PATH "${SAMPGDK_LINKER_DIR}" SAMPGDK_LINKER_DIR_NATIVE)
            	string(REPLACE "\\" "\\\\" SAMPGDK_LINKER_DIR_NATIVE "${SAMPGDK_LINKER_DIR_NATIVE}")
            	set(SAMPGDK_DEF_FLAG "/DEF:\"${SAMPGDK_LINKER_DIR_NATIVE}\"")

         	    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${SAMPGDK_DEF_FLAG}")
       	        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${SAMPGDK_DEF_FLAG}")

      	        target_link_options(sampgdk PRIVATE "${SAMPGDK_DEF_FLAG}")
     	        set_target_properties(sampgdk PROPERTIES LINK_DEPENDS "${SAMPGDK_LINKER_DIR_NATIVE}")
        	else()
            	message(WARNING "sampgdk.def not found")
        	endif()
    		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /machine:${CMAKE_GENERATOR_PLATFORM}")
    	endif()

		message(STATUS "Configuring sampgdk...")
		if(Win32)
			execute_process(
   				COMMAND ${CMAKE_COMMAND} -DCMAKE_POLICY_VERSION_MINIMUM=${CMAKE_POLICY_VERSION_MINIMUM} -DSAMP_SDK_ROOT=${PROJECT_SOURCE_DIR}/lib/amx -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -B "${SAMPGDK_BUILD_DIR}" -A ${CMAKE_GENERATOR_PLATFORM} -DCMAKE_EXE_LINKER_FLAGS="${CMAKE_EXE_LINKER_FLAGS}"
  		    	WORKING_DIRECTORY "${SAMPGDK_DIR}"
 		  		RESULT_VARIABLE CONFIGURE_RESULT
    			#OUTPUT_FILE ${STDOUT_DEVICE}
       	  		#ERROR_FILE ${STDERR_DEVICE}
			)
		else()
		    execute_process(
   				COMMAND ${CMAKE_COMMAND} -DCMAKE_POLICY_VERSION_MINIMUM=${CMAKE_POLICY_VERSION_MINIMUM} -DSAMP_SDK_ROOT=${PROJECT_SOURCE_DIR}/lib/amx -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -B "${SAMPGDK_BUILD_DIR}" -A ${CMAKE_GENERATOR_PLATFORM}
  		    	WORKING_DIRECTORY "${SAMPGDK_DIR}"
 		  		RESULT_VARIABLE CONFIGURE_RESULT
    			#OUTPUT_FILE ${STDOUT_DEVICE}
       	 	    #ERROR_FILE ${STDERR_DEVICE}
			)
		endif()
		if(NOT CONFIGURE_RESULT EQUAL 0)
    		 message(FATAL_ERROR "CMake configure failed for sampgdk")
		endif()

		message(STATUS "Building sampgdk...")
   		execute_process(
       		 COMMAND ${CMAKE_COMMAND} --build "${SAMPGDK_BUILD_DIR}" --config ${CMAKE_BUILD_TYPE}
    	     WORKING_DIRECTORY "${SAMPGDK_DIR}"
       	     RESULT_VARIABLE BUILD_RESULT
    		 #OUTPUT_FILE ${STDOUT_DEVICE}
    		 #ERROR_FILE ${STDERR_DEVICE}
  		)

		if(NOT BUILD_RESULT EQUAL 0)
  		  message(FATAL_ERROR "Failed to build sampgdk")
		endif()

		add_custom_command(TARGET sampgdk POST_BUILD
				COMMAND "${CMAKE_COMMAND}" -DCMAKE_POLICY_VERSION_MINIMUM=${CMAKE_POLICY_VERSION_MINIMUM} "${CMAKE_SOURCE_DIR}" -DMAIN_RUN=TRUE -A ${CMAKE_GENERATOR_PLATFORM}
				COMMAND "${CMAKE_COMMAND}" --build "${CMAKE_SOURCE_DIR}"
				COMMAND "${CMAKE_COMMAND}" --build "${CMAKE_SOURCE_DIR}" --config ${CMAKE_BUILD_TYPE}
		)
else ()
		file(GLOB PAWNN_HPP "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
		file(GLOB PAWNNI_HPP "${CMAKE_CURRENT_SOURCE_DIR}/Internal/*.hpp")
		add_library(pawn-natives ${PAWNN_HPP} ${PAWNNI_HPP})
		set_target_properties(pawn-natives PROPERTIES LINKER_LANGUAGE CXX)
		if(DEFINED UNIX)
		target_compile_options(pawn-natives PRIVATE -m32)
		target_link_options(pawn-natives PRIVATE -m32)
		endif()

		if(NOT TARGET AMX)
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
		endif()

		add_subdirectory(${PROJECT_SOURCE_DIR}/lib/subhook)

		target_compile_definitions(pawn-natives -D_CRT_SECURE_NO_WARNINGS -DSUBHOOK_IMPLEMENTATION -DHAVE_STDINT_H=1)

		if (WIN32)
				target_link_libraries(pawn-natives
						"${CMAKE_CURRENT_SOURCE_DIR}/sampgdk/src/sampgdk-build/Release/sampgdk4.lib"
				)

				set_target_properties(pawn-natives PROPERTIES LINKER_LANGUAGE CXX)
		else ()
				target_link_libraries(pawn-natives
						"${CMAKE_CURRENT_SOURCE_DIR}/sampgdk/src/sampgdk-build/libsampgdk.so"
				)

				set(CMAKE_CXX_FLAGS "-std=gnu++0x")
		endif ()
endif ()
